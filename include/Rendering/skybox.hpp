#pragma once

#include "shader.hpp"

class Skybox : public Configurable {

  u32 vertices;
  u32 vertexArrayId;
  u32 textureId;
  ShaderProgram shader;

  bool LoadToml(const toml::parse_result &toml) final;
  void SaveToml() override;
  void loadCubeMap(std::vector<std::string> &faces);

 public:
  void RenderSkybox();
  void UpdateCameraData(glm::mat4 view, glm::mat4 projection);

};