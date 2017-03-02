//
//  PixelMapContext.h
//  prsv
//
//  Created by Anton Simakov on 09.07.16.
//
//

#ifndef __prsv__PixelMapContext__
#define __prsv__PixelMapContext__

#include "cocos2d.h"
#include "Common.h"
#include "IPixelWorld.h"

namespace komorki
{
  namespace graphic
  {
    class PartialMap;
    
    enum class ContextType
    {
      Unknown,
      SinglePixel,
      ManyPixels,
    };
    
    class ObjectContext
    {
    public:
      ObjectContext(PartialMap *_owner);
      virtual ~ObjectContext();
      
      Vec2 GetPosInOwnerBase(Vec2ConstRef pos) const;
      std::string Description() const;
      
      virtual ContextType GetType() const { return ContextType::Unknown; }
      virtual void BecomeOwner(PartialMap* _owner) = 0;
      virtual void Destory(PartialMap* _owner) = 0;
      virtual void Free(PartialMap* _owner);
      virtual void ForceDestory(PartialMap* _owner);
      virtual void Attack(const Vec2& pos, const Vec2& offset, float animationTime) {}
      virtual void EnableSmallAnimations(bool enable) {}
      
      PartialMap* m_owner;
      
      constexpr static const float kMaxAmorpghCellOpacity = 200;
      constexpr static const float kCenterAmorpghCellOpacity = 200;
      constexpr static const float kCenterCellScale = 0.6;
    };
  }
  
}

#endif /* defined(__prsv__PixelMapContext__) */
