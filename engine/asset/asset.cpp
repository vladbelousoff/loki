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

#include "asset.h"
#include "engine/datasource/mpq/mpq_file_manager.h"

void
loki::Asset::wait_load_full(const MPQFile& file)
{
  file.read_all(buffer);
  on_fully_loaded(); // TODO: move this to the main/game thread
  loading_state = AssetLoadingState::LOADED_FULLY;

  spdlog::info("Loaded file '{}'", asset_path.to_string());
}

void
loki::Asset::request_load_full()
{
  if (get_loading_state() != AssetLoadingState::NOT_LOADED) {
    return;
  }

  loading_state = AssetLoadingState::LOADING;

  auto& file_manager = MPQFileManager::get_ref();
  auto self = weak_from_this();

  file_manager.request_file(asset_path.to_string(), [self](MPQFile& file) {
    if (auto self_shared = self.lock()) {
      self_shared->wait_load_full(file);
    } else {
      spdlog::warn("Asset '{}' is already expired", file.get_name().to_string());
    }
  });

  spdlog::info("Loading file '{}'", asset_path.to_string());
}
