#pragma once

struct KeyStates {
  bool forwardKeyPressed = false;
  bool backwardKeyPressed = false;
  bool leftKeyPressed = false;
  bool rightKeyPressed = false;
};

class PlayerController final : public Configurable {
  bool LoadToml(const toml::parse_result &toml) override;
  void SaveToml() override;

  u32 forwardKey;
  u32 backwardsKey;
  u32 rightKey;
  u32 leftKey;
  u32 menuKey;
  double updateDelta;

  float sensitivity;
  bool inMenu = true;
  glm::vec2 mousePos;
  glm::vec2 wishDir;
  glm::vec2 lastMousePos;
  float accumulatedPitch;
  float accumulatedYaw;

  KeyStates keyStates;

  static void MouseCallback(GLFWwindow *window, double x, double y);
  static void KeyPressCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

 public:

  float GetPitch();
  float GetYaw();
  glm::vec2 GetWishDirection();

  void ResetPitchYaw();
  void EnableMouseCursor();
  void DisableMouseCursor();

  void ProcessInputs(double frameDelta);

};