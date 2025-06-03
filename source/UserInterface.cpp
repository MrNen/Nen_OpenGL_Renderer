//
// Created by MrNen on 6/2/2025.
//

#include "UserInterface.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"


const char* glslVersion = "#version 430";

 UserInterface::UserInterface(GLFWwindow* window){

   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   io = ImGui::GetIO();

   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

   ImGui::StyleColorsDark();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glslVersion);
   io.Fonts->AddFontDefault();
}


UserInterface::~UserInterface(){
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
}

void UserInterface::Debug(){
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();
   bool show_demo_window = true;
   ImGui::ShowDemoWindow(&show_demo_window);
   ImGui::Render();
}