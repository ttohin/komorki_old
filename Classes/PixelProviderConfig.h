//
//  PixelProviderConfig.h
//  prsv
//
//  Created by Anton Simakov on 17.07.16.
//
//

#ifndef __prsv__PixelProviderConfig__
#define __prsv__PixelProviderConfig__

#include "PixelDescriptor.h"

namespace komorki
{

class Config
{
public:
  
  struct CellConfig
  {
    int health;
    int sleepTime;
    int attack;
    int passiveHealthChunkMin;
    int passiveHealthChunkMax;
    int healthPerAttack;
    int armor;
    int lifeTime;
    float percentOfMutations;
    CellType danger;
    CellType food;
    CellType friends;
  };
  
  int terrainSize;
  int mapHeight;
  int mapWidth;
  
  float percentOfCreatures;
  float percentOfOrange;
  float percentOfGreen;
  float percentOfSalad;
  float percentOfCyan;
  float percentOfBlue;
  float percentOfWhite;
  
  CellConfig orange;
  CellConfig green;
  CellConfig salad;
  CellConfig cyan;
  CellConfig blue;
  CellConfig white;
  CellConfig yellow;
  CellConfig pink;
  
  Config::CellConfig* ConfigForCell(CellType type);
  
  Config();
  ~Config(){}
};
  
}

#endif /* defined(__prsv__PixelProviderConfig__) */
