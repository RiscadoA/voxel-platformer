#include "camera_controller.hpp"

#include <vpg/config.hpp>

using namespace vpg;
using namespace vpg::ecs;

bool CameraController::Info::serialize(memory::Stream& stream) const {
    return true;
}

bool CameraController::Info::deserialize(memory::Stream& stream) {
    return true;
}

/*void CameraBehaviour::glfw_cursor_pos_callback(GLFWwindow* win, double x, double y) {
    auto behaviour = CameraBehaviour::current_camera;
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(behaviour->entity);

    static double px = INFINITY, py;
    if (px != INFINITY) {
        glm::quat rot = glm::angleAxis((float)(py - y) * behaviour->sensitivity, glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis((float)(px - x) * behaviour->sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
        transform->rotate(rot);
    }

    px = x;
    py = y;
}*/

CameraController::CameraController(Entity entity, const Info& info) {
    this->entity = entity;
    this->sensitivity = (float)Config::get_float("camera.sensitivity", 10.0);
    this->speed = (float)Config::get_float("camera.speed", 10.0);

    /*this->old_callback = glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    CameraBehaviour::current_camera = this;*/
}

CameraController::~CameraController() {
    /*glfwSetCursorPosCallback(window, this->old_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    CameraBehaviour::current_camera = nullptr;*/
}

void CameraController::update(float dt) {
    /*auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        transform->translate(transform->get_forward() * dt * this->speed);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        transform->translate(-transform->get_forward() * dt * this->speed);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        transform->translate(transform->get_right() * dt * this->speed);
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        transform->translate(-transform->get_right() * dt * this->speed);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        transform->translate(transform->get_up() * dt * this->speed);
    }
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        transform->translate(-transform->get_up() * dt * this->speed);
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        transform->look_at(glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }*/
}
