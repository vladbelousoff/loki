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

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"

void
loki::M2Model::on_fully_loaded()
{
  header = reinterpret_cast<Header*>(buffer.data());

  model_name.resize(header->name.length);
  memcpy(model_name.data(), &buffer[header->name.offset], model_name.size());

  spdlog::info("Loaded model name: {}", model_name.data());

  raw_vertices.resize(header->vertices.number);
  memcpy(raw_vertices.data(), &buffer[header->vertices.offset], raw_vertices.size() * sizeof(ModelVertex));

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

void
loki::M2Model::draw() const
{
  if (!is_loaded()) {
    return;
  }

  if (model_views.empty() || !model_views[0] || !model_views[0]->is_loaded()) {
    return;
  }

  const auto& model_view = model_views[0];
  if (model_view->raw_geosets.empty()) {
    return;
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbuf, nbuf, tbuf;

  glGenBuffers(1, &vbuf);
  glGenBuffers(1, &tbuf);
  glGenBuffers(1, &nbuf);

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texcoords;

  for (auto& vertex : raw_vertices) {
    vertices.push_back(vertex.pos);
    normals.push_back(glm::normalize(vertex.normal));
    texcoords.push_back(vertex.texcoords);
  }

  // Upload positions
  glBindBuffer(GL_ARRAY_BUFFER, vbuf);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
  glEnableVertexAttribArray(0);

  // Upload normals
  glBindBuffer(GL_ARRAY_BUFFER, nbuf);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
  glEnableVertexAttribArray(1);

  // Upload texture coordinates
  glBindBuffer(GL_ARRAY_BUFFER, tbuf);
  glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(glm::vec2), texcoords.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // glDrawArrays(GL_POINTS, 0, vertices.size());

  for (const auto& geoset : model_view->raw_geosets) {
    if (!geoset.display) {
      continue;
    }

    glDrawRangeElements(GL_TRIANGLES, geoset.vstart, geoset.vstart + geoset.vcount, geoset.icount, GL_UNSIGNED_SHORT, &model_view->raw_indices[geoset.istart]);
  }

  glBindVertexArray(0);

  glDeleteBuffers(1, &nbuf);
  glDeleteBuffers(1, &vbuf);
  glDeleteBuffers(1, &tbuf);

  glDeleteVertexArrays(1, &vao);
}
