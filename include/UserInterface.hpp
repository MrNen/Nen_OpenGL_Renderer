#pragma once
#include "imgui.h"
#include "GLFW/glfw3.h"

class UserInterface {

public:
  explicit UserInterface(GLFWwindow* window);
  ~UserInterface();

void Debug();

private:
  ImGuiIO io;



};