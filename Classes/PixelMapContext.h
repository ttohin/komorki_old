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
#include "IPixelDescriptorProvider.h"

namespace komorki
{
  namespace ui
  {
    class PartialMap;
    
    namespace PixelMap
    {
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
      };
      
      class SingleCellContext : public ObjectContext
      {
      public:
        

        SingleCellContext(PartialMap *_owner,
                          const cocos2d::Rect& textureRect,
                          Vec2ConstRef origin,
                          const Rect& rect);
        
        void Move(Vec2ConstRef src, Vec2ConstRef dest, float animationDuration);
        void ChangeRect(CellDescriptor* cd, const Rect& newRect, float animationDuration);
        
        virtual ContextType GetType() const override { return ContextType::SinglePixel; }
        virtual void BecomeOwner(PartialMap* _owner) override;
        virtual void Destory(PartialMap* _owner) override;
        virtual void Attack(const Vec2& pos, const Vec2& offset, float animationTime) override;
        virtual void EnableSmallAnimations(bool enable) override;
        
        cocos2d::Sprite* m_sprite = nullptr;
        cocos2d::Sprite* m_glow = nullptr;
        cocos2d::Vec2 m_offset;
        Vec2 m_pos;
        Vec2 m_posOffset; // for rect shapes
        Vec2 m_size; // for rect shapes
        cocos2d::Rect m_textureRect;
      
      };
      
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
        
        AmorphCellContext(PartialMap *owner, const cocos2d::Rect& textureRect);
        
        void AddSprite(const Rect& aabb, Vec2ConstRef pos);
        
        void MoveAmorphCells(Vec2ConstRef source,
                             komorki::Morphing& morph,
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
        virtual void BecomeOwner(PartialMap* _owner) override;
        virtual void Destory(PartialMap* _owner) override;
        virtual void Attack(const Vec2& pos, const Vec2& offset, float animationTime) override;
      };
    }
  }
}

#endif /* defined(__prsv__PixelMapContext__) */
