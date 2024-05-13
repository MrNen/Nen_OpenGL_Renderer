#include "PCH.hpp"

#include "Rendering/model.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "resourceloader.hpp"

void OpenGLModel::Draw(u32 shaderId) {
  glProgramUniformMatrix4fv(shaderId, Uniform::modelTransform, 1, GL_FALSE, glm::value_ptr(transform));
  for (auto &m : meshes) {
	m.DrawMesh();
  }
}

void OpenGLModel::BindTextures() {
  glActiveTexture(GL_TEXTURE0);
  albedo.BindTexture();
  glActiveTexture(GL_TEXTURE1);
  normal.BindTexture();
  glActiveTexture(GL_TEXTURE2);
  metallicRoughness.BindTexture();
  glActiveTexture(GL_TEXTURE3);
  occlusion.BindTexture();
  glActiveTexture(GL_TEXTURE4);
  emissive.BindTexture();
}

void OpenGLModel::UnBindTextures() {
  glActiveTexture(GL_TEXTURE0);
  albedo.UnBindTexture();
  glActiveTexture(GL_TEXTURE1);
  normal.UnBindTexture();
  glActiveTexture(GL_TEXTURE2);
  metallicRoughness.UnBindTexture();
  glActiveTexture(GL_TEXTURE3);
  occlusion.UnBindTexture();
  glActiveTexture(GL_TEXTURE4);
  emissive.UnBindTexture();
}

void OpenGLModel::TransformModel(const glm::mat4x4 trans) {
  transform *= trans;
}

bool OpenGLModel::LoadToml(const toml::parse_result &toml) {

  return true;
}
bool OpenGLModel::LoadResource(u64 id) {
  auto res = ResourceLoader::GetHandle();

  auto modelRef = res->GrabModelRef(id).lock();

  meshes.reserve(modelRef->meshes.size());

  if (modelRef->materials[0].albedoIndex != USHRT_MAX) {
	albedo = {modelRef->textures[modelRef->materials[0].albedoIndex]};
  } else {
	albedo = {};
  }

  if (modelRef->materials[0].emissiveIndex != USHRT_MAX) {
	emissive = OpenGLTexture(modelRef->textures[modelRef->materials[0].emissiveIndex]);
  } else {
	emissive = {};
  }

  if (modelRef->materials[0].normalIndex != USHRT_MAX) {
	normal = OpenGLTexture(modelRef->textures[modelRef->materials[0].normalIndex]);
  } else {
	normal = {};
  }

  if (modelRef->materials[0].aoIndex != USHRT_MAX) {
	occlusion = OpenGLTexture(modelRef->textures[modelRef->materials[0].aoIndex]);
  } else {
	occlusion = {};
  }

  if (modelRef->materials[0].metallicRoughnessIndex != USHRT_MAX) {
	metallicRoughness = OpenGLTexture(modelRef->textures[modelRef->materials[0].metallicRoughnessIndex]);
  } else {
	metallicRoughness = {};
  }

  for (const auto &m : modelRef->meshes) {
	meshes.emplace_back(m);
  }
  transform = glm::mat4(1.0f);

  return true;
}
void OpenGLModel::SaveToml() {

}
void OpenGLModel::DestroyModel() {
  albedo.DisposeOfTexture();
  emissive.DisposeOfTexture();
  normal.DisposeOfTexture();
  occlusion.DisposeOfTexture();
  metallicRoughness.DisposeOfTexture();
  for (auto &m : meshes) {
	m.DisposeOfMesh();
  }
}

