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
  
  cocos2d::Rect OffsetForType(komorki::PixelDescriptor* pd);
  
  cocos2d::Sprite* CreateSprite();
  void RemoveSprite(cocos2d::Sprite* sprite);
  cocos2d::Sprite* spriteForDescriptor(komorki::PixelDescriptor* pixelD);
  
  cocos2d::Vec2 spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector = cocos2d::Vec2());
  bool init();
  void Reset();
  void HightlightCellOnPos(int x, int y, komorki::CellType type);
  void StopHightlighting();
  void SetUpdateTime(float updateTime);
  
  void Delete(PartialMap::Context* context);
  void AddSprite(PixelDescriptor* pd, const Vec2& pos);
  void AddCreature(CellDescriptor* cd, PartialMap::Context* context);
  void AdoptSprite(PartialMap::Context* context);
  void MoveCreature(PartialMap::Context* context, const Vec2& source, const Vec2& dest);
  void Attack(PartialMap::Context* context, const Vec2& pos, const Vec2& offset);
  void StopSmallAnimations();
  void StartSmallAnimations();
 
private:
  
  void PlaySmallAnimation(cocos2d::Sprite* sprite);
  
  cocos2d::Sprite* m_hlSprite;
  std::list<cocos2d::Sprite*> m_spritesPull;
  unsigned int m_pullSize;
  float m_updateTime;
  bool m_enableSmallAnimations;
};
}
}

#endif /* defined(__komorki__PixelMapPartial__) */
