#include <vpg/data/manager.hpp>
#include <vpg/config.hpp>

#include <fstream>

using namespace vpg;
using namespace vpg::data;

std::map<std::string, Manager::Loader> Manager::loaders = {};
std::map<std::string, Asset*> Manager::assets = {};

bool Manager::init() {
	auto data_folder = Config::get_string("data.folder", "./data/");
	auto assets_cfg = data_folder + "assets.cfg";

	// Parse variables from config file
	std::ifstream fs(assets_cfg);
	if (!fs.is_open()) {
		std::cerr << "vpg::data::Manager::init() failed:" << std::endl;
		std::cerr << "Couldn't open configuration file on path \"" << assets_cfg << "\"" << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(fs, line)) {
		// Remove comments
		int i = 0;
		for (; i < line.size() && line[i] != ';'; ++i);
		line = line.substr(0, i);

		// Remove whitespace
		for (i = 0; i < line.size() && (line[i] == ' ' || line[i] == '\t'); ++i);
		line = line.substr(i);
		for (i = int(line.size()) - 1; i >= 0 && (line[i] == ' ' || line[i] == '\t'); --i);
		line = line.substr(0, size_t(i) + 1);
		if (line.empty())
			continue;

		// Search for assignment operator
		bool found = false;
		for (i = 0; i < line.size(); ++i) {
			if (line[i] == '=') {
				found = true;
				break;
			}
		}

		if (found) {
			auto key = line.substr(0, i);
			auto value = line.substr(size_t(i) + 1);

			// Remove whitespace
			for (i = 0; i < key.size() && (key[i] == ' ' || key[i] == '\t'); ++i);
			key = key.substr(i);
			for (i = int(key.size()) - 1; i >= 0 && (key[i] == ' ' || key[i] == '\t'); --i);
			key = key.substr(0, size_t(i) + 1);
			for (i = 0; i < value.size() && (value[i] == ' ' || value[i] == '\t'); ++i);
			value = value.substr(i);
			for (i = int(value.size()) - 1; i >= 0 && (value[i] == ' ' || value[i] == '\t'); --i);
			value = value.substr(0, size_t(i) + 1);

			if (key.empty()) {
				std::cerr << "vpg::data::Manager::init() failed:" << std::endl;
				std::cerr << "Couldn't parse configuration file:" << std::endl;
				std::cerr << "Invalid 'key=value' pair on \"" << line << "\"" << std::endl;
				std::cerr << "The key string must not be empty" << std::endl;
				continue; // Skip line
			}

			// Split value into usage mode, type and arguments
			std::string usage = value.substr(0, value.find(' '));
			value = value.substr(value.find(' ') + 1);
			std::string type = value.substr(0, value.find(' '));
			std::string args = value.substr(value.find(' ') + 1);

			bool dynamic;
			if (usage == "static") {
				dynamic = false;
			}
			else if (usage == "dynamic") {
				dynamic = true;
			}
			else {
				std::cerr << "vpg::data::Manager::init() failed:" << std::endl;
				std::cerr << "Couldn't parse configuration file:" << std::endl;
				std::cerr << "Invalid usage mode \"" << usage << "\" on \"" << line << "\"" << std::endl;
				std::cerr << "The usage mode must be either \"static\" or \"dynamic\"" << std::endl;
				continue; // Skip line
			}

			auto it = Manager::loaders.find(type);
			if (it == Manager::loaders.end()) {
				std::cerr << "vpg::data::Manager::init() failed:" << std::endl;
				std::cerr << "Couldn't parse configuration file:" << std::endl;
				std::cerr << "Unknown asset type \"" << type << "\" on \"" << line << "\"" << std::endl;
				std::cerr << "No loader found for this type" << std::endl;
				continue; // Skip line
			}

			auto loader = it->second;

			if (Manager::assets.find(key) != Manager::assets.end()) {
				std::cerr << "vpg::data::Manager::init() failed:" << std::endl;
				std::cerr << "Couldn't parse configuration file:" << std::endl;
				std::cerr << "More than one asset has the ID \"" << key << "\"" << std::endl;
				std::cerr << "Ignoring all but the first asset" << std::endl;
				continue; // Skip line
			}

			Manager::assets.emplace(key, new Asset(key, type, args, dynamic, loader.load_fn, loader.unload_fn));
		}
		else {
			std::cerr << "vpg::data::Manager::init() failed:" << std::endl;
			std::cerr << "Couldn't parse configuration file:" << std::endl;
			std::cerr << "Invalid 'key=value' pair on \"" << line << "\"" << std::endl;
			continue;
		}
	}

	return true;
}

void Manager::terminate() {
	for (auto& asset : Manager::assets) {
		delete asset.second;
	}
}

void Manager::register_loader(
    std::string type,
    std::function<void*(Asset*)> load_fn,
    std::function<void(Asset*)> unload_fn) {
	auto it = Manager::loaders.find(type);
	if (it != Manager::loaders.end()) {
		std::cerr << "vpg::data::Manager::register_loader() failed:\n"
				  << "Loader name conflict detected ('" << type << "')\n";
		abort();
	}

    Manager::loaders.emplace(type, Loader { load_fn, unload_fn });
}

template <>
Handle<void> Manager::load<void>(const std::string& id) {
	auto it = Manager::assets.find(id);
	if (it == Manager::assets.end()) {
		std::cerr << "vpg::data::Manager::load() failed:\n";
		std::cerr << "No asset found with the identifier '" << id << "'\n";
		return Handle<void>(nullptr);
	}

    return Handle<void>(it->second);
}