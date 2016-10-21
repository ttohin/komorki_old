//
//  PixelMap.cpp
//  Komorki
//
//  Created by Anton Simakov on 21/10/2016.
//
//

#include "PixelMap.hpp"
#include "PixelDescriptor.h"

namespace komorki
{
  PixelMap::PixelMap(const Vec2& size)
  : m_size(size)
  {
    m_map.reserve(size.x);
    for (int i = 0; i < size.x; ++i)
    {
      m_map.push_back(std::vector<PixelPtr>());
      m_map[i].reserve(size.y);
      for (int j = 0; j < size.y; ++j)
      {
        m_map[i].push_back(std::make_shared<PixelDescriptor>(i, j));
      }
    }
    
    for (int i = 0; i < size.x; ++i)
    {
      for (int j = 0; j < size.y; ++j)
      {
        PixelDescriptor* pd = m_map[i][j].get();
        
        for (int di = -1; di <= 1; ++di)
        {
          for (int dj = -1; dj <= 1; ++dj)
          {
            if (i + di >= size.x || i + di < 0)
            {
              continue;
            }
            
            if (j + dj >= size.y || j + dj < 0)
            {
              continue;
            }
            
            if (di == 0 && dj == 0) {
              continue;
            }
            
            PixelDescriptor* aroundPd = m_map[i + di][j + dj].get();
            pd->SetDirection(di, dj, aroundPd);
          }
        }
      }
    }
  }
  
  PixelDescriptor* PixelMap::GetDescriptor(PixelPos x, PixelPos y) const
  {
    if (x < 0 || x >= m_size.x)
    {
      return nullptr;
    }
    
    if (y < 0 || y >= m_size.y)
    {
      return nullptr;
    }
    
    return m_map[x][y].get();
  }
  
  Vec2 PixelMap::GetSize() const
  {
    return m_size;
  }
  
}
