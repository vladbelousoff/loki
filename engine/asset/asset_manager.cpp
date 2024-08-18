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

#include "asset_manager.h"

void
loki::AssetManager::insert_asset(loki::StringID name, const loki::AssetManager::AssetSharedPtr& asset)
{
  std::lock_guard lock(mutex);
  assets.insert({ name, asset });
  fresh_assets.push(asset);
}

void
loki::AssetManager::process_fresh_assets(const std::function<void(AssetWeakPtr&)>& callback)
{
  std::shared_lock lock(mutex);
  while (!fresh_assets.empty()) {
    callback(fresh_assets.front());
    fresh_assets.pop();
  }
}
