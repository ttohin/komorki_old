//
//  TerrainAnalizer.h
//  ground
//
//  Created by ttohin on 05.09.15.
//
//

#pragma once

#include "Buffer2D.h"

namespace komorki
{
  enum class TerrainLevel
  {
    Water,
    Background,
    Ground,
    Foreground
  };
  
  enum class TerrainPos
  {
    Unknown,
    Corner,
    Border,
    BorderToCornerLeft,
    BorderToCornerRight,
    Fill,
    InnnerCornter
  };
  
  enum class TerrainRotation
  {
    Zero,
    PI_2,
    PI,
    PI_3_2
  };
  
  struct TerrainInfo
  {
    TerrainLevel level = TerrainLevel::Water;
    TerrainLevel resultLevel = TerrainLevel::Water;
    TerrainPos pos = TerrainPos::Unknown;
    TerrainRotation rotation = TerrainRotation::Zero;
    int color = -1;
  };
  
  class TerrainAnalizer {
    
  public:
    
    using TerrainInfoBuffer = Buffer2DPtr<TerrainInfo>;
    using Levels = Buffer2D<TerrainLevel>;
    using ResultPtr = Buffer2DPtr<TerrainInfo>;
    using LevelsPtr = Buffer2DPtr<TerrainLevel>;
    
    struct Result
    {
      TerrainInfoBuffer background;
      TerrainInfoBuffer ground;
      TerrainInfoBuffer foreground;
    };
    
    TerrainAnalizer(const Buffer2DPtr<float>& buffer);
    Result GetResult() const;
    LevelsPtr GetLevels() const;
    
    virtual ~TerrainAnalizer () {}
    
  private:
    
    void MakeColors(TerrainInfoBuffer buffer);
    void Analize(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info, TerrainLevel level);
    int CountNeighbors(TerrainInfoBuffer buffer, int x, int y, TerrainLevel level) const;
    int CountNeighborsSquare(TerrainInfoBuffer buffer, int x, int y, TerrainLevel level) const;
    int CountNeighborsDiamond(TerrainInfoBuffer buffer, int x, int y, TerrainLevel level) const;
    int ColorFromneighbros(TerrainInfoBuffer buffer, int x, int y, TerrainLevel level) const;
    void AnalizeCorner(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info);
    void AnalizeBorder(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info);
    void AnalizeBorderToCorner(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info);
    void AnalizeInnterCorner(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info);
    bool GetInfo(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info, TerrainLevel defaultLevel) const;
    
    Buffer2DPtr<float> m_buffer;
    ResultPtr m_bg;
    ResultPtr m_ground;
    ResultPtr m_foreground;
    LevelsPtr m_levels;
  };
  
}
