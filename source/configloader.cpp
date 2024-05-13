#include "PCH.hpp"

#include <filesystem>
#include "configloader.hpp"
#include "windows.h"

toml::parse_result ConfigLoader::LoadSettings() {
  CHAR path[MAX_PATH];
  GetModuleFileName(nullptr, path, MAX_PATH);
  std::string execPath = path;
  execPath.erase(execPath.size() - execNameSize, execPath.size());
  std::string fileLoc = "resources/config/settings.toml";
  const auto file = std::format("{}{}", execPath, fileLoc);
  return toml::parse_file("C:/Projects/OpenGLRenderer/resources/config/settings.toml");
}

ConfigLoader *ConfigLoader::CreateConfigLoader() {
  if (!instance) {
	instance = new ConfigLoader;
  }

  return instance;
}

ConfigLoader *ConfigLoader::GetHandle() {
  return instance;
}
toml::parse_result ConfigLoader::LoadConfigFile(std::string filePath) {
  return toml::parse_file(filePath);
}

ConfigLoader::ConfigLoader() {

  try {
	std::filesystem::path path = ("resources/config/models");
	for (const auto &file : std::filesystem::directory_iterator(path)) {
	  auto toml = toml::parse_file(file.path().string());
	  u64 id = std::hash<std::string>{}(toml["Entity"]["nickname"].value_or(""));
	  configs.insert({id, toml});
	}
  }
  catch (std::exception &e) {
	std::cout << e.what() << std::endl;
  }
}

toml::parse_result ConfigLoader::LoadConfigFile(u64 id) {
  return configs[id];
}
toml::parse_result ConfigLoader::LoadPlayerSettings() {
  return toml::parse_file("resources/config/keybinds.toml");
}
