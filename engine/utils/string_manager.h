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

#include <spdlog/spdlog.h>

#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace loki {

  class StringManager;

  class StringId
  {
    friend struct std::hash<StringId>;
    friend class StringManager;

  public:
    explicit StringId();
    explicit StringId(const std::string& string);

    auto to_string() const -> const std::string&;
    bool operator==(const StringId& other) const;

  private:
    std::size_t id;
  };

  class StringManager
  {
    friend class StringId;

  public:
    static std::string invalid_string;

  private:
    static auto get_string_by_id(StringId id) -> const std::string&;
    static auto get_id_by_string(const std::string& string) -> StringId;

    static std::shared_mutex mutex;
    static std::size_t string_counter;
    static std::unordered_map<StringId, std::string> id_to_string;
    static std::unordered_map<std::string, StringId> string_to_id;
  };

} // namespace loki

template<>
struct std::hash<loki::StringId>
{
  std::size_t operator()(const loki::StringId& string) const
  {
    return std::hash<std::size_t>{}(string.id);
  }
};
