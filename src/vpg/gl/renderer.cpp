#include <vpg/gl/renderer.hpp>
#include <vpg/ecs/transform.hpp>
#include <vpg/gl/debug.hpp>

#include <GL/glew.h>
#include <random>

#define LIGHT_COUNT 64

using namespace vpg;
using namespace vpg::gl;

vpg::gl::Renderer::Renderer(CameraSystem* camera_sys, LightSystem* light_sys, RenderableSystem* renderable_sys) {
    this->size = input::Window::get_framebuffer_size();
    this->camera_sys = camera_sys;
    this->light_sys = light_sys;
    this->renderable_sys = renderable_sys;

    this->gbuffer.shader = data::Manager::load<data::Shader>("shader.gbuffer");
    this->gbuffer.shader->get_shader().bind_uniform_buffer("Lights", 0);
    this->ssao.shader = data::Manager::load<data::Shader>("shader.ssao");
    this->ssao_blur.shader = data::Manager::load<data::Shader>("shader.ssao_blur");
    this->model_shader = data::Manager::load<data::Shader>("shader.model");
    this->model_shader->get_shader().bind_uniform_buffer("Palette", 0);

    this->wireframe = false;
    this->debug_lights = false;
    this->debug_rendering = false;

    glGenVertexArrays(1, &this->screen_va);

    glGenBuffers(1, &this->lights_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, this->lights_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData) * LIGHT_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    this->create_gbuffer();
    this->create_ssao();

    this->resize_listener = input::Window::FramebufferResized.add_listener(
        std::bind(&Renderer::resize_callback, this, std::placeholders::_1)
    );
    this->debug_render_toggle_listener = input::Keyboard::Down.add_listener(
        std::bind(&Renderer::debug_render_toggle_callback, this, std::placeholders::_1)
    );
}

vpg::gl::Renderer::~Renderer() {
    input::Window::FramebufferResized.remove_listener(this->resize_listener);
    input::Keyboard::Down.remove_listener(this->debug_render_toggle_listener);

    this->destroy_ssao();
    this->destroy_gbuffer();

    glDeleteBuffers(1, &this->lights_ubo);
    glDeleteVertexArrays(1, &this->screen_va);
}

void vpg::gl::Renderer::render(float dt) {
    // Get camera
    if (this->camera_sys->entities.empty()) {
        return;
    }
    auto& camera = *ecs::Coordinator::get_component<Camera>(*this->camera_sys->entities.begin());
    camera.set_aspect_ratio((float)this->size.x / (float)this->size.y);
    camera.update();

    // Update lights UBO
    glBindBuffer(GL_UNIFORM_BUFFER, this->lights_ubo);
    auto lights = (LightData*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    int light_index = 0;
    for (auto& entity : this->light_sys->entities) {
        auto& transform = *ecs::Coordinator::get_component<ecs::Transform>(entity);
        auto& light = *ecs::Coordinator::get_component<Light>(entity);

        switch (light.type) {
        case Light::Type::Directional:
            lights[light_index].ambient = glm::vec4(light.ambient, 1.0f);
            lights[light_index].diffuse = glm::vec4(light.diffuse, 1.0f);
            lights[light_index].direction = camera.get_view() * glm::vec4(transform.get_global_rotation() * glm::vec3(0.0f, 0.0f, 1.0f), 0.0f);
            break;
        case Light::Type::Point:
            if (this->debug_lights) {
                gl::Debug::draw_sphere(transform.get_global_position(), 1.0f, lights[light_index].diffuse);
            }
            lights[light_index].position = camera.get_view() * glm::vec4(transform.get_global_position(), 1.0f);
            lights[light_index].direction.w = 1.0f;
            lights[light_index].constant = light.constant;
            lights[light_index].linear = light.linear;
            lights[light_index].quadratic = light.quadratic;
            lights[light_index].ambient = glm::vec4(light.ambient, 1.0f);
            lights[light_index].diffuse = glm::vec4(light.diffuse, 1.0f);
            break;
        }

        light_index += 1;
    }
    for (; light_index < LIGHT_COUNT; ++light_index) {
        lights[light_index].ambient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        lights[light_index].diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glViewport(0, 0, this->size.x, this->size.y);

    // Opaque pass
    glBindFramebuffer(GL_FRAMEBUFFER, this->gbuffer.fbo);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glPolygonMode(GL_FRONT_AND_BACK, this->wireframe ? GL_LINE : GL_FILL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // Clear framebuffer
    GLuint draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, &draw_buffers[0]);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto model_loc = this->model_shader->get_shader().get_uniform_location("model");
    auto view_loc = this->model_shader->get_shader().get_uniform_location("view");
    auto proj_loc = this->model_shader->get_shader().get_uniform_location("proj");
    this->model_shader->get_shader().bind();

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &camera.get_view()[0][0]);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &camera.get_proj()[0][0]);

    for (auto& entity : this->renderable_sys->entities) {
        auto& transform = *ecs::Coordinator::get_component<ecs::Transform>(entity);
        auto& renderable = *ecs::Coordinator::get_component<Renderable>(entity);

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &transform.get_global()[0][0]);

        switch (renderable.type) {
        case Renderable::Type::Model:
            renderable.model->get_palette().bind(0);
            renderable.model->get_vertex_array().bind();
            renderable.model->get_index_buffer().bind();
            glDrawElements(GL_TRIANGLES, renderable.model->get_index_count(), GL_UNSIGNED_INT, nullptr);
            break;
        }
    }

    // SSAO pass
    glBindVertexArray(this->screen_va);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, this->ssao.fbo);
    glDrawBuffers(1, &draw_buffers[0]);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.position);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->ssao.noise);

    auto noise_scale = glm::vec2(float(this->size.x) / 4.0f, float(this->size.y) / 4.0f);

    auto& ssao_shader = this->ssao.shader->get_shader();
    ssao_shader.bind();
    glUniform1i(ssao_shader.get_uniform_location("position_tex"), 0);
    glUniform1i(ssao_shader.get_uniform_location("normal_tex"), 1);
    glUniform1i(ssao_shader.get_uniform_location("noise_tex"), 2);
    glUniform3fv(ssao_shader.get_uniform_location("samples"), this->ssao.samples.size(), &this->ssao.samples[0][0]);
    glUniform2fv(ssao_shader.get_uniform_location("noise_scale"), 1, &noise_scale[0]);
    glUniformMatrix4fv(ssao_shader.get_uniform_location("projection"), 1, GL_FALSE, &camera.get_proj()[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // SSAO Blur pass
    glBindFramebuffer(GL_FRAMEBUFFER, this->ssao_blur.fbo);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ssao.color_buffer);

    auto& ssao_blur_shader = this->ssao_blur.shader->get_shader();
    ssao_blur_shader.bind();
    glUniform1i(ssao_blur_shader.get_uniform_location("ssao_tex"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // GBuffer pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.albedo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.position);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.normal);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->ssao_blur.color_buffer);

    auto& lighting_shader = this->gbuffer.shader->get_shader();
    lighting_shader.bind();
    glUniform1i(lighting_shader.get_uniform_location("albedo_tex"), 0);
    glUniform1i(lighting_shader.get_uniform_location("position_tex"), 1);
    glUniform1i(lighting_shader.get_uniform_location("normal_tex"), 2);
    glUniform1i(lighting_shader.get_uniform_location("ssao_tex"), 3);
    glUniformMatrix4fv(lighting_shader.get_uniform_location("proj"), 1, GL_FALSE, &camera.get_proj()[0][0]);
    glUniformMatrix4fv(lighting_shader.get_uniform_location("view"), 1, GL_FALSE, &camera.get_view()[0][0]);
    glUniform1f(lighting_shader.get_uniform_location("z_far"), camera.get_z_far());
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->lights_ubo, 0, sizeof(Light) * LIGHT_COUNT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Debug draw on top of screen
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gbuffer.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, this->size.x, this->size.y, 0, 0, this->size.x, this->size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    if (this->debug_rendering) {
        auto vp = camera.get_proj() * camera.get_view();
        Debug::flush(vp, dt);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void vpg::gl::Renderer::resize_callback(glm::ivec2 size) {
    this->destroy_gbuffer();
    this->destroy_ssao();
    this->size = size;
    this->create_gbuffer();
    this->create_ssao();
}

void vpg::gl::Renderer::debug_render_toggle_callback(input::Keyboard::Key key) {
    using Key = input::Keyboard::Key;
    switch (key) {
    case Key::F1:
        this->wireframe = !this->wireframe;
        break;
    case Key::F2:
        this->debug_rendering = !this->debug_rendering;
        if (this->debug_rendering) {
            Debug::init();
        }
        else {
            Debug::terminate();
        }
        break;
    case Key::F3:
        this->debug_lights = !this->debug_lights;
        break;
    }
}

void vpg::gl::Renderer::create_gbuffer() {
    // Prepare GBuffer framebuffer and textures
    glGenFramebuffers(1, &this->gbuffer.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gbuffer.fbo);

    glGenTextures(1, &this->gbuffer.albedo);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.albedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->size.x, this->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gbuffer.albedo, 0);

    glGenTextures(1, &this->gbuffer.position);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->size.x, this->size.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gbuffer.position, 0);

    glGenTextures(1, &this->gbuffer.normal);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->size.x, this->size.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->gbuffer.normal, 0);

    glGenTextures(1, &this->gbuffer.depth);
    glBindTexture(GL_TEXTURE_2D, this->gbuffer.depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->size.x, this->size.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->gbuffer.depth, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "vpg::gl::Renderer::create_gbuffer() failed:\n"
                  << "GBuffer framebuffer is not complete\n";
        abort();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void vpg::gl::Renderer::destroy_gbuffer() {
    glDeleteFramebuffers(1, &this->gbuffer.fbo);
    glDeleteTextures(1, &this->gbuffer.albedo);
    glDeleteTextures(1, &this->gbuffer.position);
    glDeleteTextures(1, &this->gbuffer.normal);
    glDeleteTextures(1, &this->gbuffer.depth);
}

void vpg::gl::Renderer::create_ssao() {
    // SSAO framebuffer and textures
    glGenFramebuffers(1, &this->ssao.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ssao.fbo);

    glGenTextures(1, &this->ssao.color_buffer);
    glBindTexture(GL_TEXTURE_2D, this->ssao.color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->size.x, this->size.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->ssao.color_buffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "vpg::gl::Renderer::create_ssao() failed:\n"
                  << "SSAO framebuffer is not complete\n";
        abort();
    }

    // SSAO kernel
    std::uniform_real_distribution<float> random_floats(0.0f, 1.0f);
    std::default_random_engine generator;
    this->ssao.samples.resize(64);
    for (int i = 0; i < this->ssao.samples.size(); ++i) {
        auto sample = glm::vec3(
            random_floats(generator) * 2.0 - 1.0,
            random_floats(generator) * 2.0 - 1.0,
            random_floats(generator)
        );
        sample = glm::normalize(sample) * random_floats(generator);

        float scale = (float)i / (float)this->ssao.samples.size();
        scale = 0.1f + scale * scale * (1.0f - 0.1f);
        sample *= scale;

        this->ssao.samples[i] = sample;
    }

    // SSAO noise texture
    std::vector<glm::vec3> ssao_noise;
    for (int i = 0; i < 16; ++i) {
        ssao_noise.push_back({
            random_floats(generator) * 2.0 - 1.0,
            random_floats(generator) * 2.0 - 1.0,
            0.0f
            });
    }

    glGenTextures(1, &this->ssao.noise);
    glBindTexture(GL_TEXTURE_2D, this->ssao.noise);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_noise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // SSAO blur framebuffer and texture
    glGenFramebuffers(1, &this->ssao_blur.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ssao_blur.fbo);

    glGenTextures(1, &this->ssao_blur.color_buffer);
    glBindTexture(GL_TEXTURE_2D, this->ssao_blur.color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->size.x, this->size.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->ssao_blur.color_buffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "vpg::gl::Renderer::create_ssao() failed:\n"
                  << "SSAO blur framebuffer is not complete\n";
        abort();
    }
}

void vpg::gl::Renderer::destroy_ssao() {
    glDeleteFramebuffers(1, &this->ssao.fbo);
    glDeleteTextures(1, &this->ssao.color_buffer);
    glDeleteTextures(1, &this->ssao.noise);
    glDeleteFramebuffers(1, &this->ssao_blur.fbo);
    glDeleteTextures(1, &this->ssao_blur.color_buffer);
}
