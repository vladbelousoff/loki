/*
 * This file is part of the Loki Project.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

auto
loki::ShaderManager::create_shader(std::string_view source, ShaderType type) -> loki::ShaderHandle
{
  uint32_t shader_id;
  switch (type) {
    case ShaderType::VERT:
      shader_id = glCreateShader(GL_VERTEX_SHADER);
      break;
    case ShaderType::FRAG:
      shader_id = glCreateShader(GL_FRAGMENT_SHADER);
      break;
  }

  const char* source_data = source.data();
  glShaderSource(shader_id, 1, (const GLchar**)&source_data, nullptr);
  glCompileShader(shader_id);

  GLint shader_compiled;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);

  if (!shader_compiled) {
    int log_msg_len;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_msg_len);
    std::vector<char> shader_log(log_msg_len + 1);
    glGetShaderInfoLog(shader_id, log_msg_len, &log_msg_len, shader_log.data());
    shader_log.at(log_msg_len) = 0;
    spdlog::error("Shader compilation failed, the compile log: {}", shader_log.data());
  } else {
    spdlog::info("Shader {} compiled successfully", type);
  }

  loki::ShaderHandle handle{};
  handle.id = shader_id;

  return handle;
}

auto
loki::ShaderManager::create_program(loki::ShaderHandle vert, loki::ShaderHandle frag) -> loki::ProgramHandle
{
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vert.id);
  glAttachShader(shader_program, frag.id);
  glLinkProgram(shader_program);

  ProgramHandle handle;
  handle.id = shader_program;

  return handle;
}

auto
loki::ShaderManager::use_program(loki::ProgramHandle handle, const std::function<void(const UniformManager& manager)>& callback) -> void
{
  glUseProgram(handle.id);
  callback(UniformManager(handle));
  glUseProgram(0);
}

auto
loki::UniformManager::set_uniform(std::string_view name, float value) const -> void
{
  glUniform1f(glGetUniformLocation(handle.id, name.data()), value);
}

auto
loki::UniformManager::set_uniform(std::string_view name, const glm::vec3& vec) const -> void
{
  glUniform3fv(glGetUniformLocation(handle.id, name.data()), 1, glm::value_ptr(vec));
}

auto
loki::UniformManager::set_uniform(std::string_view name, const glm::mat4& mat) const -> void
{
  glUniformMatrix4fv(glGetUniformLocation(handle.id, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
