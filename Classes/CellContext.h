//
//  CellContext.hpp
//  Komorki
//
//  Created by user on 02.03.17.
//
//

#pragma once

#include "ObjectContext.h"

namespace komorki
{
  namespace graphic
  {
    class CellContext : public ObjectContext
    {
    public:
      CellContext(PartialMapPtr _owner,
                  const cocos2d::Rect& textureRect,
                  Vec2ConstRef origin,
                  const Rect& rect,
                  CellDescriptor* cell);
      
      void Move(Vec2ConstRef src, Vec2ConstRef dest, float animationDuration);
      void ChangeRect(CellDescriptor* cd, const Rect& newRect, float animationDuration);
      
      virtual ContextType GetType() const override { return ContextType::SinglePixel; }
      virtual void BecomeOwner(PartialMapPtr _owner) override;
      virtual void Destory(PartialMapPtr _owner) override;
      virtual void Attack(const Vec2& pos, const Vec2& offset, float animationTime) override;
      virtual void EnableSmallAnimations(bool enable) override;
      virtual void CellDead() override;
      
      cocos2d::Sprite* m_sprite = nullptr;
      cocos2d::Sprite* m_glow = nullptr;
      cocos2d::Vec2 m_offset;
      Vec2 m_pos;
      Vec2 m_posOffset; // for rect shapes
      Vec2 m_size; // for rect shapes
      cocos2d::Rect m_textureRect;
      
    };
  }
}


