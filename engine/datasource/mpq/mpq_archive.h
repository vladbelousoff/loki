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

#include <filesystem>

#ifdef NOMINMAX
#undef NOMINMAX
#endif

#include "StormLib.h"

namespace loki {

  class MPQArchive
  {
  public:
    explicit MPQArchive() = default;
    explicit MPQArchive(const std::filesystem::path& path);

  public:
    auto is_valid() const -> bool
    {
      return handle != HANDLE{};
    }

    auto get_handle() const -> HANDLE
    {
      return handle;
    }

    auto patch(const std::filesystem::path& path, const std::string& prefix = "") -> bool;

  private:
    HANDLE handle{};
  };

} // namespace loki

