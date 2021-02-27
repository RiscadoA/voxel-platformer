#include <vpg/gl/debug.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include <iostream>

using namespace vpg::gl;

bool Debug::initialized = false;
std::vector<Debug::Command> Debug::commands = {};
std::mutex Debug::command_mutex;
Shader Debug::shader;
VertexBuffer Debug::vb;
VertexArray Debug::va;

unsigned int Debug::color_loc = 0;
unsigned int Debug::vp_loc = 0;
unsigned int Debug::model_loc = 0;

unsigned int Debug::box_first = 0;
unsigned int Debug::box_count = 0;
unsigned int Debug::sphere_first = 0;
unsigned int Debug::sphere_count = 0;

void Debug::init() {
    if (Debug::initialized) {
        return;
    }

    Debug::initialized = true;

    if (!Shader::create(Debug::shader, R"(
        #version 330 core

        layout (location = 0) in vec3 vert_pos;

        uniform mat4 vp;
        uniform mat4 model;

        void main() {
            gl_Position = vp * model * vec4(vert_pos, 1.0);
        }
    )", R"(
        #version 330 core

        uniform vec4 color;

        out vec4 frag_color;

        void main() {
            frag_color = color;
        }
    )")) {
        std::cerr << "Debug::init() failed:\nCouldn't create Debug::shader\n";
        abort();
    }

    Debug::color_loc = Debug::shader.get_uniform_location("color");
    Debug::vp_loc = Debug::shader.get_uniform_location("vp");
    Debug::model_loc = Debug::shader.get_uniform_location("model");

    std::vector<glm::vec3> data;

    // Add box
    Debug::box_first = data.size();
    for (int d = 0; d < 3; ++d) {
        glm::vec3 t, u, v;
        t = u = v = { 0.0f, 0.0f, 0.0f };
        t[d] = 1.0f;
        u[(d + 1) % 3] = 1.0f;
        v[(d + 2) % 3] = 1.0f;

        for (int s = -1; s <= 1; s += 2) {
            data.insert(data.end(), {
                t * (float)s - u - v,
                t * (float)s + u - v,
                t * (float)s + u + v,
                t * (float)s - u - v,
                t * (float)s - u + v,
                t * (float)s + u + v,
            });
        }
    }
    Debug::box_count = data.size() - Debug::box_first;

    // Add a sphere
    Debug::sphere_first = data.size();
    for (int d = 0; d < 3; ++d) {
        glm::vec3 t, u, v;
        t = u = v = { 0.0f, 0.0f, 0.0f };
        t[d] = 1.0f;
        u[(d + 1) % 3] = 1.0f;
        v[(d + 2) % 3] = 1.0f;

        const int side = 2;

        for (int x = -side; x <= side; ++x) {
            for (int y = -side; y <= side; ++y) {
                for (int s = -1; s <= 1; s += 2) {
                    data.insert(data.end(), {
                        (float)(side * 2 + 1) * t * (float)s - u - v + (float)x * u * 2.0f + (float)y * v * 2.0f,
                        (float)(side * 2 + 1) * t * (float)s + u - v + (float)x * u * 2.0f + (float)y * v * 2.0f,
                        (float)(side * 2 + 1) * t * (float)s + u + v + (float)x * u * 2.0f + (float)y * v * 2.0f,
                        (float)(side * 2 + 1) * t * (float)s - u - v + (float)x * u * 2.0f + (float)y * v * 2.0f,
                        (float)(side * 2 + 1) * t * (float)s - u + v + (float)x * u * 2.0f + (float)y * v * 2.0f,
                        (float)(side * 2 + 1) * t * (float)s + u + v + (float)x * u * 2.0f + (float)y * v * 2.0f,
                    });
                }
            }
        }
    }
    Debug::sphere_count = data.size() - Debug::sphere_first;
    for (int i = Debug::sphere_first; i < Debug::sphere_first + Debug::sphere_count; ++i) {
        data[i] = glm::normalize(data[i]);
    }

    if (!VertexBuffer::create(Debug::vb, sizeof(glm::vec3) * data.size(), data.data(), Usage::Static)) {
        std::cerr << "Debug::init() failed:\nCouldn't create Debug::vb\n";
        abort();
    }
    
    if (!VertexArray::create(Debug::va, {
         Attribute(
            Debug::vb,
            sizeof(float) * 3, 0,
            3, Attribute::Type::F32,
            0
        ),
    })) {
        std::cerr << "Debug::init() failed:\nCouldn't create Debug::va\n";
        abort();
    }
}

void Debug::terminate() {
    if (!Debug::initialized) {
        return;
    }
    
    Debug::initialized = false;

    Debug::shader = Shader();
    Debug::vb = VertexBuffer();
    Debug::va = VertexArray();
}

void Debug::flush(const glm::mat4& vp, float dt) {
    if (!Debug::initialized) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Debug::shader.bind();
    Debug::va.bind();

    glUniformMatrix4fv(Debug::vp_loc, 1, GL_FALSE, &vp[0][0]);

    std::vector<Debug::Command> preserve_commands;
    Debug::command_mutex.lock();
    
    for (auto& command : Debug::commands) {
        glUniform4fv(Debug::color_loc, 1, &command.color[0]);
        glUniformMatrix4fv(Debug::model_loc, 1, GL_FALSE, &command.transform[0][0]);
        glDrawArrays(GL_TRIANGLES, command.first, command.count);

        command.life -= dt;
        if (command.life > 0.0f) {
            preserve_commands.emplace_back(command);
        }
    }

    Debug::commands = std::move(preserve_commands);
    Debug::command_mutex.unlock();
}

void Debug::draw_box(glm::vec3 position, glm::vec3 scale, glm::vec4 color, float time) {
    if (!Debug::initialized) {
        return;
    }

    auto model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, scale);
    Debug::add_command(model, color, Debug::box_first, Debug::box_count, time);
}

void Debug::draw_sphere(glm::vec3 position, float radius, glm::vec4 color, float time) {
    if (!Debug::initialized) {
        return;
    }

    auto model = glm::scale(glm::mat4(1.0f), glm::vec3(radius));
    model = glm::translate(model, position);
    Debug::add_command(model, color, Debug::sphere_first, Debug::sphere_count, time);
}

void Debug::add_command(const glm::mat4& transform, glm::vec4 color, unsigned int first, unsigned int count, float time) {
    Debug::command_mutex.lock();
    Debug::commands.emplace_back(Debug::Command {
        transform,
        color,
        first, count,
        time
    }); 
    Debug::command_mutex.unlock();
}
