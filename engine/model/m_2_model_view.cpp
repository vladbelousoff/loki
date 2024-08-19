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

#include "m_2_model_view.h"
#include "libassert/assert.hpp"

void
loki::M2ModelView::on_fully_loaded()
{
  data = reinterpret_cast<Data*>(buffer.data());
  ASSERT(data->id[0] == 'S' && data->id[1] == 'K' && data->id[2] == 'I' && data->id[3] == 'N');

  const u16* index_lookup = reinterpret_cast<u16*>(&buffer[data->index.offset]);
  const u16* triangles = reinterpret_cast<u16*>(&buffer[data->tris.offset]);

  raw_indices.resize(data->tris.number);
  for (u32 i = 0; i < data->tris.number; ++i) {
    raw_indices[i] = index_lookup[triangles[i]];
  }

  spdlog::info("Loaded indices: {}", raw_indices.size());
}
