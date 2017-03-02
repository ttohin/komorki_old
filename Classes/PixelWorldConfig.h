//
//  PixelWorldConfig.h
//  Komorki
//
//  Created 02.03.17.
//

#pragma once

#include "Common.h"

namespace komorki
{
  struct PixelWorldConfig
  {
    PixelPos width = 300;
    PixelPos height = 200;
    float percentOfCellsOnStartup = 0.4;
  };
}

