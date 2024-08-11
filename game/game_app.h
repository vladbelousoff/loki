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

#include "engine/engine_app.h"
#include "engine/network/auth_session.h"
#include "engine/network/world_session.h"
#include "engine/render/shader.h"
#include "glm/detail/type_mat4x4.hpp"
#include "glm/vec3.hpp"

class GameApp : public loki::EngineApp
{
protected:
  bool on_init() override;
  void on_term() override;

  void on_update() override;
  void on_render() override;
  void on_gui() override;

private:
  glm::vec3 background{ 0.144f, 0.186f, 0.311f };
  glm::mat4 model{ 1.f };
  glm::mat4 view{};
  glm::mat4 projection{};
  std::shared_ptr<loki::AuthSession> auth_session;
  std::shared_ptr<loki::WorldSession> world_session;
};

