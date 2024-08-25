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

#include "blp_texture.h"

#include "GL/glew.h"
#include "blp.h"
#include "libassert/assert.hpp"

void
loki::BLPTexture::on_fully_loaded(const std::vector<char>& buffer)
{
  tBLPInfos blp_info = blp_process_buffer(buffer.data());
  ASSERT(blp_info);

  tBGRAPixel* raw_image_data = blp_convert_buffer(buffer.data(), blp_info);
  ASSERT(raw_image_data);

  const auto width = static_cast<GLsizei>(blp_width(blp_info));
  const auto height = static_cast<GLsizei>(blp_height(blp_info));

  // Create new texture and put it in memory
  glGenTextures(1, &id);
  GLuint tex_format = GL_TEXTURE_2D;
  glBindTexture(tex_format, id);

  glTexImage2D(tex_format, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw_image_data);
  glGenerateMipmap(tex_format);

  glTexParameteri(tex_format, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Filtering
  glTexParameteri(tex_format, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Filtering

  delete[] raw_image_data;
  blp_release(blp_info);

  glBindTexture(tex_format, 0);
}
