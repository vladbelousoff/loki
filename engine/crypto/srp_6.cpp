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

#include "srp_6.h"

#include "engine/crypto/crypto_random.h"
#include "libassert/assert.hpp"

#include <algorithm>

loki::SRP6::SRP6(const loki::BigNum& N, const loki::BigNum& g)
  : N(N)
  , g(g)
  , a(BigNum::from_random(19))
  , A(g.mod_exp(a, N).to_byte_array<EPHEMERAL_KEY_LENGTH>())
  , k()
{
  // Derived key
  k.set_dword(3);
}

loki::SessionKey
loki::SRP6::SHA1_interleave(const loki::SRP6::EphemeralKey& S)
{
  // split S into two buffers
  std::array<u8, EPHEMERAL_KEY_LENGTH / 2> buf0{}, buf1{};
  for (std::size_t i = 0; i < EPHEMERAL_KEY_LENGTH / 2; ++i) {
    buf0[i] = S[2 * i + 0];
    buf1[i] = S[2 * i + 1];
  }

  // find position of first nonzero byte
  std::size_t p = 0;
  while (p < EPHEMERAL_KEY_LENGTH && !S[p]) {
    ++p;
  }
  if (p & 1) {
    ++p; // skip one extra byte if p is odd
  }
  p /= 2; // offset into buffers

  // hash each of the halves, starting at the first nonzero byte
  const SHA1::Digest hash0 = SHA1::get_digest_of(buf0.data() + p, EPHEMERAL_KEY_LENGTH / 2 - p);
  const SHA1::Digest hash1 = SHA1::get_digest_of(buf1.data() + p, EPHEMERAL_KEY_LENGTH / 2 - p);

  // stick the two hashes back together
  SessionKey K;
  for (std::size_t i = 0; i < SHA1::DIGEST_LENGTH; ++i) {
    K[2 * i + 0] = hash0[i];
    K[2 * i + 1] = hash1[i];
  }

  return K;
}

void
loki::SRP6::generate(const Salt& salt, const EphemeralKey& B, std::string_view I, std::string_view P)
{
  auto x = BigNum::from_binary(SHA1::get_digest_of(salt, SHA1::get_digest_of(I, ":", P)));
  auto v = g.mod_exp(x, N);
  auto u = BigNum::from_binary(SHA1::get_digest_of(A, B));

  auto kgx = k * v;
  auto aux = a + u * x;
  auto S = (BigNum::from_binary(B) - kgx).mod_exp(aux, N);

  auto S_bin = S.to_byte_array<EPHEMERAL_KEY_LENGTH>();
  K = SHA1_interleave(S_bin);

  auto I_hash = SHA1::get_digest_of(I);
  auto N_hash = SHA1::get_digest_of(N.to_byte_vector());
  auto g_hash = SHA1::get_digest_of(g.to_byte_vector());

  SHA1::Digest Ng_hash;
  std::transform(N_hash.begin(), N_hash.end(), g_hash.begin(), Ng_hash.begin(), std::bit_xor<>());

  client_M = SHA1::get_digest_of(Ng_hash, I_hash, salt, A, B, K);
  crc_hash = SHA1::get_digest_of(A, client_M, K);
}

