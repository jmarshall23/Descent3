/*
* Descent 3
* Copyright (C) 2024 Parallax Software
*
* Descent 3: Apex
* Copyright (C) 2024 by Justin Marshall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef D3HARDWARESHADER_H
#define D3HARDWARESHADER_H

#include <string>
#include "GL/glew.h"

class d3HardwareShader {
public:
  d3HardwareShader(const std::string &vertexFilename, const std::string &fragmentFilename, std::string preprocessor);
  ~d3HardwareShader(); // Destructor to release shader program

  void use(); // Activate the shader program
  void bindNull();

  // Set uniforms
  void setMat4(const std::string &name, float *matrix);
  void setInt(const std::string &name, int value);

  // Bind textures
  void bindTexture(const std::string &name, GLuint textureID, GLenum textureUnit);
private:
  GLuint shaderProgram;

  std::string readShaderFile(const std::string &filename);
  GLuint compileShader(const std::string &source, GLenum type);
};

#endif // D3HARDWARESHADER_H