// game_patches.h - Game-specific memory patches
#pragma once

#include <algorithm>
#include <cstdint>

#include <rex/cvar.h>
#include <rex/memory/utils.h>
#include <rex/runtime.h>

#include "game_cvars.h"
#include "game_constants.h"

namespace game_patches
{
  // Ported from xenia-canary game-patches for Army of Two (4541084C)
  // https://github.com/xenia-canary/game-patches/blob/main/patches/4541084C%20-%20Army%20of%20Two%20(Europe).patch.toml
  static void FpsUnlock()
  {
    if (!REXCVAR_GET(ao2_fps_unlock))
    {
      return;
    }

    auto *rt = rex::Runtime::instance();
    uint8_t *base = rt ? rt->virtual_membase() : nullptr;
    if (!base)
    {
      return;
    }

    // NOP the frame-limiter check (be32).
    {
      auto memoryAddr = GameConstants::PatchConstants::FpsUnlockNop().address;
      auto memoryValue = GameConstants::PatchConstants::FpsUnlockNop().value;
      uint8_t *p = base + memoryAddr;
      rex::memory::PageAccess old_access{};
      rex::memory::Protect(p, sizeof(memoryValue), rex::memory::PageAccess::kReadWrite, &old_access);
      p[0] = static_cast<uint8_t>(memoryValue >> 24);
      p[1] = static_cast<uint8_t>(memoryValue >> 16);
      p[2] = static_cast<uint8_t>(memoryValue >> 8);
      p[3] = static_cast<uint8_t>(memoryValue);
      rex::memory::Protect(p, sizeof(memoryValue), old_access, nullptr);
    }

    // Frame-rate target byte, configurable via ao2_fps_unlock_mode (clamped
    // to the 0-2 range the game code understands).
    {
      auto memoryAddr = GameConstants::PatchConstants::FpsUnlockMode().address;
      int32_t mode = REXCVAR_GET(ao2_fps_unlock_mode);
      uint8_t memoryValue = static_cast<uint8_t>(std::clamp(mode, 0, 2));
      uint8_t *p = base + memoryAddr;
      rex::memory::PageAccess old_access{};
      rex::memory::Protect(p, 1, rex::memory::PageAccess::kReadWrite, &old_access);
      p[0] = memoryValue;
      rex::memory::Protect(p, 1, old_access, nullptr);
    }
  }

  // "Black Shading Fix" - disables MSAA.
  static void DisableMsaa()
  {
    if (!REXCVAR_GET(ao2_disable_msaa))
    {
      return;
    }

    auto *rt = rex::Runtime::instance();
    uint8_t *base = rt ? rt->virtual_membase() : nullptr;
    if (!base)
    {
      return;
    }
    auto memoryAddr = GameConstants::PatchConstants::DisableMsaa().address;
    auto memoryValue = GameConstants::PatchConstants::DisableMsaa().value;
    uint8_t *p = base + memoryAddr;

    rex::memory::PageAccess old_access{};
    rex::memory::Protect(p, 1, rex::memory::PageAccess::kReadWrite, &old_access);
    p[0] = static_cast<uint8_t>(memoryValue);
    rex::memory::Protect(p, 1, old_access, nullptr);
  }

  static void AnisotropicFiltering16x()
  {
    if (!REXCVAR_GET(ao2_anisotropic_16x))
    {
      return;
    }

    auto *rt = rex::Runtime::instance();
    uint8_t *base = rt ? rt->virtual_membase() : nullptr;
    if (!base)
    {
      return;
    }
    auto memoryAddr = GameConstants::PatchConstants::AnisotropicFiltering16x().address;
    auto memoryValue = GameConstants::PatchConstants::AnisotropicFiltering16x().value;
    uint8_t *p = base + memoryAddr;

    // be32 patch - write all 4 bytes in big-endian order.
    rex::memory::PageAccess old_access{};
    rex::memory::Protect(p, sizeof(memoryValue), rex::memory::PageAccess::kReadWrite, &old_access);
    p[0] = static_cast<uint8_t>(memoryValue >> 24);
    p[1] = static_cast<uint8_t>(memoryValue >> 16);
    p[2] = static_cast<uint8_t>(memoryValue >> 8);
    p[3] = static_cast<uint8_t>(memoryValue);
    rex::memory::Protect(p, sizeof(memoryValue), old_access, nullptr);
  }
} // namespace game_patches
