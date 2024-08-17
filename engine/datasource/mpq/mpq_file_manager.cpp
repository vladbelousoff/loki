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

#include "mpq_file_manager.h"
#include "libassert/assert.hpp"

loki::MPQFileManager::MPQFileManager()
  : running(true)
  , thread(&MPQFileManager::run, this)
{
}

loki::MPQFileManager::~MPQFileManager()
{
  std::lock_guard<std::mutex> lock(requests_mutex);
  running = false;
  cv.notify_all();
}

void
loki::MPQFileManager::init(const std::filesystem::path& data_dir)
{
  enqueue_request([this, data_dir]() {
    chain = MPQChain(data_dir);
  });
}

void
loki::MPQFileManager::request_open(const std::filesystem::path& path, const FileCallback& callback)
{
  enqueue_request([this, path, callback]() {
    loki::MPQFile file;

    auto archive_handle = chain.get_archive().get_handle();
    ASSERT(archive_handle, "Handle is invalid");

    if (SFileOpenFileEx(archive_handle, path.string().c_str(), SFILE_OPEN_FROM_MPQ, &file.handle)) {
      char filename[MAX_PATH];
      if (SFileGetFileName(file.handle, filename)) {
        spdlog::info("Open file: {}", filename);
      }

      callback(file);

      bool result = SFileCloseFile(file.handle);
      ASSERT(result, "Can't close the file");
    } else {
      spdlog::error("Failed to open: {}", path.string().c_str());
    }
  });
}

void
loki::MPQFileManager::run()
{
  std::unique_lock<std::mutex> lock(requests_mutex);
  while (running) {
    cv.wait(lock, [this] {
      return !requests.empty() || !running;
    });

    process_next_request();
  }
}

void
loki::MPQFileManager::enqueue_request(loki::MPQFileManager::RequestCallback&& callback)
{
  std::unique_lock lock(requests_mutex);
  requests.emplace(callback);
  cv.notify_one();
}

void
loki::MPQFileManager::process_next_request()
{
  auto&& request = pop_next_request();
  if (request) {
    spdlog::info("Executing task...");
    request();
  }
}

loki::MPQFileManager::RequestCallback
loki::MPQFileManager::pop_next_request()
{
  if (requests.empty()) {
    return {};
  }

  auto request = std::move(requests.front());
  requests.pop();

  return request;
}
