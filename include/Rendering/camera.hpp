#pragma once

#include "renderDefs.h"
#include "toml++/toml.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera : public Configurable {

  float fieldOfView;
  glm::vec3 direction;
  glm::vec3 pos;
  glm::vec3 cameraUp;
  glm::vec3 cameraRight;

  float near;
  float far;

  glm::mat3 orientation;

  bool LoadToml(const toml::parse_result &toml) override;
  void SaveToml() override;
 public:
  glm::mat4x4 viewMatrix;
  glm::mat4x4 projectionMatrix;

  Camera() = default;
  ~Camera() override = default;

  void UpdateCameraTransform(u32 id);

  void UpdateCameraFov(glm::mat4 view, u32 id);
  void InitCamera(u32 shaderId);
  void MoveCamera(glm::vec3 vec);

  //updates pitch and yaw values and rotates the view matrix accordingly
  void RotateCamera(float p, float y,double deltaTime);
};