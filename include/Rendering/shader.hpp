#pragma once
#include "renderDefs.h"
#include <fstream>
#include <sstream>

class ShaderProgram : public Configurable {
  bool LoadToml(const toml::parse_result &toml) override;
  void SaveToml() override;

 public:
  unsigned int id;
  ShaderProgram() = default;
  void UseProgram() const;
  ~ShaderProgram() override = default;

  void DeleteShader();
};