#include <vpg/gl/renderer.hpp>
#include <vpg/ecs/transform.hpp>

#include <gl/glew.h>

using namespace vpg;
using namespace vpg::gl;

vpg::gl::Renderer::Renderer(CameraSystem* camera_sys, RenderableSystem* renderable_sys) {
    this->camera_sys = camera_sys;
    this->renderable_sys = renderable_sys;

    this->model_shader = data::Manager::load<data::Shader>("shader.model");
    this->model_shader->get_shader().bind_uniform_buffer("Palette", 0);
}

vpg::gl::Renderer::~Renderer() {

}

void vpg::gl::Renderer::render() {
    // Get camera
    if (this->camera_sys->entities.empty()) {
        return;
    }
    auto& camera = *ecs::Coordinator::get_component<Camera>(*this->camera_sys->entities.begin());
    camera.update();

    // Opaque pass
    glClearColor(0.0f, 0.6f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

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
}
