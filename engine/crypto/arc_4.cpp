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

#include "arc_4.h"
#include "libassert/assert.hpp"

loki::ARC4::ARC4()
  : context(EVP_CIPHER_CTX_new())
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  cipher = EVP_CIPHER_fetch(nullptr, SN_rc4, nullptr);
#else
  const EVP_CIPHER* cipher = EVP_rc4();
  EVP_CIPHER_CTX_init(context);
#endif

  int result = EVP_EncryptInit_ex(context, cipher, nullptr, nullptr, nullptr);
  ASSERT(result == 1);
}

loki::ARC4::~ARC4()
{
  EVP_CIPHER_CTX_free(context);
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  EVP_CIPHER_free(cipher);
#endif
}

void
loki::ARC4::init(const std::uint8_t* seed, std::size_t len)
{
  int result1 = EVP_CIPHER_CTX_set_key_length(context, (int)len);
  ASSERT(result1 == 1);
  int result2 = EVP_EncryptInit_ex(context, nullptr, nullptr, seed, nullptr);
  ASSERT(result2 == 1);
}

void
loki::ARC4::update_data(std::uint8_t* data, std::size_t len)
{
  int outlen = 0;
  int result1 = EVP_EncryptUpdate(context, data, &outlen, data, (int)len);
  ASSERT(result1 == 1);
  int result2 = EVP_EncryptFinal_ex(context, data, &outlen);
  ASSERT(result2 == 1);
}
