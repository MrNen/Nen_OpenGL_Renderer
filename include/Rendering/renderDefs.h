//Header file for external include necessary for rendering related systems as well as macros
#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#define MAX_BONE_INFLUENCE 4

#define ASSERT(x) if (!(x)) __debugbreak();
#define GlCall(x) GlClearError();\
    x;\
    ASSERT(GlLogCall(#x, __FILE__, __LINE__))

constexpr glm::vec3 worldUp = {0.0, 1.0, 0.0};

enum Uniform {
  modelTransform,
  cameraView,
  cameraProjection,
  albedoSampler,
  roughnessSampler,
  normalSampler,
  ambientLight,
  cameraPosition,
  lightPos,
  lightColor
};

enum VertexAttributeType {
  position,
  normal,
  tangent,
  texcoord_n,
  color_n
};

enum Mode : uint8_t {
  Points = 0,
  Lines = 1,
  LineLoop = 2,
  LineStrip = 3,
  Triangles = 4,
  TriangleStrip = 5,
  TriangleFan = 6
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoordinates;
  glm::vec4 tangent;
  glm::vec3 bitangent;
  int boneIds[MAX_BONE_INFLUENCE];
  float boneWeights[MAX_BONE_INFLUENCE];
  glm::vec2 color;
};

//Structs in this namespace represent the actual data for rendering resources and don't handle anything on the OpenGL side.
namespace Resources {
struct Mesh {
  u64 archetypeId;
  std::vector<u32> indices;
  std::vector<Vertex> vertexes;
  Mode mode;
};
struct Texture {
  //should match the model's archetype Id
  u64 archetypeId;

  int width, height, componentNumber;
  unsigned char *textureData;
};

struct Material {
  u16 aoIndex;
  u16 albedoIndex;
  u16 metallicRoughnessIndex;
  u16 normalIndex;
  u16 emissiveIndex;
  glm::vec3 emissiveFactor;
};

struct Model {
  u64 archetypeId;
  std::vector<Mesh> meshes;
  std::vector<Texture> textures;
  std::vector<Material> materials;
};

}// namespace Resources
[[maybe_unused]] static void GlClearError() {
  while (glGetError() != GL_NO_ERROR);
}

[[maybe_unused]] static bool GlLogCall(const char *function, const char *file, int line) {
  while (GLenum error = glGetError()) {
	std::cout << "[OpenGL Error (" << error << ")" << function << " in File " << file << " on line " << line << " ]"
			  << std::endl;
	return false;
  }
  return true;
}