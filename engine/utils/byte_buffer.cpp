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

#include "byte_buffer.h"

loki::ByteBuffer::ByteBuffer()
{
  buffer.reserve(DEFAULT_SIZE);
}

void
loki::ByteBuffer::append(const std::vector<std::uint8_t>& value)
{
  append<std::uint8_t>(value.size());
  buffer.insert(buffer.end(), value.begin(), value.end());
}

void
loki::ByteBuffer::append(std::string_view value)
{
  buffer.insert(buffer.end(), value.begin(), value.end());
  buffer.push_back(0);
}

void
loki::ByteBuffer::send(sockpp::tcp_socket& conn) const
{
  conn.write(buffer.data(), buffer.size());
}

ssize_t
loki::ByteBuffer::recv(sockpp::tcp_socket& conn)
{
  buffer.resize(DEFAULT_SIZE);

  ssize_t n = conn.read(buffer.data(), DEFAULT_SIZE);

  if (n >= 0) {
    buffer.resize(n);
  }

  if (n < 0) {
    spdlog::error("Error: {}!", n);
  }

  return n;
}

void
loki::ByteBuffer::reset()
{
  r_pos = 0;
  buffer.clear();
}
