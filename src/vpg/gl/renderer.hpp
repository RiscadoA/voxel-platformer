#pragma once

#include <vpg/gl/camera.hpp>
#include <vpg/gl/renderable.hpp>
#include <vpg/gl/light.hpp>

#include <vpg/data/shader.hpp>

#include <vpg/input/window.hpp>

namespace vpg::gl {
    class Renderer {
    public:
        Renderer(CameraSystem* camera_sys, LightSystem* light_sys, RenderableSystem* renderable_sys);
        ~Renderer();

        void render(float dt);
    
    private:
        void create_gbuffer();
        void destroy_gbuffer();
        void create_ssao();
        void destroy_ssao();

        void resize_callback(glm::ivec2 size);
        Listener resize_listener;

        CameraSystem* camera_sys;
        LightSystem* light_sys;
        RenderableSystem* renderable_sys;

        data::Handle<data::Shader> model_shader;
        
        glm::vec3 sky_color;
        bool wireframe;
        bool debug_rendering;
        
        glm::ivec2 size;
        unsigned int screen_va;

        struct LightData {
            glm::vec4 position = { 0.0f, 0.0f, 0.0f, 1.0f };
            glm::vec4 direction = { 0.0f, 0.0f, 0.0f, 0.0f };
            glm::vec4 ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
            glm::vec4 diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
            float constant = 0.0f;
            float linear = 0.0f;
            float quadratic = 0.0f;
            float _padding = 0.0f;
        };
        unsigned int lights_ubo;

        struct {
            unsigned int fbo;
            unsigned int albedo, position, normal, depth;
            data::Handle<data::Shader> shader;
        } gbuffer;

        struct {
            unsigned int fbo;
            unsigned int color_buffer;
            unsigned int noise;
            std::vector<glm::vec3> samples;
            data::Handle<data::Shader> shader;
        } ssao;

        struct {
            unsigned int fbo;
            unsigned int color_buffer;
            data::Handle<data::Shader> shader;
        } ssao_blur;
    };
}