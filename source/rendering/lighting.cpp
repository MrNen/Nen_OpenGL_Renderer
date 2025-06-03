#include "PCH.hpp"

#include "Rendering/lighting.hpp"

#include "PCH.hpp"
#include "PCH.hpp"
#include "PCH.hpp"
#include "PCH.hpp"
#include "Rendering/renderDefs.h"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include "Rendering/shader.hpp"

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
  pointLights[lightIndex].color = {0.0f, 0.0f, 0.0f};
  pointLights[lightIndex].position = {0.0f, 0.0f, 0.0f};
  numberOfActiveLights -= 1;
//Essentially if a light has a "color" of (0,0,0) it is deactivated, and thus we need to put it at the rear of the array so
// to make sure only active lights are in the front of the array.
  std::ranges::sort(pointLights, [](const PointLight &a, const PointLight &b) {
	return glm::length(a.color) > glm::length(b.color);
  });

}void SceneLighting::SetDirectionalLight(glm::vec3 direction, glm::vec3 color,u32 shaderId){
  directionLight = glm::normalize(direction);
  directionColor = color;
  glProgramUniform3fv(shaderId, Uniform::directionalLight, 1, glm::value_ptr(directionLight));
  glProgramUniform3fv(shaderId, Uniform::directionalLightColor, 1, glm::value_ptr(directionColor));
}


void SceneLighting::CreateRadianceCubeMap(std::string &textureFile) {
  int width, height, nrChannels;
  unsigned int hdrTexture;
  std::string path = std::format("{}{}", std::filesystem::current_path().string(), textureFile);
  float *data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
  } else {
	std::cout << "Failed to load HDR image." << std::endl;
  }
  unsigned int captureFBO, captureRBO;
  glGenFramebuffers(1, &captureFBO);
  glGenRenderbuffers(1, &captureRBO);

  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

  glGenTextures(1, &radianceMapTextureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, radianceMapTextureId);
  for (unsigned int i = 0; i < 6; ++i) {
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
				 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
  }

  std::string tomlPath = std::format("{}/resources/config/radiance.toml", std::filesystem::current_path().string());

  auto toml = toml::parse_file(tomlPath);

  auto cubeMapShader = ConfigLoader::CreateConfigurable<ShaderProgram>(toml);

  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  std::array captureViews =
	  {
		  glm::mat4(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
								glm::vec3(1.0f, 0.0f, 0.0f),
								glm::vec3(0.0f, -1.0f, 0.0f))),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	  };
  cubeMapShader.UseProgram();

  glProgramUniformMatrix4fv(cubeMapShader.id,
							Uniform::cameraProjection,
							1,
							GL_FALSE,
							glm::value_ptr(captureProjection));

  glViewport(0, 0, 512, 512);

  for (const auto view : captureViews) {
	glProgramUniformMatrix4fv(cubeMapShader.id,
							  Uniform::cameraView,
							  1,
							  GL_FALSE,
							  glm::value_ptr(view));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
