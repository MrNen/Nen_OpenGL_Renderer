#pragma once

#include "Rendering/renderDefs.h"
#include "Rendering/renderState.hpp"
#include "gameState.hpp"
#include "playerController.hpp"

class Scene : public Configurable {

  bool LoadToml(const toml::parse_result &toml) override;
  void SaveToml() override;

  std::unique_ptr<RenderState> renderstate;
  std::unique_ptr<GameState> gamestate1;
  std::unique_ptr<GameState> gamestate2;

  std::shared_ptr<PlayerController> playerController;

 public:

  void FrameUpdate(double time);
  void FixedUpdate(u64 time);
  void Test();

  Scene() = default;
};


