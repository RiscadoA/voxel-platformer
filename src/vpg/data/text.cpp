#include <vpg/data/text.hpp>
#include <vpg/config.hpp>

#include <fstream>

using namespace vpg;
using namespace vpg::data;

void* Text::load(Asset* asset) {
    auto args = asset->get_args();
    std::string path = Config::get_string("data.folder", "./data/") + args.substr(0, args.find(' '));
    std::string fs_path = Config::get_string("data.folder", "./data/") + args.substr(args.find(' ') + 1);

    // Load text
    std::string content;
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cerr << "vpg::data::Text::load() failed:\n"
                  << "Couldn't open file '" << path << "'\n";
        return nullptr;
    }
    ifs.seekg(0, std::ios::end);
    content.reserve(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    content.assign((std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>());
    ifs.close();

    auto text = new Text();
    text->content = content;
    return text;
}

void Text::unload(Asset* asset) {
    auto text = (Text*)asset->get_data();
    delete text;
}
