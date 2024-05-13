#include "PCH.hpp"

#include "windowManager.hpp"

Window *Window::instance;

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

Window::Window() {
  window = nullptr;
  screen = nullptr;
}

bool Window::LoadToml(const toml::parse_result &toml) {
  if (instance) {
	return false;
  }

  height = toml["Screen"]["Height"].value_or(800);
  width = toml["Screen"]["Width"].value_or(800);
  mode = toml["Screen"]["Mode"].value_or("Window");

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (mode == "FullScreen") {
	window = glfwCreateWindow(width, height, applicationName.c_str(), glfwGetPrimaryMonitor(), nullptr);
	screen = glfwGetPrimaryMonitor();
  } else if (mode == "Borderless") {
	window = glfwCreateWindow(width, height, applicationName.c_str(), nullptr, nullptr);
	screen = glfwGetPrimaryMonitor();
	const GLFWvidmode *vidMode = glfwGetVideoMode(screen);
	glfwWindowHint(GLFW_RED_BITS, vidMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, vidMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, vidMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, vidMode->refreshRate);
	glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 2560, 1440, vidMode->refreshRate);
  } else {
	window = glfwCreateWindow(width, height, applicationName.c_str(), nullptr, nullptr);
	screen = glfwGetPrimaryMonitor();
  }
  if (window == nullptr) {
	std::cout << "Failed to create GLFW window" << std::endl;
	glfwTerminate();
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
	std::cout << "Failed to initialize GLAD" << std::endl;
	abort();
  }
  GlCall(glViewport(0, 0, width, height));
  glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

  instance = this;

  return true;
}

void Window::SaveToml() {

}

Window::~Window() {
  glfwTerminate();
}

void Window::PollEvents() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, true);
}

float Window::GetWindowRatio() {
  return width / height;
}

void Window::UpdateWindow() {
  glfwSwapBuffers(window);
  glfwPollEvents();
}

bool Window::CloseWindow() {
  return glfwWindowShouldClose(window);
}
GLFWwindow *Window::GetWindowPointer() {
  return window;
}
Window *Window::GetHandle() {
  return instance;
}
