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

#pragma pack(push, 1)

  struct M2Field
  {
    union
    {
      u32 number;
      u32 length;
    };

    u32 offset;
  };

  struct M2ModelRenderPass
  {
    i32 tex;
    i32 geoset;
  };

  struct M2ModelTexUnit
  {
    // probably the texture units
    // size always >=number of materials it seems
    u16 flags;       // Usually 16 for static textures, and 0 for animated textures.
    u16 shading;     // If set to 0x8000: shaders. Used in sky boxes to ditch the need for depth buffering. See below.
    u16 op;          // Material this texture is part of (index into mat)
    u16 op2;         // Always same as above?
    i16 colorIndex;  // A Color out of the Colors-Block or -1 if none.
    u16 flagsIndex;  // RenderFlags (index into render flags, TexFlags)
    u16 texunit;     // Index into the texture unit lookup table.
    u16 mode;        // See below.
    u16 texture_id;  // Index into Texture lookup table
    u16 texunit2;    // copy of texture unit value?
    u16 trans_id;    // Index into transparency lookup table.
    u16 tex_anim_id; // Index into uv animation lookup table.
  };

  struct M2ModelGeoset
  {
    u32 id;                      // mesh part id?
    u16 vstart;                  // first vertex, Starting vertex number.
    u16 vcount;                  // num vertices, Number of vertices.
    u16 istart;                  // first index, Starting triangle index (that's 3* the number of triangles drawn so far).
    u16 icount;                  // num indices, Number of triangle indices.
    u16 number_of_skinned_bones; // number of bone indices, Number of elements in the bone lookup table.
    u16 start_bones;             // ? always 1 to 4, Starting index in the bone lookup table.
    u16 root_bone;               // root bone?
    u16 number_of_bones;         //
    glm::vec3 bounding_box[2];
    float radius;
  };

  // Same as M2ModelGeoset but with an u32 as istart, to handle index > 65535 (present in HD models)
  struct M2ModelGeosetHD
  {
    M2ModelGeosetHD()
      : id(-1)
      , vstart(0)
      , vcount(0)
      , istart(0)
      , icount(0)
      , number_of_skinned_bones(0)
      , start_bones(0)
      , root_bone(0)
      , number_of_bones(0)
      , radius(0)
      , display(false)
      , bounding_box()
    {
      bounding_box[0] = glm::vec3(0, 0, 0);
      bounding_box[1] = glm::vec3(0, 0, 0);
    }

    explicit M2ModelGeosetHD(const M2ModelGeoset& geo)
      : id(geo.id & 0x7FFF)
      , vstart(geo.vstart)
      , vcount(geo.vcount)
      , istart(geo.istart)
      , icount(geo.icount)
      , number_of_skinned_bones(geo.number_of_skinned_bones)
      , start_bones(geo.start_bones)
      , root_bone(geo.root_bone)
      , number_of_bones(geo.number_of_bones)
      , radius(geo.radius)
      , display(false)
      , bounding_box()
    {
      bounding_box[0] = geo.bounding_box[0];
      bounding_box[1] = geo.bounding_box[1];
    }

    M2ModelGeosetHD(const M2ModelGeosetHD& geo)
      : id(geo.id)
      , vstart(geo.vstart)
      , vcount(geo.vcount)
      , istart(geo.istart)
      , icount(geo.icount)
      , number_of_skinned_bones(geo.number_of_skinned_bones)
      , start_bones(geo.start_bones)
      , root_bone(geo.root_bone)
      , number_of_bones(geo.number_of_bones)
      , radius(geo.radius)
      , display(geo.display)
      , bounding_box()
    {
      bounding_box[0] = geo.bounding_box[0];
      bounding_box[1] = geo.bounding_box[1];
    }

    u32 id;                      // mesh part id?
    u16 vstart;                  // first vertex, Starting vertex number.
    u16 vcount;                  // num vertices, Number of vertices.
    u32 istart;                  // first index, Starting triangle index (that's 3* the number of triangles drawn so far).
    u16 icount;                  // num indices, Number of triangle indices.
    u16 number_of_skinned_bones; // number of bone indices, Number of elements in the bone lookup table.
    u16 start_bones;             // ? always 1 to 4, Starting index in the bone lookup table.
    u16 root_bone;               // root bone?
    u16 number_of_bones;         //
    glm::vec3 bounding_box[2];
    float radius;
    bool display;
  };

#pragma pack(pop)

  class M2ModelView : public AssetWrapper<M2ModelView>
  {
    friend class M2Model;

  protected:
    void on_fully_loaded(const std::vector<char>& buffer) override;

  private:
#pragma pack(push, 1)

    struct Header
    {
      char id[4]; // Signature
      M2Field index;
      M2Field tris;
      M2Field props;
      M2Field sub;
      M2Field tex;
      i32 lod; // LOD bias?
    };

#pragma pack(pop)

    Header header;
    std::vector<u16> raw_indices;
    std::vector<M2ModelGeosetHD> raw_geosets;
    std::vector<M2ModelTexUnit> raw_tex_units;
  };

} // namespace loki

