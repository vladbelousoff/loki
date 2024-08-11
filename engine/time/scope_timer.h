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

#include <GLFW/glfw3.h>

namespace loki {

  template<typename T>
  class ScopeTimer
  {
  public:
    explicit ScopeTimer(T& seconds)
      : seconds{ seconds }
      , start_timestamp{ glfwGetTime() }
    {
    }

    ~ScopeTimer()
    {
      seconds = static_cast<T>(glfwGetTime() - start_timestamp);
    }

    auto get_start() -> double
    {
      return start_timestamp;
    }

  private:
    double start_timestamp;
    T& seconds;
  };

} // namespace loki
