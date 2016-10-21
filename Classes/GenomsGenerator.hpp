//
//  GenomsGenerator.hpp
//  Komorki
//
//  Created by Anton Simakov on 21/10/2016.
//
//

#ifndef GenomsGenerator_hpp
#define GenomsGenerator_hpp

#include <list>
#include <memory>
#include "ShapesGenerator.h"
#include "CellDescriptor.h"

namespace komorki
{
  class GenomsGenerator
  {
  public:
    
    typedef std::shared_ptr<GenomsGenerator> Ptr;
    typedef std::list<Genom> GenomsList;
    
    Genom GenerateGenom(Genom::GroupIdType groupId, const ShapesGenerator::ResultItem &shape) const;
    bool AddShape(const ShapesGenerator::ResultItem& shape, Genom::GroupIdType& outGroupId);
    
    GenomsList m_genomsList;
    PixelMap::Ptr m_internalMap;
  };
}

#endif /* GenomsGenerator_hpp */
