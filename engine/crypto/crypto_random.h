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

#include "engine/utils/types.h"

namespace loki::crypto {

  void get_random_bytes(u8* buf, size_t len);

  template<typename Container>
  void get_random_bytes(Container& c)
  {
    get_random_bytes(std::data(c), std::size(c));
  }

  template<size_t S>
  std::array<u8, S> get_random_bytes()
  {
    std::array<u8, S> arr;
    get_random_bytes(arr);
    return arr;
  }

} // namespace loki::crypto
