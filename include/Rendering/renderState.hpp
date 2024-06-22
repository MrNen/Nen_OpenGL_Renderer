#pragma once
#include "camera.hpp"
#include "renderDefs.h"
#include "gameState.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "resourceloader.hpp"
#include "lighting.hpp"
#include "Rendering/skybox.hpp"

class RenderState {

  ShaderProgram shaderProgram;
  Skybox skybox;

  SceneLighting lighting;
  std::vector<OpenGLModel> models;

 public:
  Camera camera;

  //Updates and interpolates the view based on the two game states from fixed update
  void Update(const GameState &state1, const GameState &state2, u64 currentFrameTime);
  void Render();
  void AddModel(u64 id);

  RenderState();
  ~RenderState() = default;
};
