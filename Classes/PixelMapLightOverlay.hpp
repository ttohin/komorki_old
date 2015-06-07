//
//  PixelMapLightOverlay.hpp
//  prsv
//
//  Created by Anton Simakov on 30.05.15.
//
//

#ifndef prsv_PixelMapLightOverlay_hpp
#define prsv_PixelMapLightOverlay_hpp

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "CellDescriptor.h"

namespace komorki
{
namespace ui
{
class PixelMapLightOverlay : public cocos2d::SpriteBatchNode
{
public:
  PixelMapLightOverlay(int a, int b, int width, int height, komorki::PixelDescriptorProvider* provider);
  cocos2d::Sprite* CreateSprite();
  
  void RemoveSprite(cocos2d::Sprite* sprite);
  
  cocos2d::Sprite* SpriteForLight(int value);

  bool IsInAABB(const Vec2& vec);
  
  bool IsInAABB(const int& x, const int& y);
  cocos2d::Vec2 spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector = cocos2d::Vec2());
  bool init();
  
  void Reset();
 
private:
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

#endif
