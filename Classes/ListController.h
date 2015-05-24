//
//  ListController.h
//  prsv
//
//  Created by Anton Simakov on 20.04.15.
//
//

#ifndef prsv_ListController_h
#define prsv_ListController_h

#include "cocos2d.h"
#include "CocosGUI.h"
#include "IFullScreenMenu.h"
#include "ListView.h"

class ListController : public cocos2d::Node, public IFullScreenMenu
{  
public:

  ListController();
  virtual ~ListController();
  virtual void ShowInView(cocos2d::Node* root);
  virtual void Hide();
  
  virtual void Resize(const cocos2d::Size& size);
  virtual void OnMouseScroll(cocos2d::Event* event);
  virtual void OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
  virtual void OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
  void TimerForScroll(float updateTime);
  
  void ScrollToPosition(float updateTime);
  void ConfigureConfirmationPanel();
  
  // callbacks
  virtual void OnLeftConfirmationButtonPressed() {}
  virtual void OnRightConfirmationButtonPressed() {}
  
protected:
  float m_scrollDirection;
  komorki::ListView* m_list;
  cocos2d::ui::Layout* m_confirmationPanel;
  cocos2d::ui::Button* m_ok;
  cocos2d::ui::Button* m_cancel;
  cocos2d::ui::Text* m_title;
};

#endif
