#include <vpg/data/model.hpp>
#include "model.hpp"

void* vpg::data::Model::load(Asset* asset) {
    auto model = new Model();
    return model;
}

void vpg::data::Model::unload(Asset* asset) {
    auto model = (Model*)asset->get_data();
    delete model;
}
