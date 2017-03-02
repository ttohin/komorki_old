//
//  PixelMap.hpp
//  Komorki
//
//  Created by Anton Simakov on 21/10/2016.
//
//

#ifndef PixelMap_hpp
#define PixelMap_hpp

#include <memory>
#include "Common.h"


namespace komorki
{
  class PixelMap
  {
  public:
    
    typedef std::shared_ptr<PixelMap> Ptr;
    
    PixelMap(const Vec2& size);
    virtual ~PixelMap() {}
    virtual GreatPixel* GetDescriptor(PixelPos x, PixelPos y) const;
    virtual Vec2 GetSize() const;
    
  private:
    
    typedef std::shared_ptr<GreatPixel> PixelPtr;
    typedef std::vector<std::vector<PixelPtr> > PixelsArray;
    
    PixelsArray m_map;
    Vec2 m_size;
  };
}

#endif /* PixelMap_hpp */
