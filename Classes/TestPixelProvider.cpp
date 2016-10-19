//
//  TestPixelProvider.cpp
//  prsv
//
//  Created by Anton Simakov on 17.07.16.
//
//

#include "TestPixelProvider.h"
#include <iostream>
#include <assert.h>

namespace komorki
{
  TestPixelProvider::TestPixelProvider()
  {
    bool result = LoadMap();
    assert(result);
    
    m_testConfig.mapWidth = m_mapSize.x;
    m_testConfig.mapHeight = m_mapSize.y;
   
    InitWithConfig(&m_testConfig);
  }
  
  bool TestPixelProvider::LoadMap()
  {
    std::string mapString =
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "-------#-#----------------------------------------\n"
    "-------#a#----------------------------------------\n"
    "-------##-----------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    "--------------------------------------------------\n"
    ;
    
    m_stringMap = mapString;
    
    Vec2 resultMapSize;
    Vec2 mapSize;
    
    for (const auto& s : m_stringMap)
    {
      if (s == '\n')
      {
        if (resultMapSize.x == 0)
        {
          resultMapSize.x = mapSize.x;
        }
        else
        {
          assert(resultMapSize.x == mapSize.x);
        }
        
        mapSize.y += 1;
        mapSize.x = 0;
      }
      else
      {
        mapSize.x += 1;
      }
    }
    
    resultMapSize.y = mapSize.y;
    m_mapSize = resultMapSize;
    return resultMapSize != Vec2();
  }
  
  void TestPixelProvider::ForEachSymbolInMap(const PerMapSymbolFunc& func)
  {
    Vec2 pos;
    
    for (const auto& s : m_stringMap)
    {
      Vec2 actualPos = Vec2(pos.x, m_mapSize.y - pos.y);
      func(s, actualPos);
      if (s == '\n')
      {
        pos.y += 1;
        pos.x = 0;
      }
      else
      {
        pos.x += 1;
      }
    }
  }
  
  
  void TestPixelProvider::PrintAsciiArt() const
  {
    for (int y = m_mapSize.y - 1; y >= 0; y--)
    {
      for (int x = 0; x < m_mapSize.x; x++)
      {
        std::cout << m_map[x][y]->GetAscii('a');
      }
      std::cout << std::endl;
    }
  }
  
  void TestPixelProvider::GenTerrain()
  {
    auto buff = std::make_shared<Buffer2D<float>>(m_mapSize.x, m_mapSize.y);
    ForEachSymbolInMap([&](char s, const Vec2& pos){
      if (s == '#')
      {
        buff->Set(pos.x, pos.y, 1.0);
      }
    });
    
    auto analizer = std::shared_ptr<TerrainAnalizer>(new TerrainAnalizer(buff));
    auto analizedBuffer = analizer->GetLevels();
    
    analizedBuffer->ForEach([&](const int& x, const int& y, const TerrainLevel& level)
                            {
                              if (x%2 != 0 && y%2 != 0) {
                                return ;
                              }
                              if (level >= TerrainLevel::Ground)
                              {
                                m_map[x/2][y/2]->m_type = PixelDescriptor::TerrainType;
                              }
                            });
    
    m_terrain = analizer->GetResult();
  }
  
  void TestPixelProvider::GenLights()
  {
    
  }
  
  
  void TestPixelProvider::PopulateCells()
  {
    ForEachSymbolInMap([&](char s, const Vec2& pos){
      if (s == 'a')
      {

      }
    });
  }
  
}