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
      std::uint32_t size{};
      std::array<std::uint8_t, 5> header{};
    };

#pragma pack(push, 1)

    struct ClientPacketHeader
    {
      std::uint16_t size{};
      std::uint32_t command{};
    };

#pragma pack(pop)

  public:
    explicit WorldSession(const std::weak_ptr<AuthSession>& auth_session, std::uint8_t realm_id, std::string_view host, std::uint16_t port);
    ~WorldSession();

    void stop();

  private:
    void handle_connection();
    void read_incoming_packets();
    void read_next_packet();
    void process_command(std::uint16_t command);
    void handle_auth_challenge();
    void handle_auth_response();

  private:
    std::uint8_t realm_id;
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
