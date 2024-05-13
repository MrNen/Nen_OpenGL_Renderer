#pragma once
#include "renderDefs.h"

//container of the OpenGL buffer pointer objects for vertices and indices.
class OpenGLMesh {
  u64 archetype;
  GLuint vertexBufferObject;
  GLuint indexBufferObject;
  GLuint vertexAttributeArray;
  glm::mat4x4 transform;
  GLsizei size;

  OpenGLMesh() = default;

 public:
  explicit OpenGLMesh(const Resources::Mesh &meshRef);
  void DrawMesh();
  void DisposeOfMesh();
  ~OpenGLMesh() = default;
};