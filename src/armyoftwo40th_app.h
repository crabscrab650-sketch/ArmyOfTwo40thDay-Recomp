// armyoftwo40threcomp - Army of Two: The 40th Day Recompiled Project
// Based on ReXGlue SDK v0.10.0+

#pragma once

#include <cstdlib>
#include <memory>
#include <string>

#include <rex/cvar.h>
#include <rex/logging.h>
#include <rex/memory/utils.h>
#include <rex/rex_app.h>
#include <rex/runtime.h>
#include <rex/system/gpu_plugin.h>

#include "debug_tools.h"
#include "game_cvars.h"
#include "game_patches.h"
#include "utils.h"

#ifdef REXGLUE_ENABLE_PERF_COUNTERS
#include <rex/perf/counter.h>
#endif

class Armyoftwo40thApp : public rex::ReXApp
{
public:
  using rex::ReXApp::ReXApp;

  static std::unique_ptr<rex::ui::WindowedApp> Create(
      rex::ui::WindowedAppContext &ctx)
  {
#ifdef PPCImageConfig
    return std::unique_ptr<Armyoftwo40thApp>(new Armyoftwo40thApp(ctx, "armyoftwo40threcomp",
                                                                 PPCImageConfig));
#else
    return std::unique_ptr<Armyoftwo40thApp>(new Armyoftwo40thApp(ctx, "armyoftwo40threcomp"));
#endif
  }

  void OnPreSetup(rex::RuntimeConfig &config) override
  {
    std::string backend = REXCVAR_GET(graphics_backend);
    if (backend != "any" && !config.gpu_plugin.empty())
    {
      config.graphics = rex::system::LoadGpuPlugin(config.gpu_plugin, backend);
      if (!config.graphics)
      {
        REXLOG_WARN("graphics_backend '{}' unavailable, falling back to automatic selection",
                    backend);
      }
    }
  }

  void OnPostLoadXexImage() override
  {
    game_patches::FpsUnlock();
    game_patches::DisableMsaa();
    game_patches::AnisotropicFiltering16x();
  }

  void OnPostSetup() override
  {
    utils::LoadSettingsFiles();

#ifdef REXGLUE_ENABLE_PERF_COUNTERS
    rex::perf::SetCsvLogPath(rex::cvar::GetFlagByName("perf_log_csv"));
#endif

    if (REXCVAR_GET(dev_debug_runtime))
    {
      debug_tools::PerformMissingFunctionScan();
      debug_tools::PerformStubSweep();
    }
  }

  void OnShutdown() override
  {
#ifdef REXGLUE_ENABLE_PERF_COUNTERS
    rex::perf::FlushCsv();
#endif
  }

  void OnConfigurePaths(rex::PathConfig &paths) override
  {
    if (paths.game_data_root.empty())
    {
      paths.game_data_root = utils::RepoRoot() / "assets";
    }

    paths.config_path = utils::SettingsDir() / "hardware.toml";
    utils::LoadSettingsFiles();
  }
};
