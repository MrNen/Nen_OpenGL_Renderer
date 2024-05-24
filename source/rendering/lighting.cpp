#include "PCH.hpp"

#include "Rendering/lighting.hpp"
#include "Rendering/renderDefs.h"
#include "glm/gtc/type_ptr.hpp"

void SceneLighting::BindPointLights(u32 shaderId) {
  u32 uniformLightIndex = glGetUniformBlockIndex(shaderId, "pointLights");
  glUniformBlockBinding(shaderId, uniformLightIndex, UniformBlockBindings::lights);
  glBindBuffer(GL_UNIFORM_BUFFER, lightBufferObject);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(pointLights), pointLights.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightBufferObject);
  
  glProgramUniform1uiv(shaderId, Uniform::numberOfLights, 1, &numberOfActiveLights);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SceneLighting::InitLighting(u32 shaderId) {
  glGenBuffers(1, &lightBufferObject);
  glBindBuffer(GL_UNIFORM_BUFFER, lightBufferObject);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(pointLights), nullptr, GL_STATIC_DRAW);

  u32 uniformLightIndex = glGetUniformBlockIndex(shaderId, "pointLights");
  glUniformBlockBinding(shaderId, uniformLightIndex, UniformBlockBindings::lights);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(pointLights), pointLights.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightBufferObject);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  for (auto &val : pointLights) {
	val.color = {0.0f, 0.0f, 0.0f};
	val.position = {0.0f, 0.0f, 0.0f};
  }

  glProgramUniform1ui(shaderId, Uniform::numberOfLights, 0);
  numberOfActiveLights = 0;
}
void SceneLighting::AddLight(glm::vec3 color, glm::vec3 position) {

  auto lightIndex = std::ranges::find_if(pointLights, [](const PointLight &a) {
	return (a.color == glm::vec3(0.0f, 0.0f, 0.0f));
  });
  lightIndex->color = color;
  lightIndex->position = position;

  numberOfActiveLights += 1;
}
void SceneLighting::RemoveLight(u32 lightIndex) {
  pointLights[lightIndex].color = {};
  pointLights[lightIndex].position = {};
  numberOfActiveLights -= 1;
//Essentially if a light has a "color" of (0,0,0) it is deactivated, and thus we need to put it at the rear of the array so
// to make sure only active lights are in the front of the array.
  std::ranges::sort(pointLights, [](const PointLight &a, const PointLight &b) {
	return glm::length(a.color) > glm::length(b.color);
  });

}
