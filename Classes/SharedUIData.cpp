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
  namespace graphic
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
    
    SharedUIData::SharedUIData()
    : m_genomsGenerator(new GenomsGenerator())
    {
    }
  }
}
