#include <vpg/data/model.hpp>
#include <vpg/data/qb_parser.hpp>
#include <vpg/config.hpp>

#include <iostream>

void* vpg::data::Model::load(Asset* asset) {
    std::string path = Config::get_string("data.folder", "./data/") + asset->get_args().substr(0, asset->get_args().find(' '));
    bool emissive = asset->get_args().substr(asset->get_args().find(' ') + 1) == "emissive";

    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't open file '" << path << "'\n";
        return nullptr;
    }

    auto model = new Model();

    if (!gl::Palette::create(model->palette, {})) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't create palette\n";
        delete model;
        return nullptr;
    }

    if (!parse_qb(model->matrix, model->palette, ifs, emissive)) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't parse Qubicle file\n";
        delete model;
        return nullptr;
    }

    std::vector<gl::Vertex> vertices;
    std::vector<unsigned int> indices;
    if (!gl::matrix_to_mesh(vertices, indices, model->matrix)) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't create mesh from matrix\n";
        delete model;
        return nullptr;
    }

    if (!gl::VertexBuffer::create(model->vb, vertices.size() * sizeof(gl::Vertex), vertices.data(), gl::Usage::Static)) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't create vertex buffer\n";
        delete model;
        return nullptr;
    }

    if (!gl::IndexBuffer::create(model->ib, indices.size() * sizeof(unsigned int), indices.data(), gl::Usage::Static)) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't create index buffer\n";
        delete model;
        return nullptr;
    }

    if (!gl::VertexArray::create(model->va, {
        gl::Attribute(
            model->vb,
            sizeof(gl::Vertex), offsetof(gl::Vertex, gl::Vertex::pos),
            3, gl::Attribute::Type::F32,
            0
        ),
        gl::Attribute(
            model->vb,
            sizeof(gl::Vertex), offsetof(gl::Vertex, gl::Vertex::normal),
            3, gl::Attribute::Type::F32,
            1
        ),
        gl::Attribute(
            model->vb,
            sizeof(gl::Vertex), offsetof(gl::Vertex, gl::Vertex::material),
            1, gl::Attribute::Type::U8,
            2
        )
    })) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't create vertex array\n";
        delete model;
        return nullptr;
    }

    model->palette.update();
    model->index_count = indices.size();

    return model;
}

void vpg::data::Model::unload(Asset* asset) {
    auto model = (Model*)asset->get_data();
    delete model;
}
