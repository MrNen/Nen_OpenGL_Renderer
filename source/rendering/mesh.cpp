#include "PCH.hpp"

#include "Rendering/renderDefs.h"
#include "Rendering/mesh.hpp"

OpenGLMesh::OpenGLMesh(const Resources::Mesh &meshRef) {
  archetype = meshRef.archetypeId;
  vertexAttributeArray = 0;
  vertexBufferObject = 0;
  indexBufferObject = 0;

  GlCall(glGenVertexArrays(1, &vertexAttributeArray));
  GlCall(glGenBuffers(1, &vertexBufferObject));
  GlCall(glGenBuffers(1, &indexBufferObject));

  GlCall(glBindVertexArray(vertexAttributeArray));

  GlCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject));
  GlCall(glBufferData(GL_ARRAY_BUFFER,
					  meshRef.vertexes.size() * sizeof(Vertex),
					  meshRef.vertexes.data(),
					  GL_STATIC_DRAW));

  GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
  GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					  meshRef.indices.size() * sizeof(unsigned int),
					  meshRef.indices.data(),
					  GL_STATIC_DRAW));

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(nullptr));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, normal)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoordinates));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, boneIds));

  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, boneWeights));

  size = meshRef.indices.size();

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  transform = {};
}

void OpenGLMesh::DrawMesh() {
  GlCall(glBindVertexArray(vertexAttributeArray));
  GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject))

  GlCall(glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr));
  glBindVertexArray(0);
}

void OpenGLMesh::DisposeOfMesh() {
  glDeleteBuffers(1, &vertexBufferObject);
  glDeleteBuffers(1, &indexBufferObject);
  glDeleteVertexArrays(1, &vertexAttributeArray);
}

