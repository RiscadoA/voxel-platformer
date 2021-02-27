#pragma once

#include <vpg/gl/camera.hpp>
#include <vpg/gl/renderable.hpp>

#include <vpg/data/shader.hpp>

namespace vpg::gl {
    class Renderer {
    public:
        Renderer(glm::ivec2 size, CameraSystem* camera_sys, RenderableSystem* renderable_sys);
        ~Renderer();

        void render(float dt);
        void resize(glm::ivec2 size);
    
    private:
        void create_gbuffer();
        void destroy_gbuffer();
        void create_ssao();
        void destroy_ssao();

        CameraSystem* camera_sys;
        RenderableSystem* renderable_sys;

        data::Handle<data::Shader> model_shader;
        
        glm::vec3 sky_color;
        bool wireframe;
        bool debug_rendering;
        
        glm::ivec2 size;
        unsigned int screen_va;

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