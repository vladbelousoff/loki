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

#include "auth_session.h"

#include "engine/config.h"
#include "world_session.h"

struct PaketAuthChallengeRequest
{
  loki::u8 command{};
  loki::u8 protocol_version{};
  loki::u16 packet_size{};
  std::array<loki::u8, 4> game_name{};
  loki::u8 major_version{};
  loki::u8 minor_version{};
  loki::u8 patch_version{};
  loki::u16 build{};
  std::array<loki::u8, 4> platform{};
  std::array<loki::u8, 4> os{};
  std::array<loki::u8, 4> country{};
  loki::u32 timezone{};
  loki::u32 ip_address{};
  std::vector<loki::u8> login{};
};

struct PaketAuthChallengeResponse
{
  loki::u8 command{};
  loki::u8 protocol_version{};
  loki::u8 status{};
  loki::SRP6::EphemeralKey B{};
  std::vector<loki::u8> g{};
  std::vector<loki::u8> N{};
  loki::SRP6::EphemeralKey s{};
  std::array<loki::u8, 16> crc_salt{};
  loki::u8 two_factor_enabled{};
};

struct PaketAuthLogonProofRequest
{
  loki::u8 command{};
  loki::SRP6::EphemeralKey A{};
  loki::SHA1::Digest client_M{};
  loki::SHA1::Digest crc_hash{};
  loki::u8 number_of_keys{};
  loki::u8 two_factor_enabled{};
};

struct PaketAuthLogonProofResponse
{
  loki::u8 command{};
  loki::u8 status{};
  loki::SHA1::Digest server_M{};
  loki::u32 account_flags{};
  loki::u32 hardware_survey_id{};
  loki::u16 unknown_flags{};
};

struct PacketAuthRealmListRequest
{
  loki::u8 command{};
  loki::u32 unknown{};
};

struct PacketAuthRealmListHead
{
  loki::u8 command{};
  loki::u16 packet_size{};
  loki::u32 unknown{};
  loki::u16 number_of_realms{};
};

loki::AuthSession::AuthSession(std::string_view host, loki::u16 port)
  : connector({ std::string(host), port })
  , thread()
  , running(true)
  , state(AuthSessionState::INVALID)
{
  if (connector) {
    thread = std::thread(
        [this](sockpp::tcp_socket sock) {
          socket = std::move(sock);
          handle_connection();
        },
        connector.clone());
  }
}

loki::AuthSession::~AuthSession()
{
  shutdown();
}

void
loki::AuthSession::stop()
{
  running = false;
}

void
loki::AuthSession::shutdown()
{
  stop();
  thread.join();
}

void
loki::AuthSession::handle_connection()
{
  using namespace std::chrono_literals;

  while (running) {
    switch (state) {
      case AuthSessionState::CHALLENGE:
        handle_challenge();
        break;
      case AuthSessionState::LOGON_PROOF:
        handle_logon_proof();
        break;
      case AuthSessionState::REALM_LIST:
        handle_realm_list();
        break;
      default:
        break;
    }

    if (state == AuthSessionState::REALM_LIST) {
      std::this_thread::sleep_for(1s);
    }
  }

  socket.shutdown();
}

void
loki::AuthSession::login(std::string_view username, std::string_view password)
{
  auto to_uppercase = [](std::string& string) {
    std::transform(string.begin(), string.end(), string.begin(), ::toupper);
  };

  username_uppercase = username;
  to_uppercase(username_uppercase);

  password_uppercase = password;
  to_uppercase(password_uppercase);

  state = AuthSessionState::CHALLENGE;
}

void
loki::AuthSession::handle_challenge()
{
  auto set_string = []<std::size_t N>(std::array<loki::u8, N>& data, const std::string& string) {
    DEBUG_ASSERT(string.size() <= N);
    std::reverse_copy(string.begin(), string.end(), data.begin());
  };

  {
    PaketAuthChallengeRequest pkt;
    pkt.command = 0;
    pkt.protocol_version = 8;
    pkt.packet_size = 30 + username_uppercase.length();
    set_string(pkt.game_name, config::game);
    pkt.major_version = config::major_version;
    pkt.minor_version = config::minor_version;
    pkt.patch_version = config::patch_version;
    pkt.build = config::build;
    set_string(pkt.platform, config::platform);
    set_string(pkt.os, config::os);
    set_string(pkt.country, config::locale);
    pkt.timezone = config::timezone;
    pkt.ip_address = 0;

    pkt.login.resize(username_uppercase.size());
    std::memcpy(pkt.login.data(), username_uppercase.data(), username_uppercase.size());

    buffer.save_buffer(pkt);
    buffer.send(socket);
    buffer.reset();
  }

  {
    PaketAuthChallengeResponse pkt;
    buffer.recv(socket);
    buffer.load_buffer(pkt);
    buffer.reset();

    loki::BigNum N = loki::BigNum::from_binary(pkt.N);
    loki::BigNum g = loki::BigNum::from_binary(pkt.g);
    srp6 = loki::SRP6(N, g);
    srp6->generate(pkt.s, pkt.B, username_uppercase, password_uppercase);
  }

  state = AuthSessionState::LOGON_PROOF;
}

void
loki::AuthSession::handle_logon_proof()
{
  {
    PaketAuthLogonProofRequest pkt;
    pkt.command = 0x1;
    pkt.A = srp6->get_A();
    pkt.client_M = srp6->get_client_M();
    pkt.crc_hash = srp6->get_crc_hash();
    pkt.number_of_keys = 0;
    pkt.two_factor_enabled = 0;

    buffer.save_buffer(pkt);
    buffer.send(socket);
    buffer.reset();
  }

  {
    PaketAuthLogonProofResponse pkt;
    buffer.recv(socket);
    buffer.load_buffer(pkt);
    buffer.reset();
  }

  state = AuthSessionState::REALM_LIST;
}

void
loki::AuthSession::handle_realm_list()
{
  spdlog::info("Checking ream list...");

  PacketAuthRealmListRequest pkt;
  pkt.command = 0x10; // Command: Realm List (0x10)
  pkt.unknown = 0;

  buffer.reset();
  buffer.save_buffer(pkt);
  buffer.send(socket);

  PacketAuthRealmListHead pkt_head;
  buffer.recv(socket);
  buffer.load_buffer(pkt_head);

  auto update_realms = [this, &pkt_head]() {
    std::unique_lock lock(realms_mutex);
    realms.clear();

    for (int i = 0; i < pkt_head.number_of_realms; ++i) {
      buffer.load_buffer(realms.emplace_back());
    }
  };

  update_realms();
}

auto
loki::AuthSession::get_realms() const -> std::vector<PacketAuthRealm>
{
  std::shared_lock lock(realms_mutex);
  return realms;
}

auto
loki::AuthSession::get_username() const -> const std::string&
{
  return username_uppercase;
}

auto
loki::AuthSession::get_session_key() const -> std::optional<SessionKey>
{
  if (srp6) {
    return srp6->get_session_key();
  }

  return std::nullopt;
}

auto
loki::AuthSession::connect_to_realm(loki::u8 realm_id) -> std::shared_ptr<WorldSession>
{
  stop();

  for (const auto& realm : get_realms()) {
    if (realm.realm_id == realm_id) {
      auto colon_pos = realm.server_socket.find(':');
      auto world_host = realm.server_socket.substr(0, colon_pos);
      auto world_port = std::stoul(realm.server_socket.substr(colon_pos + 1));

      return std::make_shared<WorldSession>(weak_from_this(), realm_id, world_host, static_cast<u16>(world_port));
    }
  }

  return nullptr;
}
