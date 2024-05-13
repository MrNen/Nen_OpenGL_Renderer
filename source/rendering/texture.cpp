#include "PCH.hpp"

#include "Rendering/texture.hpp"

OpenGLTexture::OpenGLTexture(const Resources::Texture &texRef) {
  textureBufferObject = 0;
  GlCall(glGenTextures(1, &textureBufferObject));

  if (texRef.componentNumber == 1)
	format = GL_RED;
  else if (texRef.componentNumber == 3)
	format = GL_RGB;
  else if (texRef.componentNumber == 4)
	format = GL_RGBA;

  GlCall(glBindTexture(GL_TEXTURE_2D, textureBufferObject));
  GlCall(glTexImage2D(GL_TEXTURE_2D,
					  0,
					  format,
					  texRef.width,
					  texRef.height,
					  0,
					  format,
					  GL_UNSIGNED_BYTE,
					  texRef.textureData));
  GlCall(glGenerateMipmap(GL_TEXTURE_2D));

  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

  GlCall(glBindTexture(GL_TEXTURE_2D, 0));

  isValid = true;
}

OpenGLTexture::OpenGLTexture() {
  isValid = false;
}

void OpenGLTexture::DisposeOfTexture() {
  glDeleteTextures(1, &textureBufferObject);
  isValid = false;
}

void OpenGLTexture::BindTexture() const {
  if (isValid) {
	GlCall(glBindTexture(GL_TEXTURE_2D, textureBufferObject));
  }
}
void OpenGLTexture::UnBindTexture() const {
  if (isValid) {
	GlCall(glBindTexture(GL_TEXTURE_2D, 0));
  }
}
