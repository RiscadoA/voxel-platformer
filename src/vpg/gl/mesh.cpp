#include <vpg/gl/mesh.hpp>

#include <iostream>
#include <stack>
#include <chrono>

#include <GL/glew.h>

using namespace vpg;
using namespace vpg::gl;

Mesh::Mesh(Mesh&& rhs) {
    this->va = std::move(rhs.va);
    this->vb = std::move(rhs.vb);
    this->ib = std::move(rhs.ib);
    this->count = rhs.count;
    rhs.count = 0;
}

void Mesh::draw() const {
    if (this->count > 0 && this->va_ready) {
        this->va.bind();
        this->ib.bind();
        glDrawElements(GL_TRIANGLES, this->count, GL_UNSIGNED_INT, nullptr);
    }
}

void Mesh::generate_va() {
    if (!this->va_ready) {
        if (this->count > 0) {
            VertexArray::create(this->va, {
                Attribute(
                    this->vb,
                    sizeof(Vertex), offsetof(Vertex, Vertex::pos),
                    3, Attribute::Type::U8,
                    0
                ),
                Attribute(
                    this->vb,
                    sizeof(Vertex), offsetof(Vertex, Vertex::normal),
                    3, Attribute::Type::I8,
                    1
                ),
                Attribute(
                    this->vb,
                    sizeof(Vertex), offsetof(Vertex, Vertex::material),
                    1, Attribute::Type::U8,
                    2
                )
            });
        }

        this->va_ready = true;
    }
}

bool Mesh::update(const Matrix& matrix, bool generate_borders, bool gen_va) {
    auto begin = std::chrono::steady_clock::now();

    std::vector<Vertex> verts;
    std::vector<unsigned int> indices;
    std::vector<unsigned char> mask;

    auto& sz = matrix.sz;

    // For both back and front faces
    bool back_face = true;
    do {
        back_face = !back_face;

        // For each axis
        for (int d = 0; d < 3; ++d) {
            int u = (d + 1) % 3;
            int v = (d + 2) % 3;

            glm::ivec3 x = { 0, 0, 0 }, q = { 0, 0, 0 };
            q[d] = 1;
            mask.resize(sz[u] * sz[v]);

            for (x[d] = -1; x[d] < int(sz[d]);) {
                int n = 0;

                // Create mask
                if (generate_borders) {
                    for (x[v] = 0; x[v] < int(sz[v]); ++x[v]) {
                        for (x[u] = 0; x[u] < int(sz[u]); ++x[u]) {
                            if (x[d] < 0) {
                                mask[n++] = back_face ?
                                    matrix.voxels[(x.x + q.x) * sz.y * sz.z + (x.y + q.y) * sz.z + (x.z + q.z)] :
                                    0;
                            }
                            else if (x[d] == int(sz[d]) - 1) {
                                mask[n++] = back_face ?
                                    0 :
                                    matrix.voxels[x.x * sz.y * sz.z + x.y * sz.z + x.z];
                            }
                            else if (matrix.voxels[x.x * sz.y * sz.z + x.y * sz.z + x.z] == 0 ||
                                     matrix.voxels[(x.x + q.x) * sz.y * sz.z + (x.y + q.y) * sz.z + (x.z + q.z)] == 0) {
                                mask[n++] = back_face ?
                                    matrix.voxels[(x.x + q.x) * sz.y * sz.z + (x.y + q.y) * sz.z + (x.z + q.z)] :
                                    matrix.voxels[x.x * sz.y * sz.z + x.y * sz.z + x.z];
                            }
                            else {
                                mask[n++] = 0;
                            }
                        }
                    }
                }
                else {
                    for (x[v] = 0; x[v] < int(sz[v]); ++x[v]) {
                        for (x[u] = 0; x[u] < int(sz[u]); ++x[u]) {
                            if (x[d] >= 0 && x[d] < int(sz[d]) - 1 &&
                                matrix.voxels[x.x * sz.y * sz.z + x.y * sz.z + x.z] == 0 ||
                                matrix.voxels[(x.x + q.x) * sz.y * sz.z + (x.y + q.y) * sz.z + (x.z + q.z)] == 0) {
                                mask[n++] = back_face ?
                                    matrix.voxels[(x.x + q.x) * sz.y * sz.z + (x.y + q.y) * sz.z + (x.z + q.z)] :
                                    matrix.voxels[x.x * sz.y * sz.z + x.y * sz.z + x.z];
                            }
                            else {
                                mask[n++] = 0;
                            }
                        }
                    }
                }

                ++x[d];
                n = 0;

                // Generate mesh from mask
                for (int j = 0; j < int(sz[v]); ++j) {
                    for (int i = 0; i < int(sz[u]);) {
                        if (mask[n] != 0) {
                            int w, h;
                            for (w = 1; i + w < int(sz[u]) && mask[n + w] == mask[n]; ++w);
                            bool done = false;
                            for (h = 1; j + h < int(sz[v]); ++h) {
                                for (int k = 0; k < w; ++k) {
                                    if (mask[n + k + h * sz[u]] == 0 || mask[n + k + h * sz[u]] != mask[n]) {
                                        done = true;
                                        break;
                                    }
                                }

                                if (done) {
                                    break;
                                }
                            }

                            if (mask[n] != 0) {
                                x[u] = i;
                                x[v] = j;

                                glm::ivec3 du = { 0, 0, 0 }, dv = { 0, 0, 0 };
                                du[u] = w;
                                dv[v] = h;

                                auto vi = verts.size();
                                verts.resize(vi + 4, { { 0, 0, 0 }, back_face ? -q : q, mask[n] });
                                verts[vi + 0].pos = glm::u8vec3(x);
                                verts[vi + 1].pos = glm::u8vec3(x + du);
                                verts[vi + 2].pos = glm::u8vec3(x + du + dv);
                                verts[vi + 3].pos = glm::u8vec3(x + dv);

                                auto ii = indices.size();
                                indices.resize(ii + 6);
                                if (back_face) {
                                    indices[ii + 0] = int(vi) + 0;
                                    indices[ii + 1] = int(vi) + 2;
                                    indices[ii + 2] = int(vi) + 1;
                                    indices[ii + 3] = int(vi) + 3;
                                    indices[ii + 4] = int(vi) + 2;
                                    indices[ii + 5] = int(vi) + 0;
                                }
                                else {
                                    indices[ii + 0] = int(vi) + 0;
                                    indices[ii + 1] = int(vi) + 1;
                                    indices[ii + 2] = int(vi) + 2;
                                    indices[ii + 3] = int(vi) + 2;
                                    indices[ii + 4] = int(vi) + 3;
                                    indices[ii + 5] = int(vi) + 0;
                                }
                            }

                            for (int l = 0; l < h; ++l) {
                                for (int k = 0; k < w; ++k) {
                                    mask[n + k + l * sz[u]] = 0;
                                }
                            }

                            i += w;
                            n += w;
                        }
                        else {
                            ++i;
                            ++n;
                        }
                    }
                }
            }
        }
    } while (back_face != true);

    auto end = std::chrono::steady_clock::now();
    auto t = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    //std::cout << "Matrix meshing time: " << t << "us" << std::endl;
    //std::cout << opaque_verts.size() << " vertices, " << (opaque_indices.size() + transparent_indices.size()) << " indices" << std::endl;

    this->count = indices.size();
    if (this->count > 0) {
        if (!IndexBuffer::create(this->ib, indices.size() * sizeof(unsigned int), indices.data(), Usage::Static)) {
            std::cerr << "vpg::gl::Mesh::update() failed:\n"
                      << "Couldn't create index buffer\n";
            return false;
        }

        if (!VertexBuffer::create(this->vb, verts.size() * sizeof(Vertex), verts.data(), Usage::Static)) {
            std::cerr << "vpg::gl::Mesh::update() failed:\n"
                      << "Couldn't create vertex buffer\n";
            return false;
        }

        this->va_ready = false;
        if (gen_va) {
            this->generate_va();
        }
    }

    return true;
}
