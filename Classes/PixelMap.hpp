//
//  PixelMap.hpp
//  Komorki
//
//  Created by Anton Simakov on 21/10/2016.
//
//

#ifndef PixelMap_hpp
#define PixelMap_hpp

#include "Common.h"

namespace komorki
{
  class PixelMap
  {
  public:
    PixelMap(const Vec2& size);
    virtual PixelDescriptor* GetDescriptor(PixelPos x, PixelPos y) const;
    virtual Vec2 GetSize() const;
    
  private:
    
    typedef std::shared_ptr<PixelDescriptor> PixelPtr;
    typedef std::vector<std::vector<PixelPtr> > PixelsArray;
    
    PixelsArray m_map;
    Vec2 m_size;
  };
}

#endif /* PixelMap_hpp */
