
#pragma once

#include "PixelDescriptor.h"
#include <list>
#include "Common.h"
#include "TerrainAnalizer.h"
#include "GenomsGenerator.h"
#include "WorldUpdateResult.h"

namespace komorki
{
  class CellDescriptor;
  class PixelDescriptor;
  class Config;

  class IPixelDescriptorProvider
  {
  public:
    virtual PixelDescriptor* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const = 0;
    virtual void InitWithConfig(Config* config, const GenomsGenerator::GenomsList& genoms) = 0;
    virtual TerrainAnalizer::Result GetTerrain() const = 0;
    virtual komorki::Vec2 GetSize() const = 0;
    
    virtual void Update(bool passUpdateResult, WorldUpdateList&) = 0;
    virtual ~IPixelDescriptorProvider () {};
  };
}

