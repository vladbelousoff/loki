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

#include "engine/crypto/crypto_hash.h"
#include "engine/network/auth_defines.h"
#include "engine/utils/big_num.h"
#include "engine/utils/types.h"

#include <array>
#include <string_view>
#include <vector>

namespace loki {

  using namespace crypto;

  class SRP6
  {
  public:
    static constexpr std::size_t SALT_LENGTH = 32;
    using Salt = std::array<u8, SALT_LENGTH>;
    static constexpr std::size_t EPHEMERAL_KEY_LENGTH = 32;
    using EphemeralKey = std::array<u8, EPHEMERAL_KEY_LENGTH>;

  public:
    explicit SRP6(const BigNum& N, const BigNum& g);
    ~SRP6() = default;

  public:
    void generate(const Salt& salt, const EphemeralKey& B, std::string_view I, std::string_view P);

    const EphemeralKey& get_A() const
    {
      return A;
    }

    const SHA1::Digest& get_client_M() const
    {
      return client_M;
    }

    const SHA1::Digest& get_crc_hash() const
    {
      return crc_hash;
    }

    const SessionKey& get_session_key() const
    {
      return K;
    }

  private:
    static SessionKey SHA1_interleave(const EphemeralKey& S);

  private:
    BigNum N;
    BigNum g;
    BigNum a;
    BigNum k;
    EphemeralKey A;
    SHA1::Digest client_M{};
    SHA1::Digest crc_hash{};
    SessionKey K{};
  };

} // namespace loki

