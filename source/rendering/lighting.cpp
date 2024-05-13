#include "PCH.hpp"

#include "Rendering/lighting.hpp"
#include "Rendering/renderDefs.h"
#include "glm/gtc/type_ptr.hpp"

void SceneLighting::BindPointLight(u32 shaderId, u32 index) {
  u32 uniformLightIndex = glGetUniformBlockIndex(shaderId, "PointLight");
  glUniformBlockBinding(shaderId, uniformLightIndex, index);

  glBindBuffer(GL_UNIFORM_BUFFER, lightBufferObject);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(gpuLight), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightBufferObject);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void SceneLighting::SetAmbientLight(glm::vec3 value, u32 shaderId) {
  ambient = value;
  glProgramUniform3fv(shaderId, Uniform::ambientLight, 1, glm::value_ptr(ambient));
}
void SceneLighting::SetDebugLight(glm::vec3 newPosition, glm::vec3 newColor) {
  gpuLight.position = newPosition;
  gpuLight.color = newColor;
  glGenBuffers(1, &lightBufferObject);
  glBindBuffer(GL_UNIFORM_BUFFER, lightBufferObject);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(gpuLight), &gpuLight);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
