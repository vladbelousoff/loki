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

#include "mpq_file.h"
#include "libassert/assert.hpp"

auto
loki::MPQFile::is_valid() const -> bool
{
  return handle != HANDLE{};
}

auto
loki::MPQFile::get_name() const -> loki::StringId
{
  return name;
}

auto
loki::MPQFile::read(void* data, unsigned long size) const -> unsigned long
{
  ASSERT(is_valid());
  DWORD bytes_read = 0;
  SFileReadFile(handle, data, size, &bytes_read, nullptr);
  return bytes_read;
}

auto
loki::MPQFile::read(unsigned long size) const -> std::vector<char>
{
  std::vector<char> data(size);
  unsigned long bytes_read = read(data.data(), size);
  ASSERT(bytes_read == size);
  return data;
}

auto
loki::MPQFile::seek(long position, long method) const -> unsigned long
{
  ASSERT(is_valid());
  return SFileSetFilePointer(handle, position, nullptr, method);
}

auto
loki::MPQFile::get_size() const -> unsigned long
{
  ASSERT(is_valid());
  return SFileGetFileSize(handle, nullptr);
}

auto
loki::MPQFile::read_all(std::vector<char>& buffer) const -> unsigned long
{
  auto size = get_size();
  buffer.resize(size);
  return read(buffer.data(), size);
}
