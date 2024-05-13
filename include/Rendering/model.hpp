#pragma once
#include <vector>
#include "renderDefs.h"
#include "mesh.hpp"
#include "texture.hpp"

class OpenGLModel : public Configurable, public ResourceBased {
  std::vector<OpenGLMesh> meshes;
  OpenGLTexture albedo;
  OpenGLTexture normal;
  OpenGLTexture metallicRoughness;
  OpenGLTexture emissive;
  OpenGLTexture occlusion;

  glm::mat4x4 transform{};

  u32 transformUni = 0;

  bool LoadToml(const toml::parse_result &toml) override;
  void SaveToml() override;
  bool LoadResource(u64 id) override;

 public:
  OpenGLModel() = default;

  void BindTextures();
  void Draw(u32 shaderId);
  void UnBindTextures();
  void TransformModel(const glm::mat4x4 trans);
  void DestroyModel();

  ~OpenGLModel() = default;
};