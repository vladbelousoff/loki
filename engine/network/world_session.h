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

#include "auth_crypt.h"
#include "auth_session.h"
#include "engine/utils/byte_buffer.h"
#include "engine/utils/types.h"
#include "opcodes.h"
#include "sockpp/tcp_connector.h"

#include <shared_mutex>
#include <string_view>
#include <thread>

namespace loki {

  class WorldSession
  {
  public:
    struct ServerPacketHeader
    {
      u32 size{};
      std::array<u8, 5> header{};
    };

#pragma pack(push, 1)

    struct ClientPacketHeader
    {
      u16 size{};
      u32 command{};
    };

#pragma pack(pop)

  public:
    explicit WorldSession(const std::weak_ptr<AuthSession>& auth_session, u8 realm_id, std::string_view host, u16 port);
    ~WorldSession();

    void stop();

  private:
    void handle_connection();
    void read_incoming_packets();
    void read_next_packet();
    void process_command(u16 command);
    void handle_auth_challenge();
    void handle_auth_response();

  private:
    u8 realm_id;
    std::weak_ptr<AuthSession> auth_session;
    sockpp::tcp_connector connector;
    sockpp::tcp_socket socket;
    std::thread thread;
    AuthCrypt auth_crypt;
    std::atomic_bool running;
    ByteBuffer buffer;
    std::queue<ByteBuffer> outgoing_messages;
    bool encrypted = false;
    bool chars = false;
  };

} // namespace loki
