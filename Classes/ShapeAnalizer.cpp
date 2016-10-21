//
//  ShapeAnalizer.cpp
//  Komorki
//
//  Created by Anton Simakov on 20/10/2016.
//
//

#include "ShapeAnalizer.hpp"


namespace komorki
{
  ShapeAnalizer::ShapeAnalizer(const Buffer2DPtr<bool>& buffer, unsigned int scale)
  : m_result(new Buffer2D<Part>(buffer->GetWidth() * scale, buffer->GetHeight() * scale))
  , m_buffer(new Buffer2D<bool>(buffer->GetWidth() * scale, buffer->GetHeight() * scale))
  {
    
    buffer->Scale(scale, *m_buffer.get());
    
    m_buffer->ForEach([&](int x, int y, const bool& value){
      
      Part part;
      if (value == false)
      {
        int nSquare = CountNeighborsSquare(x, y);
        int nDiamond = CountNeighborsDiamond(x, y);
        
        if (nSquare == 2 && nDiamond == 2)
        {
          AnalizeInnterCorner(x, y, part);
        }
      }
      else
      {
        int nSquare = CountNeighborsSquare(x, y);
        int nDiamond = CountNeighborsDiamond(x, y);
        if (nSquare == 4)
        {
          part.type = PartType::Fill;
        }
        else if (nSquare == 2 && nDiamond == 1)
        {
          AnalizeCorner(x, y, part);
        }
        else if (nSquare == 3 && nDiamond == 2)
        {
          AnalizeBorder(x, y, part);
        }
        else if (nSquare == 3 && nDiamond == 3)
        {
          AnalizeBorderToCorner(x, y, part);
        }
        else if (nSquare == 2 && nDiamond == 2)
        {
          AnalizeBridge(x, y, part);
        }
      }
      
      if (part.type == PartType::Empty && value)
      {
        part.type = PartType::Fill;
      }
      
      m_result->Set(x, y, part);
    });
  }
  
  int ShapeAnalizer::CountNeighborsSquare(int x, int y) const
  {
    bool value;
    int count = 0;
    if (m_buffer->Get(x + 1, y, value) && value == true) count += 1;
    if (m_buffer->Get(x - 1, y, value) && value == true) count += 1;
    if (m_buffer->Get(x, y + 1, value) && value == true) count += 1;
    if (m_buffer->Get(x, y - 1, value) && value == true) count += 1;
    return count;
  }
  
  int ShapeAnalizer::CountNeighborsDiamond(int x, int y) const
  {
    bool value;
    int count = 0;
    if (m_buffer->Get(x + 1, y + 1, value) && value == true) count += 1;
    if (m_buffer->Get(x - 1, y - 1, value) && value == true) count += 1;
    if (m_buffer->Get(x - 1, y + 1, value) && value == true) count += 1;
    if (m_buffer->Get(x + 1, y - 1, value) && value == true) count += 1;
    return count;
    
    return count;
  }
  
  void ShapeAnalizer::AnalizeCorner(int x, int y, Part& result)
  {
    bool value;
    if (m_buffer->Get(x, y - 1, value) && value) // left of right
    {
      if (m_buffer->Get(x + 1, y, value) && value)
        result.pos = PartPosition::TopLeft;
      else
        result.pos = PartPosition::TopRight;
    }
    else
    {
      if (m_buffer->Get(x + 1, y, value) && value)
        result.pos = PartPosition::BottomLeft;
      else
        result.pos = PartPosition::BottomRight;
    }
    
    result.type = PartType::Corner;
  }
  
  void ShapeAnalizer::AnalizeBorder(int x, int y, Part& result)
  {
    bool value;
    if (!m_buffer->Get(x, y + 1, value) || value == false)
      result.pos = PartPosition::Top;
    else if (!m_buffer->Get(x, y - 1, value) || value == false)
      result.pos = PartPosition::Bottom;
    else if (!m_buffer->Get(x + 1, y, value) || value == false)
      result.pos = PartPosition::Right;
    else if (!m_buffer->Get(x - 1, y, value) || value == false)
      result.pos = PartPosition::Left;
    
    result.type = PartType::Border;
  }
  
  void ShapeAnalizer::AnalizeBridge(int x, int y, Part& result)
  {
    bool value;
    if ((!m_buffer->Get(x, y + 1, value) || value == false)
        && (!m_buffer->Get(x + 1, y, value) || value == false))
      result.pos = PartPosition::TopRight;
    if ((!m_buffer->Get(x, y - 1, value) || value == false)
        && (!m_buffer->Get(x - 1, y, value) || value == false))
      result.pos = PartPosition::BottomLeft;
    if ((!m_buffer->Get(x, y + 1, value) || value == false)
        && (!m_buffer->Get(x - 1, y, value) || value == false))
      result.pos = PartPosition::TopLeft;
    if ((!m_buffer->Get(x, y - 1, value) || value == false)
        && (!m_buffer->Get(x + 1, y, value) || value == false))
      result.pos = PartPosition::BottomRight;
    
    result.type = PartType::Bridge;
  }

  void ShapeAnalizer::AnalizeBorderToCorner(int x, int y, Part& result)
  {
    bool value;
    if (!m_buffer->Get(x, y + 1, value) || value == false)
    {
      if (!m_buffer->Get(x - 1, y + 1, value) || value == false)
      {
        result.pos = PartPosition::Right;
        result.type = PartType::BorderTopToCorner;
      }
      else
      {
        result.pos = PartPosition::Left;
        result.type = PartType::BorderTopToCorner;
      }
    }
    else if (!m_buffer->Get(x, y - 1, value)  || value == false)
    {
      if (!m_buffer->Get(x + 1, y - 1, value)  || value == false)
      {
        result.pos = PartPosition::Left;
        result.type = PartType::BorderBottomToCorner;
      }
      else
      {
        result.pos = PartPosition::Right;
        result.type = PartType::BorderBottomToCorner;
      }
    }
    else if (!m_buffer->Get(x + 1, y, value)  || value == false)
    {
      if (!m_buffer->Get(x + 1, y + 1, value)  || value == false)
      {
        result.pos = PartPosition::Bottom;
        result.type = PartType::BorderRightToCorner;
      }
      else
      {
        result.pos = PartPosition::Top;
        result.type = PartType::BorderRightToCorner;
      }
    }
    else if (!m_buffer->Get(x - 1, y, value)  || value == false)
    {
      if (!m_buffer->Get(x - 1, y - 1, value)  || value == false)
      {
        result.pos = PartPosition::Top;
        result.type = PartType::BorderLeftToCorner;
      }
      else
      {
        result.pos = PartPosition::Bottom;
        result.type = PartType::BorderLeftToCorner;
      }
    }
  }
  
  void ShapeAnalizer::AnalizeInnterCorner(int x, int y, Part& result)
  {
    bool value;
    if (m_buffer->Get(x, y - 1, value) && value == true
        && m_buffer->Get(x + 1, y, value) && value == true)
      result.pos = PartPosition::BottomRight;
    if (m_buffer->Get(x, y + 1, value) && value == true
        && m_buffer->Get(x - 1, y, value) && value == true)
      result.pos = PartPosition::TopLeft;
    if (m_buffer->Get(x, y - 1, value) && value == true
        && m_buffer->Get(x - 1, y, value) && value == true)
      result.pos = PartPosition::BottomLeft;
    if (m_buffer->Get(x, y + 1, value) && value == true
        && m_buffer->Get(x + 1, y, value) && value == true)
      result.pos = PartPosition::TopRight;
    
    result.type = PartType::InnnerCornter;
  }

}
