//
//  CreaturesLogic.h
//  komorki
//
//  Created by ttohin on 21.03.15.
//
//

#ifndef __komorki__CellsLogic__
#define __komorki__CellsLogic__

#include "GreatPixel.h"
#include "PixelWorld.h"

namespace komorki
{
  void ProcessCell(CellDescriptor* pd,
                   Vec2 pos,
                   const PixelWorld::PixelMap& map, // ugly hack
                   Optional<Movement>& movement,
                   Optional<Action>& action,
                   Optional<Morphing>& morph,
                   Optional<ChangeRect>& changeRect);
}


#endif /* defined(__komorki__CellsLogic__) */
