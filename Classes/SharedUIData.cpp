//
//  SharedUIData.cpp
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#include "SharedUIData.h"

namespace komorki
{
namespace ui
{
  // singleton stuff
  static SharedUIData *s_SharedDirector = nullptr;
  
  SharedUIData* SharedUIData::getInstance()
  {
    if (!s_SharedDirector)
    {
      s_SharedDirector = new (std::nothrow) SharedUIData();
    }
    
    return s_SharedDirector;
  }
}
}
