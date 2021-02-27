#include <vpg/data/shader.hpp>
#include <vpg/config.hpp>

#include <fstream>

using namespace vpg;
using namespace vpg::data;

void* Shader::load(Asset* asset) {
    auto args = asset->get_args();
    std::string vs_path = Config::get_string("data.folder", "./data/") + args.substr(0, args.find(' '));
    std::string fs_path = Config::get_string("data.folder", "./data/") + args.substr(args.find(' ') + 1);
    
    // Load vertex shader
    std::string vs;
    std::ifstream vs_ifs(vs_path);
    if (!vs_ifs.is_open()) {
        std::cerr << "vpg::data::Shader::load() failed:\n"
                  << "Couldn't open file '" << vs_path << "'\n";
        return nullptr;
    }
    vs_ifs.seekg(0, std::ios::end);
    vs.reserve(vs_ifs.tellg());
    vs_ifs.seekg(0, std::ios::beg);
    vs.assign((std::istreambuf_iterator<char>(vs_ifs)),
        std::istreambuf_iterator<char>());
    vs_ifs.close();

    // Load fragment shader
    std::string fs;
    std::ifstream fs_ifs(fs_path);
    if (!fs_ifs.is_open()) {
        std::cerr << "vpg::data::Shader::load() failed:\n"
                  << "Couldn't open file '" << fs_path << "'\n";
        return nullptr;
    }
    fs_ifs.seekg(0, std::ios::end);
    fs.reserve(fs_ifs.tellg());
    fs_ifs.seekg(0, std::ios::beg);
    fs.assign((std::istreambuf_iterator<char>(fs_ifs)),
        std::istreambuf_iterator<char>());
    fs_ifs.close();

    auto shader = new Shader();

    if (!gl::Shader::create(shader->shader, vs.c_str(), fs.c_str())) {
        std::cerr << "vpg::data::Shader::load() failed:\n"
                  << "Couldn't create shader\n";
        delete shader;
        return nullptr;
    }

    return shader;
}

void Shader::unload(Asset* asset) {
    auto shader = (Shader*)asset->get_data();
    delete shader;
}
