//
//  PixelMapPartial.h
//  komorki
//
//  Created by ttohin on 11.03.15.
//
//

#ifndef __komorki__PixelDebugView__
#define __komorki__PixelDebugView__

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "PixelMapPartial.h"
#include "CellDescriptor.h"
#include "Common.h"

namespace komorki
{
namespace ui
{

class PixelDebugView : public cocos2d::SpriteBatchNode
{
public:
  
  PixelDebugView(int a, int b, int width, int height, komorki::PixelDescriptorProvider* provider);
  
  cocos2d::Sprite* CreateSprite();
  
  void RemoveSprite(cocos2d::Sprite* sprite);
  cocos2d::Sprite* ColorSprite(int i);
  
  cocos2d::Sprite* HealthSprite(komorki::PixelDescriptor* pixelD);
  
  bool IsInAABB(const Vec2& vec);
  bool IsInAABB(const int& x, const int& y);
  cocos2d::Vec2 spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector = cocos2d::Vec2());
  cocos2d::Vec2 spriteVector(komorki::PixelDescriptor* pd, const cocos2d::Vec2& vector = cocos2d::Vec2());
  void Update(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult, float updateTime);
  bool init();
private:
  void Reset();
  int m_a1;
  int m_b1;
  int m_width;
  int m_height;
  int m_a2;
  int m_b2;
  std::list<cocos2d::Sprite*> m_spritesPull;
  unsigned int m_pullSize;
  komorki::PixelDescriptorProvider* m_provider;
};
}
}

#endif /* defined(__komorki__PixelDebugView__) */
