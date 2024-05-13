#pragma once
#include "renderDefs.h"

class OpenGLTexture {
  GLuint textureBufferObject;
  GLenum format;

  bool isValid;

 public:
  void BindTexture() const;
  void UnBindTexture() const;
  [[nodiscard]] bool isValidTexture() const {
	return isValid;
  }
  OpenGLTexture();

  void DisposeOfTexture();

  OpenGLTexture(const Resources::Texture &texRef);
  ~OpenGLTexture() = default;
};
