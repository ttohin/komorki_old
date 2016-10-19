//
//  SharedUIData.h
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#ifndef __prsv__SharedUIData__
#define __prsv__SharedUIData__

#include <cocos2d.h>

namespace komorki
{
  namespace ui
  {
    class SharedUIData
    {
    public:
      static SharedUIData* getInstance();
      
      cocos2d::Texture2D* cellsTexture = nullptr;
    };
  }
}

#endif /* defined(__prsv__SharedUIData__) */
