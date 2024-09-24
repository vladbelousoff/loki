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

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "glm/gtc/type_ptr.hpp"
#include "libassert/assert.hpp"

void
loki::M2Model::on_fully_loaded(const std::vector<char>& buffer)
{
  auto* header = reinterpret_cast<const Header*>(buffer.data());

  model_name.resize(header->name.length);
  memcpy(model_name.data(), &buffer[header->name.offset], model_name.size());

  spdlog::info("Loaded model name: {}", model_name.data());

  raw_vertices.resize(header->vertices.number);
  memcpy(raw_vertices.data(), &buffer[header->vertices.offset], raw_vertices.size() * sizeof(ModelVertex));

  spdlog::info("Loaded vertices: {}", header->vertices.number);
  spdlog::info("Number of views: {}", header->number_of_views);

  for (std::uint32_t i = 0; i < header->number_of_views; ++i) {
    auto path = std::filesystem::path(asset_path.to_string());
    path.replace_extension("");
    auto model_view_path = fmt::format("{}{:02}.skin", path.string(), i);
    auto model_view = M2ModelView::create(model_view_path);
    model_view->request_load_full();
    model_views.push_back(std::move(model_view));
  }

  spdlog::info("Number of textures: {}", header->textures.number);

  auto* texture_def = reinterpret_cast<const M2ModelTextureDef*>(&buffer[header->textures.offset]);
  textures.resize(header->textures.number);

  for (std::uint32_t i = 0; i < header->textures.number; ++i) {
    if (texture_def[i].type == TextureType::FILENAME) {
      std::string texture_name = &buffer[texture_def[i].name.offset];
      spdlog::info("Texture index: {}, name: {}", i, texture_name);
      textures[i] = BLPTexture::create(texture_name);
      textures[i]->request_load_full();
    }
  }

  auto* tex_lookup = reinterpret_cast<const std::uint16_t*>(&buffer[header->tex_lookup.offset]);
  raw_tex_lookup.resize(header->tex_lookup.number);
  memcpy(raw_tex_lookup.data(), tex_lookup, header->tex_lookup.number * sizeof(std::uint16_t));

  // Probably we can get rid of it, but for now let's live with these guys
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texcoords;

  for (auto& vertex : raw_vertices) {
    vertices.push_back(vertex.pos);
    normals.push_back(glm::normalize(vertex.normal));
    texcoords.push_back(vertex.texcoords);
  }

  // That would be nice to delete all these buffers in the destructor, but
  // I don't want to call OpenGL-related things automatically in random places
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbuf);
  glGenBuffers(1, &tbuf);
  glGenBuffers(1, &nbuf);

  // Bind current VAO
  glBindVertexArray(vao);

  // TODO: Note that glVertexAttribPointer indices here are hardcoded, but probably we can get it from the shader

  // Upload positions
  glBindBuffer(GL_ARRAY_BUFFER, vbuf);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertices.size() * sizeof(glm::vec3)), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  // Upload normals
  glBindBuffer(GL_ARRAY_BUFFER, nbuf);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(normals.size() * sizeof(glm::vec3)), normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(1);

  // Upload texture coordinates
  glBindBuffer(GL_ARRAY_BUFFER, tbuf);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(texcoords.size() * sizeof(glm::vec2)), texcoords.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(2);

  // Clean the current buffer id
  glBindBuffer(GL_ARRAY_BUFFER, 0);
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

  // TODO: Create this array on every draw is not a good idea
  std::vector<M2ModelRenderPass> render_passes;

  for (std::uint32_t i = 0; i < model_view->header.tex.number; ++i) {
    M2ModelRenderPass pass{};

    ASSERT(i < model_view->raw_tex_units.size());
    auto& tex_unit = model_view->raw_tex_units[i];

    auto geoset = tex_unit.op;
    pass.geoset = geoset;

    ASSERT(tex_unit.texture_id < raw_tex_lookup.size());
    pass.tex = raw_tex_lookup[tex_unit.texture_id];

    render_passes.push_back(pass);
  }

  // Bind current VAO
  glBindVertexArray(vao);

  for (const auto& pass : render_passes) {
    ASSERT(pass.tex >= 0 && pass.tex < textures.size());
    auto& texture = textures[pass.tex];
    ASSERT(texture);

    if (!texture->is_loaded()) {
      // Skip drawing for unloaded textures
      continue;
    }

    // Bind the current texture
    glBindTexture(GL_TEXTURE_2D, texture->id);

    auto& geoset = model_view->raw_geosets[pass.geoset];

    // Currently we don't have a EBO, and we get the indices from RAM
    glDrawElements(GL_TRIANGLES, geoset.icount, GL_UNSIGNED_SHORT, &model_view->raw_indices[geoset.istart]);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // Unbind current VAO
  glBindVertexArray(0);
}
