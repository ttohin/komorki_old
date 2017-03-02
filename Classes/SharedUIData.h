//
//  SharedUIData.h
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#ifndef __prsv__SharedUIData__
#define __prsv__SharedUIData__

#include <list>
#include <unordered_map>
#include <cocos2d.h>
#include "ShapesGenerator.h"
#include "CellDescriptor.h"
#include "GenomsGenerator.h"

namespace komorki
{
  namespace graphic
  {
    class SharedUIData
    {
    public:
      static SharedUIData* getInstance();
      
      typedef std::unordered_map<komorki::Genom::GroupIdType, cocos2d::Rect> CellShapeTexturesMap;
      
      SharedUIData();
      
      CellShapeTexturesMap m_textureMap;
      GenomsGenerator::Ptr m_genomsGenerator;
    };
  }
}

#endif /* defined(__prsv__SharedUIData__) */
