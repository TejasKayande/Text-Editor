/*===============================================================================
  @File:   platform.cpp
  @Brief:  Platform-specific entry point selection.
  @Author: Tejas
  @Date:   28-08-2025 
  @Notice: Released under the MIT License. See LICENSE file for details.
  =============================================================================== */

#include "base.h"

#if ON_WINDOWS
#define NOMINMAX
#include <windows.h>
#include "win_platform.cpp"
#include "win_main.cpp"
#else

#error This Platform is currently not supported!
#endif
