#pragma once

class SceneLighting {

#pragma pack(push, 1)
  struct PointLight {
	glm::vec3 position;
	float padding1 = 0;
	glm::vec3 color;
	float padding2 = 0;
  };
#pragma pack(pop)

  std::vector<PointLight> pointLights;
  GLuint lightBufferObject;

  //simple debug atm.
  PointLight gpuLight;
  glm::vec3 ambient;


 public:
  void SetDebugLight(glm::vec3 newPosition, glm::vec3 newColor);
  void BindPointLight(u32 shaderId, u32 index);
  SceneLighting() = default;
  void SetAmbientLight(glm::vec3 value, u32 shaderId);
};
