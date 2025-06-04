#include "PCH.hpp"

#include  "Rendering/renderState.hpp"

#include <glm/ext/matrix_transform.hpp>

void RenderState::Update(const GameState &state1, const GameState &state2, u64 currentFrameTime) {
  // models[0].TransformModel(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}));

}

void RenderState::Render() {
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

}void RenderState::UpdateCamera(float pitch, float yaw, glm::vec3 direction, double time){
  camera.RotateCamera(pitch, yaw, time);
  camera.MoveCamera(direction,time);
}

RenderState::RenderState() {
  glEnable(GL_DEPTH_TEST);
  auto configLoader = ConfigLoader::GetHandle();
  models.reserve(20);
  camera = ConfigLoader::CreateConfigurable<Camera>(configLoader->LoadSettings());
  shaderProgram = ConfigLoader::CreateConfigurable<ShaderProgram>(configLoader->LoadSettings());
  skybox =
	  ConfigLoader::CreateConfigurable<Skybox>(ConfigLoader::GetHandle()->LoadConfigFile("resources/config/skybox.toml"));
  camera.InitCamera(shaderProgram.id);
  lighting.InitLighting(shaderProgram.id);
  lighting.BindPointLights(shaderProgram.id);

  /*
  std::string radianceFile = "/resources/textures/sky.hdr";
  lighting.CreateRadianceCubeMap(radianceFile);
  */

  glm::vec3 lightPos = {5.0f, 4.0f, 4.0f};
  glm::vec3 lightDiffuse = {.00f, .00f, 10.00f};

  glm::vec3 lightPos2 = {-5.0f, 4.0f, 4.0f};
  glm::vec3 lightDiffuse2 = {10.00f, .00f, .00f};

  lighting.SetDirectionalLight({-1.2f, 1.0f, -1.3f}, {1.0f, 1.0f, 1.0f},shaderProgram.id );
  lighting.AddLight(lightDiffuse, lightPos);
  lighting.BindPointLights(shaderProgram.id);


}