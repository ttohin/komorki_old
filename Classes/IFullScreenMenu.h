//
//  IFullScreenMenu.h
//  komorki
//
//  Created by ttohin on 19.04.15.
//
//

#ifndef komorki_IFullScreenMenu_h
#define komorki_IFullScreenMenu_h

#include "cocos2d.h"

class IFullScreenMenu
{
public:
  virtual ~IFullScreenMenu() {}
 
  virtual void ShowInView(cocos2d::Node* root) = 0;
  virtual void Hide() = 0;
  virtual void Resize(const cocos2d::Size& size) = 0;
  virtual void OnMouseScroll(cocos2d::Event* event) = 0;
  virtual void OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) = 0;
  virtual void OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) = 0;
};

#endif
