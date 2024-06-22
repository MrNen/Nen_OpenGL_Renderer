#include "PCH.hpp"

#include  "Rendering/renderState.hpp"

void RenderState::Update(const GameState &state1, const GameState &state2, u64 currentFrameTime) {
  // models[0].TransformModel(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}));

}

void RenderState::Render() {
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderProgram.UseProgram();
  camera.UpdateCameraTransform(shaderProgram.id);
  for (auto &m : models) {
	m.BindTextures();
	m.Draw(shaderProgram.id);
	m.UnBindTextures();
  }
  skybox.UpdateCameraData(camera.viewMatrix, camera.projectionMatrix);
  skybox.RenderSkybox();
}

void RenderState::AddModel(u64 id) {
  models.emplace_back(ConfigLoader::CreateConfigurable<OpenGLModel>(id));

  const u64 id2 = std::hash<std::string>{}("Damaged_Helmet");

  models.emplace_back(ConfigLoader::CreateConfigurable<OpenGLModel>(id2));

  glm::mat4 transform = glm::translate(glm::mat4(1), {4.0, 0.0, 0.0});
  models[1].TransformModel(transform);

}

RenderState::RenderState() {
  auto configLoader = ConfigLoader::GetHandle();
  models.reserve(20);
  camera = ConfigLoader::CreateConfigurable<Camera>(configLoader->LoadSettings());
  shaderProgram = ConfigLoader::CreateConfigurable<ShaderProgram>(configLoader->LoadSettings());
  skybox =
	  ConfigLoader::CreateConfigurable<Skybox>(ConfigLoader::GetHandle()->LoadConfigFile("resources/config/skybox.toml"));
  camera.InitCamera(shaderProgram.id);
  lighting.InitLighting(shaderProgram.id);
  lighting.BindPointLights(shaderProgram.id);

  std::string radianceFile = "/resources/textures/sky.hdr";
  lighting.CreateRadianceCubeMap(radianceFile);

  glm::vec3 lightPos = {5.0f, 4.0f, 4.0f};
  glm::vec3 lightDiffuse = {.00f, .00f, 1.80f};

  glm::vec3 lightPos2 = {-5.0f, 4.0f, 4.0f};
  glm::vec3 lightDiffuse2 = {1.90f, .00f, .00f};

  lighting.AddLight(lightDiffuse, lightPos);
  lighting.AddLight(lightDiffuse2, lightPos2);
  lighting.BindPointLights(shaderProgram.id);

}