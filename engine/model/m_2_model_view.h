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

namespace loki {

  class M2ModelView : public AssetWrapper<M2ModelView>
  {
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

    struct Data
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

    Data* data{};
    std::vector<u32> raw_indices;
  };

} // namespace loki

