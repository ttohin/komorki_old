//
//  AmorphCellContext.hpp
//  Komorki
//
//  Created by user on 02.03.17.
//
//

#ifndef AmorphCellContext_hpp
#define AmorphCellContext_hpp

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
        Vec2 originalPos;
        Vec2 targetPos;
        Vec2 prevPos;
        bool animationPlayed = false;
        bool fade = false;
        Morph::MorphDir direction;
        cocos2d::Vec2 offset;
      };
      
      typedef std::unordered_map<std::string, PolymorphShapeContext::Ptr> SpriteMap;
      typedef std::list<PolymorphShapeContext::Ptr> SpriteList;
      
    public:
      
      Vec2 m_pos;
      SpriteMap m_spriteMap;
      SpriteList m_spritesPull;
      cocos2d::Vec2 m_offset;
      cocos2d::Sprite* m_centerSprite;
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
      void AnimatePart(PolymorphShapeContext::Ptr& context, const Rect& aabb, float animationDuration);
      
      PolymorphShapeContext::Ptr CreateContext(const Rect& aabb,
                                               Vec2ConstRef originalPos);
      PolymorphShapeContext::Ptr PopSprite(int x, int y);
      PolymorphShapeContext::Ptr GetSprite(int x, int y);
      void SetSprite(PolymorphShapeContext::Ptr, int x, int y);
      std::string GetKey(int x, int y) const;
      
      cocos2d::Sprite* CreateSprite();
      void RemoveSprite(cocos2d::Sprite* sprite);
      cocos2d::Vec2 GetCenterPos() const;
      
      virtual ContextType GetType() const override { return ContextType::ManyPixels; }
      virtual void BecomeOwner(PartialMapPtr _owner) override;
      virtual void Destory(PartialMapPtr _owner) override;
      virtual void Attack(const Vec2& pos, const Vec2& offset, float animationTime) override;
      virtual void CellDead() override;
    };
  }
}

#endif /* AmorphCellContext_hpp */
