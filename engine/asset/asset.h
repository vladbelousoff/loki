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

#include "engine/datasource/mpq/mpq_file.h"
#include "engine/string_manager.h"
#include "engine/utils/strings.h"

#include <filesystem>
#include <mutex>

namespace loki {

  class Asset
  {
  public:
    Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;

    ~Asset() = default;

  public:
    auto is_loaded() const -> bool
    {
      std::shared_lock lock(load_mutex);
      return all_loaded;
    }

    void wait_load_all(const MPQFile& file);

  protected:
    virtual void load_all(const MPQFile& file) = 0;

  public:
    explicit Asset()
      : all_loaded(false)
    {
    }

  private:
    mutable std::shared_mutex load_mutex;
    bool all_loaded;
  };

} // namespace loki

