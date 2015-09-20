//
//  DiamondSquareGenerator.cpp
//  prsv
//
//  Created by ttohin on 16.08.15.
//
//

#include "DiamondSquareGenerator.h"

using namespace komorki;

DiamondSquareGenerator::DiamondSquareGenerator(int width, int height, float roughness,  float randDelta, bool enableOverflow)
: m_width(width)
, m_height(height)
, m_roughness(roughness)
, m_randDelta(randDelta)
, m_overflow(enableOverflow)
{
}

bool DiamondSquareGenerator::GetValue(int x, int y, float& value) const
{
  if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    return false;
  else
    value = GetValue(x, y);
  
  return true;
}

void DiamondSquareGenerator::Generate(const OnValue& onValue)
{
  m_data.reserve(m_width * m_height);
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      SetValue(i, j, 0);
    }
  }
  
  for (int l = std::floorf((float)m_width/1.f); l > 0; l = std::floorf((float)l/2.f))
  {
    for (int i = 0; i < m_width; i += l)
    {
      for (int j = 0; j < m_height; j += l)
      {
        SquareDiamond(i, j, l, l);
      }
    }
  }
  
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      float value = GetValue(i, j);
      if (onValue) onValue(i, j, value);
      SetValue(i, j, value);
    }
  }
}

void DiamondSquareGenerator::ForEach(const OnValue& onValue)
{
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      float value = GetValue(i, j);
      if (onValue) onValue(i, j, value);
      SetValue(i, j, value);
    }
  }
}

void DiamondSquareGenerator::Step(const OnDiff& onDiff, int value, float roughness)
{
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      float prevValue = GetValue(i, j);
      float result = Displace(prevValue, value, roughness);
      float diff = result - prevValue;
      if (onDiff) onDiff(i, j, prevValue, diff);
      SetValue(i, j, prevValue + diff);
    }
  }
}

void DiamondSquareGenerator::Add(DiamondSquareGenerator* other, const OnDiff& onDiff)
{
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      float prevValue = GetValue(i, j);
      float result = prevValue + other->GetValue(i, j);
      float diff = result - prevValue;
      if (onDiff) onDiff(i, j, prevValue, diff);
      SetValue(i, j, prevValue + diff);
    }
  }
}

void DiamondSquareGenerator::Multiply(DiamondSquareGenerator* other, const OnDiff& onDiff)
{
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      float prevValue = GetValue(i, j);
      float result = prevValue * other->GetValue(i, j);
      float diff = result - prevValue;
      if (onDiff) onDiff(i, j, prevValue, diff);
      SetValue(i, j, prevValue + diff);
    }
  }
}

void DiamondSquareGenerator::Add(float diff)
{
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      SetValue(i, j, GetValue(i, j) + diff);
    }
  }
}

void DiamondSquareGenerator::Multiply(float value)
{
  for (int i = 0; i < m_width; ++i)
  {
    for (int j = 0; j < m_height; ++j)
    {
      SetValue(i, j, GetValue(i, j) * value);
    }
  }
}

Buffer2DPtr<float> DiamondSquareGenerator::GetBuffer(int x, int y, int width, int height) const
{
  if (x < 0 || x + width > m_width || y < 0 || y + height > m_height)
    return nullptr;
 
  auto result = std::make_shared<Buffer2D<float>>(width, height);
  
  for (int i = x; i < x + width; ++i)
  {
    for (int j = y; j < y + height; ++j)
    {
      result->Set(i - x, j - y, GetValue(i, j));
    }
  }
  
  return result;
}

void DiamondSquareGenerator::SquareDiamond(int x, int y, int xBlockSize, int yBlockSize)
{
  int xBlockMiddele = std::floorf((float)xBlockSize / 2.0f);
  int yBlockMiddele = std::floorf((float)yBlockSize / 2.0f);
  
  Square(x + xBlockMiddele, y + yBlockMiddele, xBlockMiddele, yBlockMiddele);
  Diamond(x , y + yBlockMiddele, xBlockMiddele, yBlockMiddele);
  Diamond(x + xBlockMiddele, y, xBlockMiddele, yBlockMiddele);
  Diamond(x + xBlockSize, y + yBlockMiddele, xBlockMiddele, yBlockMiddele);
  Diamond(x + xBlockMiddele, y + yBlockSize, xBlockMiddele, yBlockMiddele);
}

float DiamondSquareGenerator::Displace(float value, int blockSize, float roughness) const
{
  return value + ((float)rand()/(float)RAND_MAX + m_randDelta) * 2 * blockSize / m_width * roughness;
}

bool DiamondSquareGenerator::SetValue(int x, int y, float value)
{
  if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    return false;
 
  if (m_overflow)
  {
    m_data[x + y*m_width] = value;
  }
  else
  {
    m_data[x + y*m_width] = std::max(0.0f, std::min(1.0f, value));
  }
  
  return true;
}

float DiamondSquareGenerator::GetValue(int x, int y) const
{
  return m_data[x + y*m_width];
}

void DiamondSquareGenerator::Square(int x, int y, int xBlockSize, int yBlockSize)
{
  float numberOfValus = 0;
  float valueAccumulator = 0;
  float value = 0;
  if (GetValue(x - xBlockSize, y - yBlockSize, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
  if (GetValue(x + xBlockSize, y - yBlockSize, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
  if (GetValue(x - xBlockSize, y + yBlockSize, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
  if (GetValue(x + xBlockSize, y + yBlockSize, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
 
  float result = Displace(valueAccumulator / numberOfValus, xBlockSize + yBlockSize, m_roughness);
  SetValue(x, y, result);
  
}

void DiamondSquareGenerator::Diamond(int x, int y, int xBlockSize, int yBlockSize)
{
  float numberOfValus = 0;
  float valueAccumulator = 0;
  float value = 0;
  if (GetValue(x - xBlockSize, y, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
  if (GetValue(x + xBlockSize, y, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
  if (GetValue(x, y - yBlockSize, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
  if (GetValue(x, y + yBlockSize, value))
  {
    valueAccumulator += value;
    numberOfValus += 1;
  }
  
  float result = Displace(valueAccumulator / numberOfValus, xBlockSize + yBlockSize, m_roughness);
  SetValue(x, y, result);
}
