#pragma once

#include <map>
#include <string>
#include <mutex>

namespace vpg {
	class Config final {
	public:
		Config() = delete;
		~Config() = delete;

		static bool load(int argc, char** argv);

		static bool get_boolean(const std::string& key, bool default);
		static int64_t get_integer(const std::string& key, int64_t default);
		static double get_float(const std::string& key, double default);
		static std::string get_string(const std::string& key, const std::string& default);

		static void set_boolean(const std::string& key, bool value);
		static void set_integer(const std::string& key, int64_t value);
		static void set_float(const std::string& key, double value);
		static void set_string(const std::string& key, const std::string& value);

	private:
		static std::mutex mutex;
		static std::map<std::string, std::string> variables;
	};
}