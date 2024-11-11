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

#include "mpq_archive.h"

#include "libassert/assert.hpp"
#include "spdlog/spdlog.h"

loki::MPQArchive::MPQArchive(const std::filesystem::path& path)
{
  if (!SFileOpenArchive(path.string().c_str(), 0, MPQ_OPEN_NO_LISTFILE | MPQ_OPEN_READ_ONLY, &handle)) {
    spdlog::error("Error opening MPQ archive: {}", path.string());
  }
}

bool
loki::MPQArchive::patch(const std::filesystem::path& path, const std::string& prefix)
{
  DEBUG_ASSERT(is_valid(), "Cannot apply a patch to an invalid MPQ archive", this);
  return SFileOpenPatchArchive(handle, path.string().c_str(), prefix.c_str(), 0);
}
