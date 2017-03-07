//
//  AmorphCellContext.hpp
//  Komorki
//
//  Created by user on 02.03.17.
//
//

#ifndef AmorphCellContext_hpp
#define AmorphCellContext_hpp

#include "Common.h"
#include "ObjectContext.h"

namespace komorki
{
  namespace graphic
  {
    class AmorphCellContext : public ObjectContext
    {
      struct PolymorphShapeContext
      {
        typedef std::shared_ptr<PolymorphShapeContext> Ptr;
        cocos2d::Sprite* sprite = nullptr;
        cocos2d::Sprite* centerSprite = nullptr;
        Vec2 originalPos;
        Vec2 targetPos;
        Vec2 prevPos;
        bool animationPlayed = false;
        bool fade = false;
        Morph::MorphDir direction;
        cocos2d::Vec2 offset;
        bool useCenterSprite = true;
      };
      
      typedef std::unordered_map<Vec2, PolymorphShapeContext::Ptr, Vec2Hasher> SpriteMap;
      typedef std::list<PolymorphShapeContext::Ptr> SpriteList;
      
    public:
      
      Vec2 m_pos;
      SpriteMap m_spriteMap;
      SpriteList m_spritesPull;
      cocos2d::Vec2 m_offset;
      cocos2d::Rect m_textureRect;
      Rect m_aabb;
      
      AmorphCellContext(PartialMapPtr _owner,
                        const cocos2d::Rect& textureRect,
                        CellDescriptor* cell);
      
      void AddSprite(const Rect& aabb, Vec2ConstRef pos);
      
      void MoveAmorphCells(Vec2ConstRef source,
                           const komorki::Morphing& morph,
                           const Rect& aabb,
                           float animationDuration);
      void AnimatePart(PolymorphShapeContext::Ptr& context,
                       const Rect& aabb,
                       float animationDuration);
      
      PolymorphShapeContext::Ptr CreateContext(const Rect& aabb,
                                               Vec2ConstRef originalPos);
      PolymorphShapeContext::Ptr PopSprite(Vec2ConstRef pos);
      PolymorphShapeContext::Ptr GetSprite(Vec2ConstRef pos);
      void SetSprite(PolymorphShapeContext::Ptr, Vec2ConstRef pos);
      
      cocos2d::Sprite* CreateSprite();
      void RemoveSprite(cocos2d::Sprite* sprite);
      cocos2d::Vec2 GetCenterPos() const;
      
      virtual ContextType GetType() const override { return ContextType::ManyPixels; }
      virtual void BecomeOwner(PartialMapPtr _owner) override;
      virtual void Destory(PartialMapPtr _owner) override;
      virtual void Attack(const Vec2& pos, const Vec2& offset, float animationTime) override;
      virtual void ToggleAnimation() override;
      virtual void CellDead() override;
    };
  }
}

#endif /* AmorphCellContext_hpp */
