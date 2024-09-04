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

#include "auth_crypt.h"

#include "engine/crypto/crypto_hmac.h"

void
loki::AuthCrypt::init(const loki::SessionKey& session_key)
{
  u8 server_encryption_key[] = { 0xCC, 0x98, 0xAE, 0x04, 0xE8, 0x97, 0xEA, 0xCA, 0x12, 0xDD, 0xC0, 0x93, 0x42, 0x91, 0x53, 0x57 };
  decrypt.init(crypto::HMAC_SHA1::get_digest_of(server_encryption_key, session_key));
  u8 server_decryption_key[] = { 0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE, 0x2F, 0x43, 0x67, 0xCE };
  encrypt.init(crypto::HMAC_SHA1::get_digest_of(server_decryption_key, session_key));

  // Drop first 1024 bytes, as WoW uses ARC4-drop1024.
  std::array<u8, 1024> sync_buf{};
  decrypt.update_data(sync_buf);
  encrypt.update_data(sync_buf);

  initialized = true;
}

void
loki::AuthCrypt::decrypt_recv(loki::u8* data, std::size_t len)
{
  ASSERT(initialized);
  decrypt.update_data(data, len);
}

void
loki::AuthCrypt::encrypt_send(loki::u8* data, std::size_t len)
{
  ASSERT(initialized);
  encrypt.update_data(data, len);
}
