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
    PixelPos width = 150;
    PixelPos height = 100;
    float percentOfCellsOnStartup = 0.4;
    bool generateTerrain = false;
    bool generateLight = false;
  };
}

