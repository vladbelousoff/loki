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

#include <array>
#include <cstdint>

namespace loki::crypto {

  void get_random_bytes(std::uint8_t* buf, std::size_t len);

  template<typename Container>
  void get_random_bytes(Container& c)
  {
    get_random_bytes(std::data(c), std::size(c));
  }

  template<std::size_t S>
  std::array<std::uint8_t, S> get_random_bytes()
  {
    std::array<std::uint8_t, S> arr;
    get_random_bytes(arr);
    return arr;
  }

} // namespace loki::crypto
