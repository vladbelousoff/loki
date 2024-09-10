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

#include "game_app.h"

#include "engine/model/m_2_model.h"
#include "engine/mt/main_thread_queue.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "spdlog/spdlog.h"

#include <boost/pfr.hpp>

struct
{
  float distance_to_origin{ 5.f };
  float phi{ 1.08f }, theta{ 5.8f };
} camera;

glm::vec3 light_position(0.0, -0.2, 0.2);

static std::string default_shader_vert =
    "#version 330 core\n"
    "layout (location = 0) in vec3 a_position;\n"
    "layout (location = 1) in vec3 a_normal;\n"
    "layout (location = 2) in vec2 a_texcoord;\n"
    "uniform mat4 u_model;\n"
    "uniform mat4 u_view;\n"
    "uniform mat4 u_projection;\n"
    "out vec3 normal;\n"
    "out vec2 texcoord;\n"
    "void main() {\n"
    "  normal = mat3(transpose(inverse(u_model))) * a_normal;\n"
    "  gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0f);\n"
    "  texcoord = a_texcoord;\n"
    "}\n";

static std::string default_shader_frag =
    "#version 330 core\n"
    "in vec3 normal;\n"
    "in vec2 texcoord;\n"
    "out vec4 color;\n"
    "uniform sampler2D u_texture;\n"
    "uniform vec3 u_light_position;\n"
    "void main() {\n"
    "  float lighting = max(dot(normalize(normal), normalize(u_light_position)), 0.0);\n"
    "  vec4 tex_color = texture(u_texture, texcoord);\n"
    "  color = tex_color;\n"
    "}\n";

GameApp::~GameApp()
{
  loki::MPQFileManager::get_ref().term();
}

// std::filesystem::path model_path = R"(Character\Draenei\Female\DraeneiFemale.M2)";
std::filesystem::path model_path = R"(Creature\ArthasLichKing\ArthasLichKing.M2)";
std::shared_ptr<loki::M2Model> m2_model;

bool
GameApp::on_init()
{
  if (!EngineApp::on_init()) {
    return false;
  }

  sockpp::initialize();

  frag = loki::ShaderManager::create_shader(default_shader_frag, loki::ShaderType::FRAG);
  vert = loki::ShaderManager::create_shader(default_shader_vert, loki::ShaderType::VERT);
  prog = loki::ShaderManager::create_program(vert, frag);

  loki::MPQFileManager::get_ref().init(get_root_path() / "data");
  m2_model = loki::M2Model::create(model_path);
  m2_model->request_load_full();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  return true;
}

void
GameApp::on_term()
{
  EngineApp::on_term();
}

void
GameApp::on_update()
{
  // Update main thread at the start of the frame
  loki::MainThreadQueue::get_ref().perform_all_tasks();

  float x = camera.distance_to_origin * glm::sin(camera.phi) * glm::cos(camera.theta);
  float y = camera.distance_to_origin * glm::sin(camera.phi) * glm::sin(camera.theta);
  float z = camera.distance_to_origin * glm::cos(camera.phi);

  view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1));

  loki::ShaderManager::use_program(prog, [this](const loki::UniformManager& manager) {
    manager.set_uniform("u_view", view);
  });

  glm::ivec2 window_size;
  glfwGetWindowSize(get_window(), &window_size.x, &window_size.y);
  glViewport(0, 0, window_size.x, window_size.y);

  projection = glm::perspective(glm::radians(45.0f), (float)window_size.x / (float)window_size.y, 0.1f, 100.0f);

  loki::ShaderManager::use_program(prog, [this](const loki::UniformManager& manager) {
    manager.set_uniform("u_projection", projection);
    manager.set_uniform("u_light_position", light_position);
  });
}

void
GameApp::on_gui()
{
  if (ImGui::Begin("Settings")) {
    ImGui::SliderFloat("Distance", &camera.distance_to_origin, 0.0f, 100.0f);
    ImGui::SliderFloat("Phi", &camera.phi, 0.0f, glm::pi<float>() * 2.f);
    ImGui::SliderFloat("Theta", &camera.theta, 0.0f, glm::pi<float>() * 2.f);

#if 0
    ImGui::SliderFloat("Light X", &light_position.x, -1.0f, 1.0f);
    ImGui::SliderFloat("Light Y", &light_position.y, -1.0f, 1.0f);
    ImGui::SliderFloat("Light Z", &light_position.z, -1.0f, 1.0f);
#endif

    static char host[32] = "localhost";
    ImGui::InputText("Host", host, sizeof(host));
    static int port = 3724;
    ImGui::InputInt("Port", &port);
    static char username[32] = "test";
    ImGui::InputText("Username", username, sizeof(username));
    static char password[32] = "test";
    ImGui::InputText("Password", password, sizeof(password));

    if (ImGui::Button("Connect")) {
      spdlog::info("Connecting to auth-server @{}:{}...", host, port);

      auth_session = std::make_shared<loki::AuthSession>(host, (std::uint16_t)port);
      auth_session->login(username, password);
    }

    if (auth_session) {
      auto realms = auth_session->get_realms();
      int num_of_fields = boost::pfr::detail::fields_count<loki::PacketAuthRealm>();
      if (ImGui::BeginTable("Realms", num_of_fields + 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Table headers
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Locked");
        ImGui::TableSetupColumn("Flags");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Server Socket");
        ImGui::TableSetupColumn("Population");
        ImGui::TableSetupColumn("Number of Characters");
        ImGui::TableSetupColumn("Category");
        ImGui::TableSetupColumn("Realm ID");
        ImGui::TableHeadersRow();

        for (const auto& realm : realms) {
          ImGui::TableNextRow();

          boost::pfr::for_each_field(realm, [](auto& field, auto field_index) {
            ImGui::TableSetColumnIndex(field_index);
            std::string string = fmt::format("{}", field);
            ImGui::Text("%s", string.c_str());
          });

          ImGui::TableSetColumnIndex(num_of_fields);
          if (ImGui::Button("Connect")) {
            world_session = auth_session->connect_to_realm(realm.realm_id);
          }
        }

        ImGui::EndTable();
      }
    }
  }

  ImGui::End();
}

void
GameApp::on_render()
{
  glClearColor(background.r, background.g, background.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);

  loki::ShaderManager::use_program(prog, [this](const loki::UniformManager& manager) {
    manager.set_uniform("u_model", model);
    m2_model->draw();
  });
}
