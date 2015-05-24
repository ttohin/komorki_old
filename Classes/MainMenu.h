//
//  MainMenu.h
//  komorki
//
//  Created by ttohin on 15.04.15.
//
//

#ifndef __komorki__MainMenu__
#define __komorki__MainMenu__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "PixelDescriptorProvider.h"
#include "IFullScreenMenu.h"

class MainMenu : public IFullScreenMenu
{
public:
  
  typedef std::function<void(void)> MainMenuCallback;
 
  MainMenu();
  void Init(const MainMenuCallback& optionCallback,
            const MainMenuCallback& exitCallback,
            const MainMenuCallback& backCallback);
  
  virtual ~MainMenu();
  
  // IFullScreenMenu
  virtual void ShowInView(cocos2d::Node* root);
  virtual void Hide();
  virtual void Resize(const cocos2d::Size& size);
  virtual void OnMouseScroll(cocos2d::Event* event);
  virtual void OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
  virtual void OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
  
private:

  cocos2d::Node* m_rootNode;
  cocos2d::ui::Layout* m_bg;
  cocos2d::ui::Layout* m_mainMenu;
  cocos2d::Node* m_titleNode;
  MainMenuCallback m_optionCallback;
  MainMenuCallback m_exitCallback;
  MainMenuCallback m_backCallback;
};

#endif /* defined(__komorki__MainMenu__) */
