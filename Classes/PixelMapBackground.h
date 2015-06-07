//
//  PixelMapPartial.h
//  komorki
//
//  Created by ttohin on 11.03.15.
//
//

#ifndef __komorki__PixelMapBackground__
#define __komorki__PixelMapBackground__

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "PixelMapPartial.h"
#include "CellDescriptor.h"


namespace komorki
{
namespace ui
{
class PixelMapBackground : public cocos2d::SpriteBatchNode
{
public:
  
  PixelMapBackground(int a, int b, int width, int height);
  cocos2d::Sprite* CreateSprite();
  void RemoveSprite(cocos2d::Sprite* sprite);
  cocos2d::Sprite* spriteDeadCell(komorki::CellDescriptor* cd);
  bool IsInAABB(const Vec2& vec);
  bool IsInAABB(const int& x, const int& y);
  cocos2d::Vec2 spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector = cocos2d::Vec2());
  cocos2d::Vec2 spriteVector(komorki::PixelDescriptor* pd, const cocos2d::Vec2& vector = cocos2d::Vec2());
  void Update(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult, float updateTime);
  bool init();
  void Reset();
  
  int m_a1;
  int m_b1;
  int m_width;
  int m_height;
  int m_a2;
  int m_b2;
  std::list<cocos2d::Sprite*> m_spritesPull;
  unsigned int m_pullSize;
};
}
}

#endif /* defined(__komorki__PixelMapBackground__) */
