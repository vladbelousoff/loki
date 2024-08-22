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
    void on_fully_loaded() override;

  private:
#pragma pack(push, 1)

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
      char id[4]; // Signature
      Record index;
      Record tris;
      Record props;
      Record sub;
      Record tex;
      i32 lod; // LOD bias?
    };

#pragma pack(pop)

    Header* header = nullptr;
    std::vector<u16> raw_indices;
    std::vector<M2ModelGeosetHD> raw_geosets;
  };

} // namespace loki

