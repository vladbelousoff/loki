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
#include "engine/texture/blp_texture.h"
#include "engine/utils/types.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace loki {

  enum class TextureType : u32
  {
    FILENAME = 0,
    BODY,
    CAPE,
    ITEM = CAPE,
    ARMOR_REFLECT,
    HAIR = 6,
    FUR = 8,
    INVENTORY_ART_1,
    QUILL,
    GAME_OBJECT_1,
    GAME_OBJECT_2,
    GAME_OBJECT_3,
    TEXTURE_15,
    TEXTURE_16,
    TEXTURE_17,
  };

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

  constexpr int TextureMaxCount = 32;

  struct M2ModelTextureDef
  {
    TextureType type;
    u32 flags;
    M2Field name;
  };

#pragma pack(pop)

  class M2Model : public AssetWrapper<M2Model>
  {
  public:
    void draw() const;

  protected:
    void on_fully_loaded(const std::vector<char>& buffer) override;

  private:
#pragma pack(push, 1)

    struct Sphere
    {
      glm::vec3 min;
      glm::vec3 max;
      float radius;
    };

    struct Header
    {
      u8 id[4];
      u8 version[4];
      M2Field name;
      u32 global_model_flags;
      M2Field global_sequence;
      M2Field animations;
      M2Field animation_lookup;
      M2Field bones;
      M2Field key_bone_lookup;
      M2Field vertices;
      u32 number_of_views;
      M2Field colors;
      M2Field textures;
      M2Field transparency;
      M2Field tex_anims;
      M2Field tex_lookup;
      M2Field tex_unit_lookup;
      M2Field transparency_lookup;
      M2Field tex_anim_lookup;
      Sphere collision_sphere;
      Sphere bound_sphere;
      M2Field bounding_triangles;
      M2Field bounding_vertices;
      M2Field bounding_normals;
      M2Field attachments;
      M2Field attach_lookup;
      M2Field events;
      M2Field lights;
      M2Field cameras;
      M2Field camera_lookup;
      M2Field ribbon_emitters;
      M2Field particles_emitters;
      M2Field texture_combiner_combos;
    };

#pragma pack(pop)

    std::vector<char> model_name;
    std::vector<ModelVertex> raw_vertices;
    std::vector<std::shared_ptr<M2ModelView>> model_views;
    std::vector<std::shared_ptr<BLPTexture>> textures;
    GLuint vao;
    GLuint vbuf;
    GLuint nbuf;
    GLuint tbuf;
  };

} // namespace loki
