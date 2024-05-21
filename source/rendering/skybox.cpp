#include <glm/gtc/type_ptr.hpp>
#include "PCH.hpp"

#include "Rendering/skybox.hpp"
#include "stb_image.h"

void Skybox::loadCubeMap(std::vector<std::string> &faces) {
  textureId = 0;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

  std::string path = std::filesystem::current_path().string();

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
	unsigned char *data = stbi_load(std::format("{}{}", path, faces[i]).c_str(), &width, &height, &nrChannels, 0);
	if (data) {
	  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				   0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
	  );
	  stbi_image_free(data);
	} else {
	  std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
	  stbi_image_free(data);
	}
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

bool Skybox::LoadToml(const toml::parse_result &toml) {
  std::vector<std::string> faces;

  std::vector<float> vertexData = {
	  -1.0f, 1.0f, -1.0f,
	  -1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,
	  1.0f, 1.0f, -1.0f,
	  -1.0f, 1.0f, -1.0f,

	  -1.0f, -1.0f, 1.0f,
	  -1.0f, -1.0f, -1.0f,
	  -1.0f, 1.0f, -1.0f,
	  -1.0f, 1.0f, -1.0f,
	  -1.0f, 1.0f, 1.0f,
	  -1.0f, -1.0f, 1.0f,

	  1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,

	  -1.0f, -1.0f, 1.0f,
	  -1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, -1.0f, 1.0f,
	  -1.0f, -1.0f, 1.0f,

	  -1.0f, 1.0f, -1.0f,
	  1.0f, 1.0f, -1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  -1.0f, 1.0f, 1.0f,
	  -1.0f, 1.0f, -1.0f,

	  -1.0f, -1.0f, -1.0f,
	  -1.0f, -1.0f, 1.0f,
	  1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,
	  -1.0f, -1.0f, 1.0f,
	  1.0f, -1.0f, 1.0f
  };

  u32 test = vertexData.size();

  glGenVertexArrays(1, &vertexArrayId);
  glGenBuffers(1, &vertices);
  glBindVertexArray(vertexArrayId);
  glBindBuffer(GL_ARRAY_BUFFER, vertices);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  faces.emplace_back(toml["Images"]["right"].value_or(""));
  faces.emplace_back(toml["Images"]["left"].value_or(""));
  faces.emplace_back(toml["Images"]["top"].value_or(""));
  faces.emplace_back(toml["Images"]["bottom"].value_or(""));
  faces.emplace_back(toml["Images"]["front"].value_or(""));
  faces.emplace_back(toml["Images"]["back"].value_or(""));

  loadCubeMap(faces);

  shader = ConfigLoader::CreateConfigurable<ShaderProgram>(toml);

  return true;

}
void Skybox::SaveToml() {

}
void Skybox::RenderSkybox() {
  glDepthFunc(GL_LEQUAL);
  shader.UseProgram();
  GlCall(glBindVertexArray(vertexArrayId));
  glActiveTexture(GL_TEXTURE0);
  GlCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureId));
  GlCall(glDrawArrays(GL_TRIANGLES, 0, 36));

  GlCall(glBindVertexArray(0));
  GlCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
  glDepthFunc(GL_LESS);
}
void Skybox::UpdateCameraData(glm::mat4 view, glm::mat4 projection) {

  //We remove the translation part of the camera to make sure the cubemap is always centered about the camera.
  glm::mat4 noTransformView = glm::mat4(glm::mat3(view));
  glProgramUniformMatrix4fv(shader.id, Uniform::cameraProjection, 1, GL_FALSE, glm::value_ptr(projection));
  glProgramUniformMatrix4fv(shader.id, Uniform::cameraView, 1, GL_FALSE, glm::value_ptr(noTransformView));
}
