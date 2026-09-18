// game_constants.h - Game-specific constants
#pragma once

#include <cstdint>

namespace GameConstants
{
  constexpr uint32_t kCodeBase = 0x82210000;
  constexpr uint32_t kCodeEnd = 0x8309C604;
}

namespace GameConstants::PatchConstants
{
  struct Patch
  {
    std::uintptr_t address;
    std::uint32_t value;
  };

  // Ported from xenia-canary game-patches for Army of Two retail (4541084C)
  // NOPs the frame-limiter check; game speed is tied to FPS above ~90FPS.
  constexpr Patch FpsUnlockNop()
  {
    return Patch{
        0x8247D550,
        0x60000000};
  }

  // Frame-rate target byte: 0x00=unlimited, 0x01=60FPS, 0x02=30FPS.
  constexpr Patch FpsUnlockMode()
  {
    return Patch{
        0x82A56447,
        0x01};
  }

  // "Black Shading Fix" - disables MSAA.
  constexpr Patch DisableMsaa()
  {
    return Patch{
        0x830B6FE7,
        0x04};
  }

  constexpr Patch AnisotropicFiltering16x()
  {
    return Patch{
        0x829D57D0,
        0x38A00010};
  }
}