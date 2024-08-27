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

#include "crypto_random.h"
#include "libassert/assert.hpp"

#include <openssl/rand.h>

void
loki::crypto::get_random_bytes(loki::u8* buf, size_t len)
{
  i32 result = RAND_bytes(buf, static_cast<i32>(len));
  DEBUG_ASSERT(result == 1, "Not enough randomness in OpenSSL's entropy pool. What in the world are you running on?");
}
