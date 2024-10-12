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

#include "string_manager.h"

std::shared_mutex loki::StringManager::mutex;
std::string loki::StringManager::invalid_string;
std::unordered_map<loki::StringId, std::string> loki::StringManager::id_to_string;
std::unordered_map<std::string, loki::StringId> loki::StringManager::string_to_id;
std::size_t loki::StringManager::string_counter = 0;

loki::StringId::StringId()
  : id{ 0 }
{
}

loki::StringId::StringId(const std::string& string)
  : id{ StringManager::get_id_by_string(string).id }
{
}

bool
loki::StringId::operator==(const loki::StringId& other) const
{
  return id == other.id;
}

auto
loki::StringId::to_string() const -> const std::string&
{
  return StringManager::get_string_by_id(*this);
}

auto
loki::StringManager::get_string_by_id(loki::StringId id) -> const std::string&
{
  std::shared_lock<std::shared_mutex> lock(mutex);
  auto it = id_to_string.find(id);
  if (it != id_to_string.end()) {
    return it->second;
  }

  return invalid_string;
}

auto
loki::StringManager::get_id_by_string(const std::string& string) -> loki::StringId
{
  {
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = string_to_id.find(string);
    if (it != string_to_id.end()) {
      return it->second;
    }
  }

  StringId id = StringId{};

  {
    std::unique_lock<std::shared_mutex> lock(mutex);
    assert(string_counter < std::numeric_limits<std::size_t>::max());
    id.id = string_counter++;
    string_to_id.insert({ string, id });
    id_to_string.insert({ id, string });
  }

  return id;
}
