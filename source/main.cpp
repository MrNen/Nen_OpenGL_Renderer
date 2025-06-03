#include "PCH.hpp"
#include "UserInterface.hpp"

#include <imgui_impl_opengl3.h>
#include <thread>

static_assert(sizeof(unsigned long long) == sizeof(void *), "This application only supports x64 architecture");

#include <chrono>
#include <filesystem>
#include <cstdlib>

#include "resourceloader.hpp"
#include "windowManager.hpp"
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "scene.hpp"

using namespace std::chrono_literals;

int main() {

  std::string root = std::getenv("N_OPGL_ROOT");
  std::filesystem::current_path(root);

  const auto configLoader = ConfigLoader::CreateConfigLoader();
  const auto window = ConfigLoader::CreateConfigurablePtr<Window>(configLoader->LoadSettings());
  const auto Ui = std::make_unique<UserInterface>(window->GetWindowPointer());


  ResourceLoader::CreateResourceLoader();

  u64 accumulator = 0;
  u64 previousFrame =
	  duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  u64 currentFrame = previousFrame;

  //Things break if frametime is 0, the loop itself gaurantees it will be at least 4
  // so we set it to 4 manually for the first frame of the application.
  u64 frameTime = 4;

  glEnable(GL_DEPTH_TEST);
  auto scene = ConfigLoader::CreateConfigurablePtr<Scene>(configLoader->LoadSettings());

  scene->Test();

  while (!window->CloseWindow()) {
	previousFrame = currentFrame;
	window->PollEvents();

    Ui->Debug();
	// Fixed time-step update
	while (accumulator >= 16) {
	  accumulator -= deltaTimeUint;
	  scene->FixedUpdate(deltaTimeUint);

	  //if for whatever reason we acquire too large of a buffer for updates we should skip them to keep the
	  //performance consistent and not get stuck doing updates forever.
	  if (accumulator >= 160)
		accumulator = 0;
	  //Fixed Update processes.
	}

	scene->FrameUpdate(static_cast<double>(frameTime));
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	window->UpdateWindow();

    //Cap the app update time to at least every 4ms (250 fps)
    while (true){
	currentFrame =
		duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	frameTime = currentFrame - previousFrame;

      if (frameTime >= 4) {
        accumulator += currentFrame - previousFrame;
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }


  }
  return 0;
}
