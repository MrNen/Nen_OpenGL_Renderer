#pragma once

class SceneLighting {

#pragma pack(push, 1)
  struct PointLight {
	glm::vec3 position;
	float padding1 = 0;
	glm::vec3 color;
	float padding2 = 0;
  };

  std::array<PointLight, 10> pointLights;
#pragma pack(pop)

  u32 lightBufferObject;
  u32 numberOfActiveLights;

  u32 radianceMapVertices;
  u32 radianceMapVertexArrayId;
  u32 radianceMapTextureId;
  
  glm::vec3 directionLight;
  glm::vec3 directionColor;

 public:

  void BindPointLights(u32 shaderId);
  void CreateRadianceCubeMap(std::string &textureFile);
  SceneLighting() = default;
  void InitLighting(u32 shaderId);
  void AddLight(glm::vec3 color, glm::vec3 position);
  void RemoveLight(u32 lightIndex);

};
