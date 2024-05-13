#pragma once

#include "fx/gltf.h"
#include "Rendering/renderDefs.h"

class ResourceLoader {
  ResourceLoader() = default;
  static ResourceLoader *instance;
  std::unordered_map<u64, std::shared_ptr<Resources::Model>> modelReferences;

 public:
  ResourceLoader(const ResourceLoader &) = delete;
  ResourceLoader(ResourceLoader &&) = delete;

  static ResourceLoader *CreateResourceLoader();
  bool GenerateResource(const toml::parse_result &toml);
  std::weak_ptr<Resources::Model> GrabModelRef(u64 id);

  ~ResourceLoader() = default;
  static ResourceLoader *GetHandle();
};
