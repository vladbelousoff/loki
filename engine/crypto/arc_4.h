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
#include "openssl/evp.h"

namespace loki {

  class ARC4
  {
  public:
    ARC4();
    ~ARC4();

  public:
    void init(const u8* seed, std::size_t len);
    void update_data(u8* data, std::size_t len);

    template<typename Container>
    void init(const Container& c)
    {
      init(std::data(c), std::size(c));
    }

    template<typename Container>
    void update_data(Container& c)
    {
      update_data(std::data(c), std::size(c));
    }

  private:
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    EVP_CIPHER* cipher{};
#endif
    EVP_CIPHER_CTX* context{};
  };

} // namespace loki
