//
//  CreaturesLogic.h
//  komorki
//
//  Created by ttohin on 21.03.15.
//
//

#ifndef __komorki__CellsLogic__
#define __komorki__CellsLogic__

#include "PixelDescriptor.h"
#include "PixelDescriptorProvider.h"

namespace komorki
{
  void ProcessCell(CellDescriptor* pd,
                   Vec2 pos,
                   PixelDescriptorProvider::Config* config,
                   const PixelDescriptorProvider::PixelMap& map, // ugly hack
                   Optional<Movement>& movement,
                   Optional<Action>& action,
                   Optional<Morphing>& morph);
}


#endif /* defined(__komorki__CellsLogic__) */
