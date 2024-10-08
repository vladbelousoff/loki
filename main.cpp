#include "CLI/CLI.hpp"
#include "game/game_app.h"
#include "spdlog/spdlog.h"

int
main(int argc, char* argv[])
{
  CLI::App app{ "Loki Project" };
  argv = app.ensure_utf8(argv);

  std::shared_ptr<loki::EngineSettings> settings = std::make_shared<loki::EngineSettings>();

  app.add_option("--root", settings->root_path);
  CLI11_PARSE(app, argc, argv)

  spdlog::info("Root: {}", absolute(settings->root_path).string());
  return GameApp().launch(settings);
}
