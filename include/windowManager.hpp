#pragma once

#include "Rendering/renderDefs.h"

//Manages the instancing, context and updating of the application window.
class Window final : public Configurable {
  GLFWwindow *window;
  int width = 600;
  int height = 800;

  static Window *instance;
  std::string mode;
  GLFWmonitor *screen;
  std::string applicationName = "Engine";
  bool LoadToml(const toml::parse_result &toml) override;

 public:
  Window(const Window &) = delete;
  Window(Window &&) = delete;
  Window();
  ~Window();
  void UpdateWindow();
  bool CloseWindow();
  void PollEvents();
  float GetWindowRatio();
  void SaveToml() override;
  static Window *GetHandle();

  GLFWwindow *GetWindowPointer();

};
