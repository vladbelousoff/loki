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

#include "m_2_model_view.h"

#include "GL/glew.h"
#include "engine/asset/asset.h"
#include "engine/utils/types.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace loki {

  class M2Model : public AssetWrapper<M2Model>
  {
  public:
    void draw() const;

  protected:
    void on_fully_loaded(const std::vector<char>& buffer) override;

  private:
#pragma pack(push, 1)

    struct ModelVertex
    {
      glm::vec3 pos;
      u8 weights[4];
      u8 bones[4];
      glm::vec3 normal;
      glm::vec2 texcoords;
      int unk1, unk2; // always 0,0 so this is probably unused
    };

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

    std::vector<char> model_name;
    std::vector<ModelVertex> raw_vertices;
    std::vector<std::shared_ptr<M2ModelView>> model_views;
  };

} // namespace loki
