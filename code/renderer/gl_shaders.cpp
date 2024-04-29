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

#include "tr_local.h"
#include <fstream>
#include <sstream>
#include <iostream>

d3HardwareShader *currentBoundShader = nullptr;

d3HardwareShader::d3HardwareShader(const std::string &vertexFilename, const std::string &fragmentFilename, std::string preprocessor) {
  std::string vertexPath = "shaders/" + vertexFilename;
  std::string fragmentPath = "shaders/" + fragmentFilename;

  std::string vertexCodeShader = readShaderFile(vertexPath);
  std::string fragmentCodeShader = readShaderFile(fragmentPath);

  std::string vertexCode = "#version 330 core\n" + preprocessor + "\n" + vertexCodeShader;
  std::string fragmentCode = "#version 330 core\n" + preprocessor + "\n" + fragmentCodeShader;

  GLuint vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
  GLuint fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

  // Create shader program
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Check for linking errors
  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cerr << "Shader program linking error: " << infoLog << std::endl;
    OutputDebugStringA(infoLog); // Output to debugger
    Int3();
  }

  // Delete the shaders as they're linked into our program now and no longer necessary
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

d3HardwareShader::~d3HardwareShader() {
  // Release shader program
  glDeleteProgram(shaderProgram);
}

std::string d3HardwareShader::readShaderFile(const std::string &filename) {
  std::string shaderCode;
  std::ifstream shaderFile;

  // Ensure ifstream objects can throw exceptions
  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // Open file
    shaderFile.open(filename);
    std::stringstream shaderStream;

    // Read file's buffer contents into streams
    shaderStream << shaderFile.rdbuf();

    // Close file handler
    shaderFile.close();

    // Convert stream into string
    shaderCode = shaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cerr << "Error reading shader file: " << filename << std::endl;
    OutputDebugStringA(("Error reading shader file: " + filename).c_str()); // Output to debugger
  }

  return shaderCode;
}

GLuint d3HardwareShader::compileShader(const std::string &source, GLenum type) {
  GLuint shader = glCreateShader(type);
  const char *shaderCode = source.c_str();
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);

  // Check for compilation errors
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "Shader compilation error: " << infoLog << std::endl;
    OutputDebugStringA(infoLog); // Output to debugger
    Int3();
  }

  return shader;
}

void d3HardwareShader::use() { 
    currentBoundShader = this;
    glUseProgram(shaderProgram); 
}

void d3HardwareShader::bindNull() {
    currentBoundShader = nullptr;
    glUseProgram(0);
}

void d3HardwareShader::setMat4(const std::string &name, float *matrix) {
  GLuint location = glGetUniformLocation(shaderProgram, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void d3HardwareShader::setInt(const std::string &name, int value) {
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void d3HardwareShader::bindTexture(const std::string &name, GLuint textureID, GLenum textureUnit) {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), textureUnit);
}

d3HardwareShaderScopedBind::d3HardwareShaderScopedBind(d3HardwareShader *shader) {
  this->shader = shader;
  shader->use();

  // Enable vertex attributes
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  // Set vertex attribute pointers (assuming arrays are tightly packed)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, GL_verts);       // Vertex positions
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, GL_tex_coords);  // Texture coordinates
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, GL_tex_coords2); // Lightmap coordinates
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, GL_colors);      // Vertex Colors
}

d3HardwareShaderScopedBind::~d3HardwareShaderScopedBind() {
  shader->bindNull();

  // Disable vertex attributes
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
}
