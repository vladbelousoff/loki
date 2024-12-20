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

#include "engine/utils/string_manager.h"
#include "mpq_archive.h"

namespace loki {

  class MPQFile
  {
  public:
    explicit MPQFile(const std::string& name, HANDLE handle)
      : name(name)
      , handle(handle)
    {
    }

  public:
    auto is_valid() const -> bool;
    auto get_name() const -> StringId;
    auto get_size() const -> unsigned long;

    auto read(void* data, unsigned long size) const -> unsigned long;
    auto read(unsigned long size) const -> std::vector<char>;
    auto read_all(std::vector<char>& buffer) const -> unsigned long;

    auto seek(long position, long method) const -> unsigned long;

  private:
    StringId name;
    HANDLE handle;
  };

} // namespace loki

