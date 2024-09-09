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
loki::M2ModelView::on_fully_loaded(const std::vector<char>& buffer)
{
  memcpy(&header, buffer.data(), sizeof(header));
  ASSERT(header.id[0] == 'S' && header.id[1] == 'K' && header.id[2] == 'I' && header.id[3] == 'N');

  const auto* index_lookup = reinterpret_cast<const std::uint16_t*>(&buffer[header.index.offset]);
  const auto* triangles = reinterpret_cast<const std::uint16_t*>(&buffer[header.tris.offset]);

  raw_indices.resize(header.tris.number);
  for (std::uint32_t i = 0; i < header.tris.number; ++i) {
    raw_indices[i] = index_lookup[triangles[i]];
  }

  spdlog::info("Loaded indices: {}", raw_indices.size());

  // Render ops
  auto* ops = reinterpret_cast<const M2ModelGeoset*>(&buffer[header.sub.offset]);

  std::uint32_t istart = 0;
  for (std::uint32_t i = 0; i < header.sub.number; ++i) {
    auto& hd_geo = raw_geosets.emplace_back(ops[i]);
    hd_geo.istart = istart;
    istart += hd_geo.icount;
    hd_geo.display = hd_geo.id == 0;
  }

  spdlog::info("Loaded geo sets: {}", raw_geosets.size());

  auto* tex_units = &buffer[header.tex.offset];
  raw_tex_units.resize(header.tex.number);
  memcpy(raw_tex_units.data(), tex_units, header.tex.number * sizeof(M2ModelTexUnit));

  spdlog::info("Loaded tex units: {}", raw_tex_units.size());
}
