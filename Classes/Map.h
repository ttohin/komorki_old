//
//  Map.h
//  prsv
//
//  Created by ttohin on 07.06.15.
//
//

#ifndef __prsv__Map__
#define __prsv__Map__

namespace komorki
{
  class PixelDescriptorProvider;
  
  class Map
  {
  public:
    void Apply(PixelDescriptorProvider* provider);
  };
}

#endif /* defined(__prsv__Map__) */
