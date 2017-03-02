//
//  PixelMapPartial.h
//  komorki
//
//  Created by ttohin on 11.03.15.
//
//

#ifndef __komorki__PixelMapPartial__
#define __komorki__PixelMapPartial__

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "CellDescriptor.h"
#include "PartialMap.h"

namespace komorki
{
namespace ui
{
  
class PixelMapPartial : public cocos2d::SpriteBatchNode
{
public:
  
  PixelMapPartial();
  virtual ~PixelMapPartial();
  
  cocos2d::Sprite* CreateSprite();
  void RemoveSprite(cocos2d::Sprite* sprite);

  bool init();
  void Reset();
  void SetUpdateTime(float updateTime);
  
  void AddSprite(PixelDescriptor* pd, const Vec2& pos);
  void AddCreature(CellDescriptor* cd, PixelMap::SingleCellContext* context, Vec2 partialMapOffset);
  void AddPolymorphCreature(CellDescriptor* cd, PixelMap::AmorphCellContext* context, Vec2 partialMapOffset);
  void MoveCreature(PixelMap::ObjectContext* context, const Vec2& source, const Vec2& dest, int duration, Morphing& morphing, Vec2 partialMapOffset,CellDescriptor* cd);
  
private:
  
  cocos2d::Sprite* m_hlSprite;
  std::list<cocos2d::Sprite*> m_spritesPull;
  unsigned int m_pullSize;
  float m_updateTime;
  
};
}
}

#endif /* defined(__komorki__PixelMapPartial__) */
