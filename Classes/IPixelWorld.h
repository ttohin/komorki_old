
#pragma once

#include "GreatPixel.h"
#include <list>
#include "Common.h"
#include "TerrainAnalizer.h"
#include "GenomsGenerator.h"
#include "WorldUpdateResult.h"
#include "PixelWorldConfig.h"

namespace komorki
{
  class CellDescriptor;
  class GreatPixel;
  class Config;

  class IPixelWorld
  {
  public:
    virtual GreatPixel* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const = 0;
    virtual TerrainAnalizer::Result GetTerrain() const = 0;
    virtual komorki::Vec2 GetSize() const = 0;
    virtual void Init(const PixelWorldConfig& config,
                      const GenomsGenerator::GenomsList& genoms) = 0;
    
    virtual void Update(bool passUpdateResult, WorldUpdateList&) = 0;
    virtual ~IPixelWorld () {};
  };
}

