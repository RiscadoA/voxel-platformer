#include <vpg/config.hpp>

#include <fstream>
#include <iostream>
#include <cstring>

using namespace vpg;

std::mutex Config::mutex;
std::map<std::string, std::string> Config::variables;

bool Config::load(int argc, char** argv) {
	std::string config_path = "./vpg.cfg"; // Defaut config file path

	// Parse variables from command-line arguments
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-c") == 0) {
			if (i + 1 >= argc) {
				std::cerr << "vpg::Config::load() failed:" << std::endl;
				std::cerr << "Command-line arguments parsing failed:" << std::endl;
				std::cerr << "Found '-c' but its argument is missing (unexpected end of string)" << std::endl;
				return false;
			}
			config_path = argv[++i];
		}
		else { // Parse key-value pair
			bool found = false;
			int j = 0;
			for (; argv[i][j] != '\0'; ++j) {
				if (argv[i][j] == '=') {
					found = true;
					++j;
					break;
				}
			}

			if (!found) {
				std::cerr << "vpg::Config::load() failed:" << std::endl;
				std::cerr << "Command-line arguments parsing failed:" << std::endl;
				std::cerr << "Expected 'key=value' pair, found \"" << argv[i] << "\"" << std::endl;
				return false;
			}

			auto key = std::string(argv[i][0], j - 1);
			auto value = std::string(&argv[i][j]);

			Config::variables.insert(std::make_pair(key, value));
		}
	}

	// Parse variables from config file
	std::ifstream fs(config_path);
	if (!fs.is_open()) {
		std::cerr << "vpg::Config::load() failed:" << std::endl;
		std::cerr << "Couldn't open configuration file on path \"" << config_path << "\"" << std::endl;
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
				std::cerr << "vpg::Config::load() failed:" << std::endl;
				std::cerr << "Couldn't parse configuration file:" << std::endl;
				std::cerr << "Invalid 'key=value' pair on \"" << line << "\"" << std::endl;
				std::cerr << "The key string must not be empty" << std::endl;
				return false;
			}

			if (Config::variables.find(key) == Config::variables.end())
				Config::variables.insert(std::make_pair(key, value));
		}
		else {
			std::cerr << "vpg::Config::load() failed:" << std::endl;
			std::cerr << "Couldn't parse configuration file:" << std::endl;
			std::cerr << "Invalid 'key=value' pair on \"" << line << "\"" << std::endl;
			return false;
		}
	}
}

bool vpg::Config::get_boolean(const std::string& key, bool def) {
	std::lock_guard guard(Config::mutex);

	if (Config::variables.find(key) == Config::variables.end()) {
		Config::variables[key] = std::to_string(def);
	}

	if (Config::variables[key] == "true") {
		return true;
	}
	else if (Config::variables[key] == "false") {
		return false;
	}
	else {
		Config::variables[key] = def ? "true" : "false";
		return def;
	}
}

int64_t Config::get_integer(const std::string& key, int64_t def) {
	std::lock_guard guard(Config::mutex);

	if (Config::variables.find(key) == Config::variables.end()) {
		Config::variables[key] = std::to_string(def);
	}

	try {
		return std::stoll(Config::variables[key]);
	}
	catch (...) {
		Config::variables[key] = std::to_string(def);
		return def;
	}
}

double Config::get_float(const std::string& key, double def) {
	std::lock_guard guard(Config::mutex);

	if (Config::variables.find(key) == Config::variables.end()) {
		Config::variables[key] = std::to_string(def);
	}

	try {
		return std::stod(Config::variables[key]);
	}
	catch (...) {
		Config::variables[key] = std::to_string(def);
		return def;
	}
}

std::string Config::get_string(const std::string& key, const std::string& def) {
	std::lock_guard guard(Config::mutex);

	if (Config::variables.find(key) == Config::variables.end()) {
		Config::variables[key] = def;
	}

	return Config::variables[key];
}

void vpg::Config::set_boolean(const std::string& key, bool value) {
	std::lock_guard guard(Config::mutex);
	Config::variables[key] = value ? "true" : "false";
}

void Config::set_integer(const std::string& key, int64_t value) {
	std::lock_guard guard(Config::mutex);
	Config::variables[key] = std::to_string(value);
}

void Config::set_float(const std::string& key, double value) {
	std::lock_guard guard(Config::mutex);
	Config::variables[key] = std::to_string(value);
}

void Config::set_string(const std::string& key, const std::string& value) {
	std::lock_guard guard(Config::mutex);
	Config::variables[key] = value;
}
