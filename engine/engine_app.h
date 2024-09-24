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

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <filesystem>
#include <memory>
#include <utility>

namespace loki {

  struct EngineSettings
  {
    std::filesystem::path root_path;
  };

  class EngineApp
  {
  public:
    virtual ~EngineApp() = default;

  public:
    auto launch(const std::shared_ptr<EngineSettings>& settings) -> int;

  protected:
    virtual bool on_init();
    virtual void on_term();

    virtual void on_update() = 0;
    virtual void on_render() = 0;
    virtual void on_gui() = 0;

  protected:
    auto get_delta_time() const -> float
    {
      return delta_time;
    }

    auto get_fps() const -> float
    {
      return delta_time != 0.f ? 1.f / delta_time : 0.f;
    }

    auto get_root_path() const -> std::filesystem::path
    {
      return settings->root_path;
    }

    auto get_window() const -> GLFWwindow*
    {
      return window;
    }

  private:
    std::shared_ptr<EngineSettings> settings{ nullptr };
    float delta_time{ 0.f };
    GLFWwindow* window{ nullptr };
  };

} // namespace loki

