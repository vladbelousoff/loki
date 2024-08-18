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

#include "engine/asset/asset.h"
#include "engine/datasource/mpq/mpq_file_manager.h"

namespace loki {

  class AssetManager
  {
    using AssetSharedPtr = std::shared_ptr<Asset>;
    using AssetWeakPtr = std::weak_ptr<Asset>;

  public:
    explicit AssetManager(const std::weak_ptr<MPQFileManager>& file_manager)
      : file_manager(file_manager)
    {
    }

    virtual ~AssetManager() = default;

  public:
    template<typename AssetType>
    void request_load_asset_full(const std::filesystem::path& path)
    {
      file_manager.lock()->request_file(path, [this](MPQFile& file) {
        auto asset = std::make_shared<AssetType>();
        asset->wait_load_all(file);
        insert_asset(file.get_name(), asset);
      });
    }

    void process_fresh_assets(const std::function<void(AssetWeakPtr&)>& callback);

  private:
    void insert_asset(StringID name, const AssetSharedPtr& asset);

  private:
    std::weak_ptr<MPQFileManager> file_manager;
    std::unordered_map<StringID, AssetSharedPtr> assets;
    mutable std::shared_mutex mutex;
    std::queue<AssetWeakPtr> fresh_assets;
  };

} // namespace loki
