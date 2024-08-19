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

#include "m_2_model.h"

void
loki::M2Model::on_fully_loaded()
{
  header = reinterpret_cast<Header*>(buffer.data());

  model_name.resize(header->name.length);
  memcpy(model_name.data(), &buffer[header->name.offset], model_name.size());

  spdlog::info("Loaded model name: {}", model_name.data());

  raw_vertices.resize(header->vertices.number);
  memcpy(raw_vertices.data(), &buffer[header->vertices.offset], raw_vertices.size());

  spdlog::info("Loaded vertices: {}", header->vertices.number);
  spdlog::info("Number of views: {}", header->number_of_views);

  for (u32 i = 0; i < header->number_of_views; ++i) {
    auto path = std::filesystem::path(asset_path.to_string());
    path.replace_extension("");
    auto model_view_path = std::format("{}0{}.skin", path.string(), i);
    auto model_view = M2ModelView::create(model_view_path);
    model_view->request_load_full();
    model_views.push_back(std::move(model_view));
  }
}
