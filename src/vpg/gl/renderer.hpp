#pragma once

#include <vpg/gl/camera.hpp>
#include <vpg/gl/renderable.hpp>

#include <vpg/data/shader.hpp>

namespace vpg::gl {
    class Renderer {
    public:
        Renderer(CameraSystem* camera_sys, RenderableSystem* renderable_sys);
        ~Renderer();

        void render();
    
    private:
        CameraSystem* camera_sys;
        RenderableSystem* renderable_sys;

        data::Handle<data::Shader> model_shader;
        data::Handle<data::Shader> lighting_shader;
    };
}