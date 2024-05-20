#include "PCH.hpp"
#include "Rendering/camera.hpp"
#include "glm/gtx/matrix_decompose.hpp"

bool Camera::LoadToml(const toml::parse_result &toml) {
  pos = {toml["Camera"]["Position"][0].value_or(0.0), toml["Camera"]["Position"][1].value_or(5.0),
		 toml["Camera"]["Position"][2].value_or(-10.0)};
  direction = {toml["Camera"]["Look_At"][0].value_or(0.0), toml["Camera"]["Look_At"][1].value_or(0.0),
			   toml["Camera"]["Look_At"][2].value_or(0.0)};
  fieldOfView = toml["Camera"]["Field_of_view"].value_or(60.0f);
  near = toml["Camera"]["Near"].value_or(0.1f);
  far = toml["Camera"]["Far"].value_or(100.0f);

  cameraUp = worldUp;
  cameraRight = glm::normalize(glm::cross(direction, worldUp));

  orientation = {cameraRight, cameraUp, direction};

  viewMatrix = glm::lookAt(pos, pos + direction, cameraUp);
  projectionMatrix = glm::perspectiveFov(glm::radians(fieldOfView), 2560.0f, 1440.0f, near, far);

  return true;
}

void Camera::SaveToml() {

}

void Camera::UpdateCameraTransform(u32 id) {

  viewMatrix = glm::lookAt(pos, pos + direction, cameraUp);
  glProgramUniformMatrix4fv(id, Uniform::cameraView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glProgramUniform3fv(id, Uniform::cameraPosition, 1, glm::value_ptr(pos));
}

void Camera::UpdateCameraFov(glm::mat4 view, u32 id) {
  projectionMatrix = view;
  glProgramUniformMatrix4fv(id, Uniform::cameraProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void Camera::InitCamera(u32 shaderId) {
  glProgramUniformMatrix4fv(shaderId, 1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glProgramUniformMatrix4fv(shaderId, 2, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}
void Camera::RotateCamera(float p, float y) {

  //Yaw the camera about the world up, this will make sure the camera is always panning horizontally relative to the horizon.
  glm::mat3 rotationYaw = glm::rotate(glm::mat4(1.0f), y, worldUp);
  glm::vec3 newDirection = direction * rotationYaw;

  //We now generate a new x-axis to pitch around.
  glm::vec3 newCameraRight = glm::normalize(glm::cross(newDirection, worldUp));
  glm::mat3 rotationPitch = glm::rotate(glm::mat4(1.0f), p, newCameraRight);
  newDirection = newDirection * rotationPitch;

  //Check to see if the pitch will result in us reaching our up and down deadzones, and if so, don't execute the pitch.
  // TODO: Recalculate the newDirection rather than returning, this may cause camera locking.
  if ((glm::dot(newDirection, worldUp) >= .96 || glm::dot(newDirection, worldUp) <= -.96)) {
	return;
  }

  direction = newDirection;
  cameraRight = newCameraRight;
  cameraUp = glm::normalize(glm::cross(cameraRight, direction));

}
void Camera::MoveCamera(glm::vec3 vec) {
  glm::quat rotation;
  glm::vec3 _;
  glm::vec4 _2;
  glm::decompose(viewMatrix, _, rotation, _, _, _2);

  auto mat = glm::inverse(glm::mat3(rotation));
  vec = mat * vec;
  pos += vec;
}
