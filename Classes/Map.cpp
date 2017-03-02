//
//  Map.cpp
//  prsv
//
//  Created by ttohin on 07.06.15.
//
//

#include "Map.h"
#include "PixelDescriptorProvider.h"
#include "Random.h"

namespace
{
  const float kMaxHeight = 0.2f;
}

using namespace komorki;

void Map::Apply(komorki::PixelDescriptorProvider *provider)
{
  Vec2 size = provider->GetSize();
  for (int i = 0; i < size.x; ++i)
  {
    int height = cRandABInt(1, size.y * kMaxHeight);
    for (int j = 0; j < height; ++j)
    {
      auto pixelD = provider->GetDescriptor(i, j);
      if (pixelD) pixelD->m_type = PixelDescriptor::TerrainType;
    }
    
    for (int j = height - 2; j >= 0; --j)
    {
      int minWidht = 0.1f * (float)(height - 2 - j) + 1;
      int maxWidht = 0.3f * (float)(height - 2 - j) + 1;
      int side = cRandABInt(minWidht, maxWidht);
      for (int c = 0; c < side; c++)
      {
        auto pixelD = provider->GetDescriptor(i + c, j);
        if (pixelD) pixelD->m_type = PixelDescriptor::TerrainType;
      }
      
      side = cRandABInt(minWidht, maxWidht);
      for (int c = 0; c < side; c++)
      {
        auto pixelD = provider->GetDescriptor(i - c, j);
        if (pixelD) pixelD->m_type = PixelDescriptor::TerrainType;
      }
    }
  }
}
