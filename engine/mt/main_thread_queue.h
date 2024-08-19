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

namespace loki {

  class MainThreadQueue
  {
    using Task = std::function<void()>;

  public:
    static MainThreadQueue& get_ref()
    {
      static MainThreadQueue instance;
      return instance;
    }

    void add_task(Task&& task);
    void perform_all_tasks();

    auto is_empty() -> bool;
    auto get_next_task() -> Task;

  private:
    MainThreadQueue() = default;

  private:
    std::mutex mutex{};
    std::queue<Task> task_queue{};
  };

} // namespace loki

