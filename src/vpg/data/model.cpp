#include <vpg/data/model.hpp>
#include <vpg/data/qb_parser.hpp>
#include <vpg/config.hpp>

#include <iostream>

void* vpg::data::Model::load(Asset* asset) {
    std::string path = Config::get_string("data.folder", "./data/") + asset->get_args();
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

    if (!parse_qb(model->matrix, model->palette, ifs)) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't parse Qubicle file\n";
        delete model;
        return nullptr;
    }

    if (!model->mesh.update(model->matrix)) {
        std::cerr << "vpg::data::Model::load() failed:\n"
                  << "Couldn't create mesh\n";
        delete model;
        return nullptr;
    }

    model->palette.update();

    return model;
}

void vpg::data::Model::unload(Asset* asset) {
    auto model = (Model*)asset->get_data();
    delete model;
}
