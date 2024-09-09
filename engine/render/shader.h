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

#pragma once

#include <cstdint>
#include <glm/fwd.hpp>
#include <spdlog/spdlog.h>
#include <string_view>

namespace loki {

  enum class ShaderType
  {
    VERT,
    FRAG,
  };

  struct ShaderManager;

  class ShaderHandle
  {
    friend struct ShaderManager;

  private:
    uint32_t id;
  };

  class ProgramHandle
  {
    friend struct ShaderManager;
    friend class UniformManager;

  private:
    uint32_t id;
  };

  class UniformManager
  {
  public:
    explicit UniformManager(ProgramHandle handle)
      : handle{ handle }
    {
    }

  public:
    auto set_uniform(std::string_view name, float value) const -> void;
    auto set_uniform(std::string_view name, const glm::vec3& vec) const -> void;
    auto set_uniform(std::string_view name, const glm::mat4& mat) const -> void;

  private:
    ProgramHandle handle;
  };

  struct ShaderManager
  {
    // general stuff
    static auto create_shader(std::string_view source, ShaderType type) -> ShaderHandle;
    static auto create_program(ShaderHandle vert, ShaderHandle frag) -> ProgramHandle;
    static auto use_program(ProgramHandle handle, const std::function<void(const UniformManager& manager)>& callback) -> void;
  };
} // namespace loki

template<>
struct fmt::formatter<loki::ShaderType> : fmt::formatter<std::string>
{
  static auto format(const loki::ShaderType& type, fmt::format_context& ctx) -> decltype(ctx.out())
  {
    switch (type) {
      case loki::ShaderType::VERT:
        return fmt::format_to(ctx.out(), "loki::ShaderType::VERT");
      case loki::ShaderType::FRAG:
        return fmt::format_to(ctx.out(), "loki::ShaderType::FRAG");
    }
    return fmt::format_to(ctx.out(), "Unknown");
  }
};
