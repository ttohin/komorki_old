//
//  WorldUpdateResult.cpp
//  Komorki
//
//  Created by user on 02.03.17.
//
//

#include "WorldUpdateResult.h"
#include "CellDescriptor.h"

namespace komorki
{
  WorldUpateDiff::WorldUpateDiff(CellDescriptor* desc)
  : desc(desc->parent)
  , userData(desc->userData)
  {
    //  assert(userData);
  }
}
