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
}

void RenderState::AddModel(u64 id) {
  models.emplace_back(ConfigLoader::CreateConfigurable<OpenGLModel>(id));
}

RenderState::RenderState() {
  auto configLoader = ConfigLoader::GetHandle();
  models.reserve(20);
  camera = ConfigLoader::CreateConfigurable<Camera>(configLoader->LoadSettings());
  shaderProgram = ConfigLoader::CreateConfigurable<ShaderProgram>(configLoader->LoadSettings());
  camera.InitCamera(shaderProgram.id);
  ambientLight = 1.0f;

  glm::vec3 lightPos = {0.0f, 1.0f, 4.0f};
  glm::vec3 lightDiffuse = {.60f, .80f, .80f};
  glProgramUniform3fv(shaderProgram.id, Uniform::lightPos, 1, glm::value_ptr(lightPos));
  glProgramUniform3fv(shaderProgram.id, Uniform::lightColor, 1, glm::value_ptr(lightDiffuse));

  lighting.SetDebugLight(lightPos, lightDiffuse);
  lighting.BindPointLight(shaderProgram.id, 0);
  lighting.SetAmbientLight({55.6f, 55.6f, 55.6f}, shaderProgram.id);
}
void RenderState::SetAmbientLight(float value) {

}

