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

#include <functional>
#include <mutex>
#include <queue>

#include "engine/string_manager.h"
#include "mpq_chain.h"
#include "mpq_file.h"

namespace loki {

  class MPQFileManager
  {
    using FileCallback = std::function<void(MPQFile)>;
    using RequestCallback = std::function<void()>;

  public:
    explicit MPQFileManager();
    ~MPQFileManager();

  public:
    void init(const std::filesystem::path& data_dir);
    void request_open(const std::filesystem::path& path, const FileCallback& callback);

  private:
    void run();
    void enqueue_request(RequestCallback&& callback);

    RequestCallback pop_next_request();
    void process_next_request();

  private:
    MPQChain chain;
    bool running;
    std::jthread thread;
    std::mutex requests_mutex;
    std::condition_variable cv;
    std::queue<RequestCallback> requests;
  };

} // namespace loki