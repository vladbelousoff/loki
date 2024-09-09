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

#include "world_session.h"

#include "engine/config.h"
#include "engine/crypto/crypto_hash.h"
#include "engine/crypto/crypto_random.h"
#include "opcodes.h"

loki::WorldSession::WorldSession(const std::weak_ptr<AuthSession>& auth_session, std::uint8_t realm_id, std::string_view host, std::uint16_t port)
  : auth_session(auth_session)
  , realm_id(realm_id)
  , connector({ std::string(host), port })
  , thread()
  , auth_crypt()
  , running(true)
{
  using namespace std::chrono_literals;

  if (connector) {
    socket.read_timeout(0.2s);
    thread = std::thread(
        [this](sockpp::tcp_socket sock) {
          socket = std::move(sock);
          handle_connection();
        },
        connector.clone());
  }
}

loki::WorldSession::~WorldSession()
{
  stop();
  thread.join();
}

void
loki::WorldSession::stop()
{
  running = false;
}

void
loki::WorldSession::handle_connection()
{
  spdlog::info("Connected to {}", socket.peer_address().to_string());

  const auto& session_key = auth_session.lock()->get_session_key();
  auth_crypt.init(*session_key);

  while (running) {
    buffer.reset();
    read_incoming_packets();
  }

  socket.shutdown();
}

void
loki::WorldSession::read_incoming_packets()
{
  ssize_t packet_size = buffer.recv(socket);
  if (packet_size <= 0) {
    return;
  }

  if (encrypted) {
    while (buffer.can_read()) {
      read_next_packet();
    }

    // CMSG_CHAR_ENUM
    if (!chars) {
      ClientPacketHeader client_header{};
      client_header.size = htons(4);
      client_header.command = CMSG_CHAR_ENUM;

      auth_crypt.encrypt_send((std::uint8_t*)&client_header, sizeof(client_header));

      buffer.reset();
      buffer.save_buffer(client_header);
      buffer.send(socket);

      chars = true;
    }
  } else {
    buffer.read<std::uint16_t>(); // header, unused data (probably size?)
    auto command = buffer.read<std::uint16_t>();
    process_command(command);
  }
}

void
loki::WorldSession::read_next_packet()
{
  auto cur_pos = buffer.get_r_pos();
  std::array<std::uint8_t, 4> header{};

  buffer.load_buffer(header);
  auth_crypt.decrypt_recv(header.data(), 4 /* small size packet, there's also 5-size packets, but it's not our case so far */);

  auto size = htons(*reinterpret_cast<std::uint16_t*>(&header[0]));
  std::uint16_t command = *reinterpret_cast<std::uint16_t*>(&header[2]);
  spdlog::info("Packet size: {}", size);

  process_command(command);
  buffer.set_r_pos(cur_pos + size + 2);
}

void
loki::WorldSession::process_command(std::uint16_t command)
{
  switch (command) {
    case SMSG_AUTH_CHALLENGE:
      handle_auth_challenge();
      break;
    case SMSG_AUTH_RESPONSE:
      handle_auth_response();
      break;
    default:
      spdlog::info("Unknown command: {:x}", command);
      break;
  }
}

void
loki::WorldSession::handle_auth_challenge()
{
  spdlog::info("Receiving SMSG_AUTH_CHALLENGE");

  auto one = buffer.read<std::uint32_t>();
  DEBUG_ASSERT(one == 0x1);

  std::array<std::uint8_t, 4> auth_seed{};
  buffer.read(auth_seed);

  std::array<std::uint8_t, 4> t{};

  auto local_challenge = crypto::get_random_bytes<4>();
  auto session_key = auth_session.lock()->get_session_key();
  auto& username = auth_session.lock()->get_username();

  struct
  {
    std::uint32_t build = config::build;
    std::uint32_t login_server_id = 0;
    std::string account{};
    std::uint32_t local_server_type = 0;
    std::array<std::uint8_t, 4> local_challenge{};
    std::uint32_t region_id = 0;
    std::uint32_t battle_group_id = 0;
    std::uint32_t realm_id = 0;
    std::uint64_t dos_response = 0;
    SHA1::Digest digest{};
    std::vector<std::uint8_t> addon_info{};
  } auth_info;

  auth_info.realm_id = realm_id;
  auth_info.local_challenge = local_challenge;
  auth_info.digest = SHA1::get_digest_of(username, t, local_challenge, auth_seed, *session_key);
  auth_info.account = username;

  ClientPacketHeader client_header{};
  client_header.size = htons(username.length() + auth_info.addon_info.size() + 62);
  client_header.command = CMSG_AUTH_SESSION;

  buffer.reset();
  buffer.save_buffer(client_header);
  buffer.save_buffer(auth_info);
  buffer.send(socket);

  spdlog::info("Sending CMSG_AUTH_SESSION");

  encrypted = true;
}

void
loki::WorldSession::handle_auth_response()
{
  spdlog::info("Receiving SMSG_AUTH_RESPONSE");

  auto status = buffer.read<std::uint8_t>();
  ASSERT(status == 12, "You're not the first in the queue"); // AUTH_OK code
}
