#include "PCH.hpp"

#include "Rendering/shader.hpp"

bool ShaderProgram::LoadToml(const toml::parse_result &toml) {
  std::string vertexPath =
	  std::format("{}{}", std::filesystem::current_path().string(), toml["Shader"]["Vertex_Path"].value_or(""));
  std::string fragmentPath =
	  std::format("{}{}", std::filesystem::current_path().string(), toml["Shader"]["Fragment_Path"].value_or(""));

  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream file;
  std::stringstream buffer;

  try {
	file.open(vertexPath);
	buffer << file.rdbuf();

	vertexCode = buffer.str();

	file.close();
	file.clear();
	buffer.clear();
	buffer.str("");

	file.open(fragmentPath);
	buffer << file.rdbuf();

	fragmentCode = buffer.str();

	file.close();
	file.clear();
	buffer.clear();
	buffer.str("");
  }
  catch (std::exception &err) {
	std::cout << err.what();
	return false;
  }

  const char *vertexShaderCode = vertexCode.c_str();
  const char *fragmentShaderCode = fragmentCode.c_str();
  unsigned int vertex = 0;
  unsigned int fragment = 0;
  int success = 0;
  char infoLog[1024] = {};

  vertex = glCreateShader(GL_VERTEX_SHADER);
  GlCall(glShaderSource(vertex, 1, &vertexShaderCode, nullptr));
  GlCall(glCompileShader(vertex));

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
	glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
  glCompileShader(fragment);

  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
	glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
	std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);

  GlCall(glGetProgramiv(id, GL_LINK_STATUS, &success))
  if (!success) {
	glGetShaderInfoLog(id, 1024, nullptr, infoLog);
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return true;
}

void ShaderProgram::SaveToml() {

}

void ShaderProgram::UseProgram() const {
  if (id == 0) {
	throw;
  }
  GlCall(glUseProgram(id))
}

void ShaderProgram::DeleteShader() {
  GlCall(glDeleteProgram(id));

  id = 0;
}
