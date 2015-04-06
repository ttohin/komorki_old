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
  void ProcessCell(PixelDescriptor* pd,
                   Vec2 pos,
                   const PixelDescriptorProvider::PixelMap& map, // ugly hack
                   Optional<Movement>& movement,
                   Optional<Action>& action);
}


#endif /* defined(__komorki__CellsLogic__) */
