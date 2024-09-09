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
#include "glm/vec3.hpp"

namespace loki {

#pragma pack(push, 1)

  struct M2Field
  {
    union
    {
      std::uint32_t number;
      std::uint32_t length;
    };

    std::uint32_t offset;
  };

  struct M2ModelRenderPass
  {
    std::int32_t tex;
    std::int32_t geoset;
  };

  struct M2ModelTexUnit
  {
    // probably the texture units
    // size always >=number of materials it seems
    std::uint16_t flags;       // Usually 16 for static textures, and 0 for animated textures.
    std::uint16_t shading;     // If set to 0x8000: shaders. Used in sky boxes to ditch the need for depth buffering. See below.
    std::uint16_t op;          // Material this texture is part of (index into mat)
    std::uint16_t op2;         // Always same as above?
    std::int16_t colorIndex;   // A Color out of the Colors-Block or -1 if none.
    std::uint16_t flagsIndex;  // RenderFlags (index into render flags, TexFlags)
    std::uint16_t texunit;     // Index into the texture unit lookup table.
    std::uint16_t mode;        // See below.
    std::uint16_t texture_id;  // Index into Texture lookup table
    std::uint16_t texunit2;    // copy of texture unit value?
    std::uint16_t trans_id;    // Index into transparency lookup table.
    std::uint16_t tex_anim_id; // Index into uv animation lookup table.
  };

  struct M2ModelGeoset
  {
    std::uint32_t id;                      // mesh part id?
    std::uint16_t vstart;                  // first vertex, Starting vertex number.
    std::uint16_t vcount;                  // num vertices, Number of vertices.
    std::uint16_t istart;                  // first index, Starting triangle index (that's 3* the number of triangles drawn so far).
    std::uint16_t icount;                  // num indices, Number of triangle indices.
    std::uint16_t number_of_skinned_bones; // number of bone indices, Number of elements in the bone lookup table.
    std::uint16_t start_bones;             // ? always 1 to 4, Starting index in the bone lookup table.
    std::uint16_t root_bone;               // root bone?
    std::uint16_t number_of_bones;         //
    glm::vec3 bounding_box[2];
    float radius;
  };

  // Same as M2ModelGeoset but with a std::uint32_t as istart, to handle index > 65535 (present in HD models)
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

    std::uint32_t id;                      // mesh part id?
    std::uint16_t vstart;                  // first vertex, Starting vertex number.
    std::uint16_t vcount;                  // num vertices, Number of vertices.
    std::uint32_t istart;                  // first index, Starting triangle index (that's 3* the number of triangles drawn so far).
    std::uint16_t icount;                  // num indices, Number of triangle indices.
    std::uint16_t number_of_skinned_bones; // number of bone indices, Number of elements in the bone lookup table.
    std::uint16_t start_bones;             // ? always 1 to 4, Starting index in the bone lookup table.
    std::uint16_t root_bone;               // root bone?
    std::uint16_t number_of_bones;         //
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
      std::int32_t lod; // LOD bias?
    };

#pragma pack(pop)

    Header header;
    std::vector<std::uint16_t> raw_indices;
    std::vector<M2ModelGeosetHD> raw_geosets;
    std::vector<M2ModelTexUnit> raw_tex_units;
  };

} // namespace loki

