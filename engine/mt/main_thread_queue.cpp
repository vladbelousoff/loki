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

#include "main_thread_queue.h"

void
loki::MainThreadQueue::add_task(loki::MainThreadQueue::Task&& task)
{
  std::lock_guard lock(mutex);
  task_queue.emplace(task);
}

void
loki::MainThreadQueue::perform_all_tasks()
{
  while (!is_empty()) {
    auto task = get_next_task();
    if (task) {
      task();
    }
  }
}

auto
loki::MainThreadQueue::get_next_task() -> Task
{
  std::lock_guard lock(mutex);
  if (task_queue.empty()) {
    return {};
  }

  auto task = std::move(task_queue.front());
  task_queue.pop();

  return task;
}

auto
loki::MainThreadQueue::is_empty() -> bool
{
  std::lock_guard lock(mutex);
  return task_queue.empty();
}
