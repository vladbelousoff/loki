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

  enum class AssetLoadingState
  {
    NOT_LOADED,
    LOADING,
    LOADED_FULLY,
  };

  class Asset : public std::enable_shared_from_this<Asset>
  {
  public:
    explicit Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;

    virtual ~Asset() = default;

  public:
    auto get_loading_state() const -> AssetLoadingState
    {
      return loading_state;
    }

    auto is_loaded() const -> bool
    {
      return get_loading_state() == AssetLoadingState::LOADED_FULLY;
    }

    void request_load_full();

  protected:
    virtual void on_fully_loaded() = 0;

  protected:
    StringID asset_path;
    std::vector<char> buffer;

  protected:
    explicit Asset()
      : loading_state(AssetLoadingState::NOT_LOADED)
    {
    }

  private:
    void wait_load_full(const MPQFile& file);

  private:
    std::atomic<AssetLoadingState> loading_state;
  };

  template<typename AssetType>
  class AssetWrapper : public Asset
  {
  public:
    static auto create(const std::filesystem::path& path) -> std::shared_ptr<AssetType>
    {
      auto result = std::make_shared<AssetType>();
      result->asset_path = StringID(to_uppercase(path.string()));
      return result;
    }
  };

} // namespace loki

