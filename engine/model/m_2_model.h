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

#include "engine/asset/asset.h"
#include "engine/utils/types.h"

#include "glm/vec3.hpp"

namespace loki {

  class M2Model : public Asset
  {
  protected:
    void load_all(const loki::MPQFile& file) override;

  private:
#pragma pack(push, 1)

    struct Sphere
    {
      glm::vec3 min;
      glm::vec3 max;
      float radius;
    };

    struct Record
    {
      union
      {
        u32 number;
        u32 length;
      };

      u32 offset;
    };

    struct Header
    {
      u8 id[4];
      u8 version[4];
      Record name;
      u32 global_model_flags;
      Record global_sequence;
      Record animations;
      Record animation_lookup;
      Record bones;
      Record key_bone_lookup;
      Record vertices;
      u32 number_of_views;
      Record colors;
      Record textures;
      Record transparency;
      Record tex_anims;
      Record tex_lookup;
      Record tex_unit_lookup;
      Record transparency_lookup;
      Record tex_anim_lookup;
      Sphere collision_sphere;
      Sphere bound_sphere;
      Record bounding_triangles;
      Record bounding_vertices;
      Record bounding_normals;
      Record attachments;
      Record attach_lookup;
      Record events;
      Record lights;
      Record cameras;
      Record camera_lookup;
      Record ribbon_emitters;
      Record particles_emitters;
      Record texture_combiner_combos;
    };

#pragma pack(pop)

    Header header{};
  };

} // namespace loki
