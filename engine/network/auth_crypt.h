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

#include "auth_defines.h"
#include "engine/crypto/arc_4.h"

namespace loki {

  class AuthCrypt
  {
  public:
    void init(const SessionKey& session_key);
    void decrypt_recv(u8* data, size_t len);
    void encrypt_send(u8* data, size_t len);

    bool is_inited() const
    {
      return initialized;
    }

  private:
    ARC4 decrypt{};
    ARC4 encrypt{};
    bool initialized = false;
  };

} // namespace loki