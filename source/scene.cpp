#include "PCH.hpp"

#include "scene.hpp"

bool Scene::LoadToml(const toml::parse_result &toml) {
  renderstate = std::make_unique<RenderState>();
  gamestate1 = {};
  gamestate2 = {};
  playerController =
	  ConfigLoader::CreateConfigurablePtr<PlayerController>(ConfigLoader::GetHandle()->LoadPlayerSettings());

  return true;
}

void Scene::SaveToml() {

}

void Scene::FixedUpdate(u64 time) {

}

void Scene::Test() {
  std::string nickName = "SF_Helmet";
  u64 id = std::hash<std::string>{}(nickName);
  renderstate->AddModel(id);
}

void Scene::FrameUpdate(double time) {

  renderstate->camera.RotateCamera(playerController->GetPitch(), playerController->GetYaw(),time);

  glm::vec3 tempWishDir(playerController->GetWishDirection().x, 0.0f, playerController->GetWishDirection().y);

  tempWishDir *= time / 1000;

  renderstate->camera.MoveCamera(tempWishDir);
  playerController->ProcessInputs(time);
  renderstate->Render();
}
