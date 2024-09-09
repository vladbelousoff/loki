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

#include "engine/crypto/srp_6.h"
#include "engine/utils/byte_buffer.h"
#include "sockpp/tcp_connector.h"

#include <memory>
#include <queue>
#include <shared_mutex>
#include <thread>

namespace loki {

  class WorldSession;

  struct PacketAuthRealm
  {
    std::uint8_t type{};
    std::uint8_t locked{};
    std::uint8_t flags{};
    std::string name;
    std::string server_socket;
    std::uint32_t population_level{};
    std::uint8_t number_of_characters{};
    std::uint8_t category{};
    std::uint8_t realm_id{};
  };

  enum class AuthSessionState : std::int8_t
  {
    INVALID = -1,
    CHALLENGE = 0,
    LOGON_PROOF = 1,
    REALM_LIST = 2,
  };

  class AuthSession : public std::enable_shared_from_this<AuthSession>
  {
  public:
    explicit AuthSession(std::string_view host, std::uint16_t port);
    ~AuthSession();

    AuthSession(const AuthSession&) = delete;
    AuthSession& operator=(const AuthSession&) = delete;

  public:
    void login(std::string_view username, std::string_view password);
    auto connect_to_realm(std::uint8_t realm_id) -> std::shared_ptr<WorldSession>;
    void stop();
    void shutdown();
    auto get_realms() const -> std::vector<PacketAuthRealm>;
    auto get_username() const -> const std::string&;
    auto get_session_key() const -> std::optional<SessionKey>;

  private:
    void handle_connection();
    void handle_challenge();
    void handle_logon_proof();
    void handle_realm_list();

  private:
    std::string username_uppercase;
    std::string password_uppercase;
    sockpp::tcp_connector connector;
    sockpp::tcp_socket socket;
    std::thread thread;
    std::atomic_bool running;
    std::atomic<AuthSessionState> state;
    std::optional<loki::SRP6> srp6;
    ByteBuffer buffer;
    mutable std::shared_mutex realms_mutex;
    std::vector<PacketAuthRealm> realms;
  };

} // namespace loki
