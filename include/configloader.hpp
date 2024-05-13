#pragma once

#include "toml++/toml.h"
#include "unordered_map"

class Configurable {
 public:
  virtual bool LoadToml(const toml::parse_result &toml) = 0;
  virtual void SaveToml() = 0;
  virtual ~Configurable() = default;
};

class ResourceBased {
 public:
  virtual bool LoadResource(u64 id) = 0;
};

//In charge of loading and serializing config files.
class ConfigLoader {
  inline static ConfigLoader *instance = nullptr;
  ConfigLoader();

  std::unordered_map<u64, toml::parse_result> configs;

 public:
  ConfigLoader(const ConfigLoader &) = delete;
  ConfigLoader(ConfigLoader &&) = delete;
  ~ConfigLoader() = default;
  toml::parse_result LoadSettings();
  static ConfigLoader *CreateConfigLoader();
  static ConfigLoader *GetHandle();
  toml::parse_result LoadConfigFile(std::string filePath);
  toml::parse_result LoadConfigFile(u64 id);
  toml::parse_result LoadPlayerSettings();

  template<typename T>
  requires std::is_base_of_v<Configurable, T>
  static std::shared_ptr<T> CreateConfigurablePtr(const toml::parse_result &toml) {
	std::shared_ptr<T> obj = std::make_shared<T>();
	const auto config = dynamic_cast<Configurable *>(obj.get());
	config->LoadToml(toml);
	return obj;
  }
  template<typename T>
  requires std::is_base_of_v<Configurable, T>
  static T CreateConfigurable(const toml::parse_result &toml) {
	T obj;
	Configurable *config = &obj;
	config->LoadToml(toml);
	return obj;
  }
  template<typename T>
  requires std::is_base_of_v<Configurable, T>
  static std::shared_ptr<T> CreateConfigurablePtr(u64 id) {
	std::shared_ptr<T> obj = std::make_shared<T>();
	const auto config = dynamic_cast<Configurable *>(obj.get());
	const auto toml = ConfigLoader::GetHandle()->configs[id];
	config->LoadToml(toml);
	return obj;
  }
  template<typename T>
  requires std::is_base_of_v<Configurable, T> && (!std::is_base_of_v<ResourceBased, T>)
  static T CreateConfigurable(u64 id) {
	T obj;
	Configurable *config = &obj;
	const auto toml = ConfigLoader::GetHandle()->configs[id];
	config->LoadToml(toml);
	return obj;
  }
  template<typename T>
  requires std::is_base_of_v<Configurable, T> && std::is_base_of_v<ResourceBased, T>
  static std::shared_ptr<T> CreateConfigurablePtr(u64 id) {
	std::shared_ptr<T> obj = std::make_shared<T>;
	auto *config = dynamic_cast<Configurable *>(obj.get());
	const auto toml = ConfigLoader::GetHandle()->configs[id];
	config->LoadToml(toml);
	auto *res = dynamic_cast<ResourceBased *>(obj.get());
	res->LoadResource(id);
	return obj;
  }

  template<typename T>
  requires std::is_base_of_v<Configurable, T> && std::is_base_of_v<ResourceBased, T>
  static T CreateConfigurable(u64 id) {
	T obj;
	Configurable *config = &obj;
	const auto toml = ConfigLoader::GetHandle()->configs[id];
	config->LoadToml(toml);
	ResourceBased *res = &obj;
	res->LoadResource(id);
	return obj;
  }

};
