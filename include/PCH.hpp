//
// Created by MrNen on 10/24/2023.
//
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <format>
#include <filesystem>
#include <array>

const std::string execName = "OpenGLRenderer.exe";
const std::string debugRoot = "C:/projects/Nen_OpenGL_Renderer";

const uint32_t execNameSize = execName.size();

constexpr unsigned long long deltaTimeUint = 16;
constexpr double dt = .016;

using u64 = unsigned long long;
using u32 = unsigned int;
using i64 = long long;
using i32 = int;
using u16 = unsigned short;
using i16 = short;
using f32 = float;
using f64 = double;

#include "toml++/toml.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"


#include "configloader.hpp"