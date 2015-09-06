//
//  TerrainAnalizer.cpp
//  ground
//
//  Created by ttohin on 05.09.15.
//
//

#include "TerrainAnalizer.h"


TerrainAnalizer::TerrainAnalizer(const Buffer2DPtr<float>& buffer)
: m_buffer(buffer)
, m_bg(new Buffer2D<TerrainInfo>(buffer->GetWidth() * 2, buffer->GetHeight() * 2))
, m_ground(new Buffer2D<TerrainInfo>(buffer->GetWidth() * 2, buffer->GetHeight() * 2))
, m_foreground(new Buffer2D<TerrainInfo>(buffer->GetWidth() * 2, buffer->GetHeight() * 2))
, m_levels(new Levels(buffer->GetWidth() * 2, buffer->GetHeight() * 2))
{
  float topLevel = 0.84;
  float wallLevel = 0.7;
  float bgLevel = 0.19;
  
  buffer->ForEach([&](int _x, int _y, const float& value)
                 {
                   int x = _x * 2;
                   int y = _y * 2;
                   TerrainInfo info;
                   if (value > topLevel)
                   {
                     auto level = TerrainLevel::Foreground;
                     m_levels->Set(x, y, level);
                     m_levels->Set(x + 1, y + 1, level);
                     m_levels->Set(x, y + 1, level);
                     m_levels->Set(x + 1, y, level);
                   }
                   else if (value > wallLevel)
                   {
                     auto level = TerrainLevel::Ground;
                     m_levels->Set(x, y, level);
                     m_levels->Set(x + 1, y + 1, level);
                     m_levels->Set(x, y + 1, level);
                     m_levels->Set(x + 1, y, level);
                   }
                   else if (value > bgLevel)
                   {
                     auto level = TerrainLevel::Background;
                     m_levels->Set(x, y, level);
                     m_levels->Set(x + 1, y + 1, level);
                     m_levels->Set(x, y + 1, level);
                     m_levels->Set(x + 1, y, level);
                   }
                   else
                   {
                     auto level = TerrainLevel::Water;
                     m_levels->Set(x, y, level);
                     m_levels->Set(x + 1, y + 1, level);
                     m_levels->Set(x, y + 1, level);
                     m_levels->Set(x + 1, y, level);
                   }});
 
  m_levels->ForEach([&](const int& x, const int& y, const TerrainLevel& level)
                    {
                      TerrainInfo info;
                      info.level = level;
                      switch (level) {
                        case TerrainLevel::Foreground:
                          m_foreground->Set(x, y, info);
                        case TerrainLevel::Ground:
                          info.level = TerrainLevel::Ground;
                          m_ground->Set(x, y, info);
                        case TerrainLevel::Background:
                          info.level = TerrainLevel::Background;
                          m_bg->Set(x, y, info);
                          break;
                        default:
                          break;
                      }
                    });
  
  m_ground->ForEach([&](const int& x, const int& y, const TerrainInfo& value)
                    {
                      TerrainInfo info = value;
                      Analize(m_ground, x, y, info, TerrainLevel::Ground);
                      m_ground->Set(x, y, info);
                    });
  m_bg->ForEach([&](const int& x, const int& y, const TerrainInfo& value)
                    {
                      TerrainInfo info = value;
                      Analize(m_bg, x, y, info, TerrainLevel::Background);
                      m_bg->Set(x, y, info);
                    });
  m_foreground->ForEach([&](const int& x, const int& y, const TerrainInfo& value)
                    {
                      TerrainInfo info = value;
                      Analize(m_foreground, x, y, info, TerrainLevel::Foreground);
                      m_foreground->Set(x, y, info);
                    });
}

TerrainAnalizer::Result TerrainAnalizer::GetResult() const
{
  Result r;
  r.background = m_bg;
  r.foreground = m_foreground;
  r.ground = m_ground;
  
  return r;
}

TerrainAnalizer::LevelsPtr TerrainAnalizer::GetLevels() const
{
  return m_levels;
}

void TerrainAnalizer::Analize(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info, TerrainLevel level)
{
  if (info.level >= TerrainLevel::Background)
  {
    int nSquare = CountNeighborsSquare(buffer, x, y, info.level);
    int nDiamond = CountNeighborsDiamond(buffer, x, y, info.level);
    if (nSquare == 4)
    {
      info.pos = TerrainPos::Fill;
    }
    else if (nSquare == 2)
    {
      info.pos = TerrainPos::Corner;
      AnalizeCorner(buffer, x, y, info);
    }
    else if (nSquare == 3 && nDiamond == 2)
    {
      info.pos = TerrainPos::Border;
      AnalizeBorder(buffer, x, y, info);
    }
    else if (nSquare == 3 && nDiamond == 3)
    {
      info.pos = TerrainPos::BorderToCornerLeft;
      AnalizeBorderToCorner(buffer, x, y, info);
    }
    info.resultLevel = info.level;
  }
  else
  {
    int nSquare = CountNeighborsSquare(buffer, x, y, level);
    int nDiamond = CountNeighborsDiamond(buffer, x, y, level);
    
    if (nSquare == 2 && nDiamond == 3)
    {
      info.pos = TerrainPos::InnnerCornter;
      info.resultLevel = level;
      AnalizeInnterCorner(buffer, x, y, info);
    }
    else
    {
      info.resultLevel = info.level;
    }
  }
}

int TerrainAnalizer::CountNeighbors(TerrainInfoBuffer buffer, int x, int y, TerrainLevel level) const
{
  TerrainInfo value;
  int count = 0;
  for (int i = -1; i < 2; i++)
  {
    for (int j = -1; j < 2; j++)
    {
      if (i != 0 && j != 0 && buffer->Get(x + i, y + j, value))
      {
        if (value.level >= level)
        {
          count += 1;
        }
      }
    }
  }
  
  return count;
}

int TerrainAnalizer::CountNeighborsSquare(TerrainInfoBuffer buffer, int x, int y, TerrainLevel level) const
{
  TerrainInfo value;
  int count = 0;
  if (GetInfo(buffer, x + 1, y, value, level) && value.level >= level) count += 1;
  if (GetInfo(buffer, x, y + 1, value, level) && value.level >= level) count += 1;
  if (GetInfo(buffer, x - 1, y, value, level) && value.level >= level) count += 1;
  if (GetInfo(buffer, x, y - 1, value, level) && value.level >= level) count += 1;
  return count;
}

int TerrainAnalizer::CountNeighborsDiamond(TerrainInfoBuffer buffer, int x, int y, TerrainLevel level) const
{
  TerrainInfo value;
  int count = 0;
  if (GetInfo(buffer, x + 1, y + 1, value, level) && value.level >= level) count += 1;
  if (GetInfo(buffer, x - 1, y - 1, value, level) && value.level >= level) count += 1;
  if (GetInfo(buffer, x + 1, y - 1, value, level) && value.level >= level) count += 1;
  if (GetInfo(buffer, x - 1, y + 1, value, level) && value.level >= level) count += 1;
  
  return count;
}

void TerrainAnalizer::AnalizeCorner(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info)
{
  TerrainInfo value;
  if (buffer->Get(x, y - 1, value) && value.level >= info.level) // left of right
  {
    if (buffer->Get(x + 1, y, value) && value.level >= info.level)
      info.rotation = TerrainRotation::Zero;
    else
      info.rotation = TerrainRotation::PI_2;
  }
  else
  {
    if (buffer->Get(x + 1, y, value) && value.level >= info.level)
      info.rotation = TerrainRotation::PI_3_2;
    else
      info.rotation = TerrainRotation::PI;
  }
}

void TerrainAnalizer::AnalizeBorder(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info)
{
  TerrainInfo value;
  if (buffer->Get(x, y + 1, value) && value.level < info.level)
    info.rotation = TerrainRotation::Zero;
  else if (buffer->Get(x, y - 1, value) && value.level < info.level)
    info.rotation = TerrainRotation::PI;
  else if (buffer->Get(x + 1, y, value) && value.level < info.level)
    info.rotation = TerrainRotation::PI_2;
  else if (buffer->Get(x - 1, y, value) && value.level < info.level)
    info.rotation = TerrainRotation::PI_3_2;
}

void TerrainAnalizer::AnalizeBorderToCorner(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info)
{
  TerrainInfo value;
  if (buffer->Get(x, y + 1, value) && value.level < info.level)
  {
    if (buffer->Get(x - 1, y + 1, value) && value.level < info.level)
    {
      info.rotation = TerrainRotation::Zero;
      info.pos = TerrainPos::BorderToCornerLeft;
    }
    else
    {
      info.rotation = TerrainRotation::Zero;
      info.pos = TerrainPos::BorderToCornerRight;
    }
  }
  else if (buffer->Get(x, y - 1, value) && value.level < info.level)
  {
    if (buffer->Get(x + 1, y - 1, value) && value.level < info.level)
    {
      info.rotation = TerrainRotation::PI;
      info.pos = TerrainPos::BorderToCornerLeft;
    }
    else
    {
      info.rotation = TerrainRotation::PI;
      info.pos = TerrainPos::BorderToCornerRight;
    }
  }
  else if (buffer->Get(x + 1, y, value) && value.level < info.level)
  {
    if (buffer->Get(x + 1, y + 1, value) && value.level < info.level)
    {
      info.rotation = TerrainRotation::PI_2;
      info.pos = TerrainPos::BorderToCornerLeft;
    }
    else
    {
      info.rotation = TerrainRotation::PI_2;
      info.pos = TerrainPos::BorderToCornerRight;
    }
  }
  else if (buffer->Get(x - 1, y, value) && value.level < info.level)
  {
    if (buffer->Get(x - 1, y - 1, value) && value.level < info.level)
    {
      info.rotation = TerrainRotation::PI_3_2;
      info.pos = TerrainPos::BorderToCornerLeft;
    }
    else
    {
      info.rotation = TerrainRotation::PI_3_2;
      info.pos = TerrainPos::BorderToCornerRight;
    }
  }
}

void TerrainAnalizer::AnalizeInnterCorner(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info)
{
  TerrainInfo value;
  if (buffer->Get(x - 1, y + 1, value) && value.level < info.resultLevel)
    info.rotation = TerrainRotation::Zero;
  else if (buffer->Get(x + 1, y + 1, value) && value.level < info.resultLevel)
    info.rotation = TerrainRotation::PI_2;
  else if (buffer->Get(x + 1, y - 1, value) && value.level < info.resultLevel)
    info.rotation = TerrainRotation::PI;
  else if (buffer->Get(x - 1, y - 1, value) && value.level < info.resultLevel)
    info.rotation = TerrainRotation::PI_3_2;
}

bool TerrainAnalizer::GetInfo(TerrainInfoBuffer buffer, int x, int y, TerrainInfo& info, TerrainLevel defaultLevel) const
{
  if (buffer->Get(x, y, info))
  {
    return true;
  }
  
  info.level = defaultLevel;
  return true;
}

