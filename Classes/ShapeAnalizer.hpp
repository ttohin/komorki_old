//
//  ShapeAnalizer.hpp
//  Komorki
//
//  Created by Anton Simakov on 20/10/2016.
//
//

#ifndef ShapeAnalizer_hpp
#define ShapeAnalizer_hpp

#include "Buffer2D.h"

namespace komorki
{
  class ShapeAnalizer
  {
  public:
    enum class PartType
    {
      Empty,
      Corner,
      Border,
      BorderTopToCorner,
      BorderBottomToCorner,
      BorderLeftToCorner,
      BorderRightToCorner,
      Bridge,
      Fill,
      InnnerCornter
    };
    
    enum class PartPosition
    {
      Top,
      Left,
      Bottom,
      Right,
      Center,
      TopLeft,
      TopCenter,
      TopRight,
      CenterRight,
      BottomRight,
      BottonCenter,
      BottomLeft,
      CenterLeft
    };
    
    struct Part
    {
      PartType type = PartType::Empty;
      PartPosition pos = PartPosition::Center;
    };
    
  public:
    
    using ResultBuffer = Buffer2DPtr<Part>;
    using InternalBuffer = Buffer2DPtr<bool>;
    
    ShapeAnalizer(const Buffer2DPtr<bool>& buffer, unsigned int scale);
    
  private:
    
    int CountNeighborsSquare(int x, int y) const;
    int CountNeighborsDiamond(int x, int y) const;
    
    void AnalizeCorner(int x, int y, Part& result);
    void AnalizeBorder(int x, int y, Part& result);
    void AnalizeBridge(int x, int y, Part& result);
    void AnalizeBorderToCorner(int x, int y, Part& result);
    void AnalizeInnterCorner(int x, int y, Part& result);
    
  public:
    ResultBuffer m_result;
    InternalBuffer m_buffer;
  };
}

#endif /* ShapeAnalizer_hpp */
