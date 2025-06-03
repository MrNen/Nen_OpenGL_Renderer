#include "PCH.hpp"

#include "playerController.hpp"
#include "windowManager.hpp"

constexpr double mouseScalingFactor = .03f;

bool PlayerController::LoadToml(const toml::parse_result &toml) {
  auto ptr = Window::GetHandle()->GetWindowPointer();

  glfwSetWindowUserPointer(ptr, reinterpret_cast<void *>(this));
  glfwSetCursorPosCallback(ptr, MouseCallback);
  glfwSetKeyCallback(ptr, KeyPressCallback);

  forwardKey = toml["Keybinds"]["Forward"].value_or(GLFW_KEY_W);
  backwardsKey = toml["Keybinds"]["Backwards"].value_or(GLFW_KEY_S);
  leftKey = toml["Keybinds"]["Left"].value_or(GLFW_KEY_A);
  rightKey = toml["Keybinds"]["Right"].value_or(GLFW_KEY_D);
  menuKey = toml["Keybinds"]["Menu"].value_or(GLFW_KEY_ESCAPE);
  sensitivity = toml["Mouse"]["Sensitivity"].value_or(.1f);

  lastMousePos.x = ConfigLoader::GetHandle()->LoadSettings()["Screen"]["Width"].value_or(1920) / 2;
  lastMousePos.y = ConfigLoader::GetHandle()->LoadSettings()["Screen"]["Height"].value_or(1080) / 2;

  return true;
}

void PlayerController::SaveToml() {
  std::fstream file("resources/config/keybinds.toml");

  auto tbl = toml::table{
	  {"Keybinds", toml::table{
		  {"Forward", forwardKey},
		  {"Backwards", backwardsKey},
		  {"Left", leftKey},
		  {"Right", rightKey},
		  {"Menu", menuKey},
	  }},
	  {"Mouse", toml::table{{"Sensitivity", sensitivity}}}};

  file << tbl;

  file.close();
}

void PlayerController::MouseCallback(GLFWwindow *window, double x, double y) {

  //ptr is essentially our "This" ptr for PlayerController
  const auto ptr = static_cast<PlayerController *>(glfwGetWindowUserPointer(window));
  if (ptr->inMenu) {
	ptr->mousePos = {x, y};
	ptr->lastMousePos = glm::vec2(x, y);
	return;
  }

  glm::vec2 difference = glm::vec2(x, y) - ptr->lastMousePos;

  //Simply multiplying by a constant in order to get desirable camera sensitivity with the sensitivity variable at 1.0
  difference *= mouseScalingFactor;
  difference *= ptr->sensitivity;
  difference *= ptr->updateDelta;

  ptr->accumulatedYaw += glm::radians(difference.x);
  ptr->accumulatedPitch += glm::radians(difference.y);
  ptr->lastMousePos = glm::vec2(x, y);

}

void PlayerController::EnableMouseCursor() {
  auto ptr = Window::GetHandle()->GetWindowPointer();
  glfwSetInputMode(ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  inMenu = true;
}
void PlayerController::DisableMouseCursor() {
  auto ptr = Window::GetHandle()->GetWindowPointer();
  glfwSetInputMode(ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwRawMouseMotionSupported())
	glfwSetInputMode(ptr, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  inMenu = false;
}
void PlayerController::ResetPitchYaw() {
  accumulatedPitch = 0;
  accumulatedYaw = 0;
}
void PlayerController::ProcessInputs(double frameDelta) {
  updateDelta = frameDelta;
  ResetPitchYaw();
}

float PlayerController::GetYaw()const {
  return accumulatedYaw;
}
float PlayerController::GetPitch()const {
  return accumulatedPitch;
}
void PlayerController::KeyPressCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  const auto ptr = static_cast<PlayerController *>(glfwGetWindowUserPointer(window));

  //Forward Key
  if (key == ptr->forwardKey && action == GLFW_PRESS) {
	ptr->keyStates.forwardKeyPressed = true;
  }
  if (key == ptr->forwardKey && action == GLFW_RELEASE) {
	ptr->keyStates.forwardKeyPressed = false;
  }

  //Backwards Key
  if (key == ptr->backwardsKey && action == GLFW_PRESS) {
	ptr->keyStates.backwardKeyPressed = true;
  }
  if (key == ptr->backwardsKey && action == GLFW_RELEASE) {
	ptr->keyStates.backwardKeyPressed = false;
  }

  //Leftward Key
  if (key == ptr->leftKey && action == GLFW_PRESS) {
	ptr->keyStates.leftKeyPressed = true;
  }
  if (key == ptr->leftKey && action == GLFW_RELEASE) {
	ptr->keyStates.leftKeyPressed = false;
  }

  //Rightward Key
  if (key == ptr->rightKey && action == GLFW_PRESS) {
	ptr->keyStates.rightKeyPressed = true;
  }
  if (key == ptr->rightKey && action == GLFW_RELEASE) {
	ptr->keyStates.rightKeyPressed = false;
  }

  //Menu Key
  if (key == ptr->menuKey && action == GLFW_PRESS) {
	ptr->inMenu = !ptr->inMenu;

	if (ptr->inMenu) {
	  ptr->EnableMouseCursor();
	} else {
	  ptr->DisableMouseCursor();
	}
  }
}

glm::vec2 PlayerController::GetWishDirection() {
  if (inMenu) {
	return {0.0f, 0.0f};
  }

  float forward, backwards, left, right;

  forward = keyStates.forwardKeyPressed;
  forward *= -1;
  backwards = keyStates.backwardKeyPressed;
  right = keyStates.rightKeyPressed;
  left = keyStates.leftKeyPressed;
  left *= -1.0f;

  wishDir = glm::vec2(left + right, forward + backwards);

  return wishDir;
}
