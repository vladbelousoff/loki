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

#include "engine_app.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "spdlog/spdlog.h"
#include "time/scope_timer.h"

auto
loki::EngineApp::launch(const std::shared_ptr<EngineSettings>& _settings) -> int
{
  settings = _settings;

  if (!on_init()) {
    window = nullptr;
    return 1;
  }

  // Unbind VAO, VBO, and EBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  while (!glfwWindowShouldClose(window)) {
    loki::ScopeTimer scope_timer{ delta_time };

    // Main update
    on_update();

    // Draw main scene
    on_render();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw UI
    on_gui();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifdef _WIN32
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
#endif

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  on_term();
  return 0;
}

bool
loki::EngineApp::on_init()
{
  if (!glfwInit()) {
    spdlog::error("Failed to initialize GLFW!");
    return false;
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Set error callback
  glfwSetErrorCallback([](int error, const char* description) {
    spdlog::error("Code: {}, description: {}", error, description);
  });

  // Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(1920, 1080, "Loki Engine", nullptr, nullptr);
  if (!window) {
    spdlog::error("Failed to create GLFW window!");
    glfwTerminate();
    return false;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  // VSYNC
  glfwSwapInterval(1);

  // Init GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    spdlog::error("Failed to initialize GLAD!");
    return false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#ifdef _WIN32
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

  // Setup Dear ImGui style
  ImGui::StyleColorsLight();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  return true;
}

void
loki::EngineApp::on_term()
{
  // Some ImGui cleanups here
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  window = nullptr;
}
