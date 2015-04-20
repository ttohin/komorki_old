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

static float s_scrollPosition = 0;

class ListController : public cocos2d::Node, public IFullScreenMenu
{
  const float kScrollStep = 40;
  const float kScrollTimer = 0.1;
  
public:

  ListController()
  {
  }
  virtual ~ListController(){}
  virtual void ShowInView(cocos2d::Node* root) {}
  virtual void Hide()
  {
    m_scrollDirection = 0;
    TimerForScroll(kScrollTimer);
    this->unschedule(CC_SCHEDULE_SELECTOR(ListController::TimerForScroll));
  }
  
  virtual void Resize(const cocos2d::Size& size) {}
  virtual void OnMouseScroll(cocos2d::Event* event)
  {
    if (m_list == nullptr) return;
    
    cocos2d::EventMouse* mouseEvent = dynamic_cast<cocos2d::EventMouse*>(event);
    float scrollY = mouseEvent->getScrollY();
    
    float scrollValue = 0;
    if (scrollY > 0)
      scrollValue = scrollY;
    else
      scrollValue = scrollY;
    
    m_scrollDirection += scrollValue;
    TimerForScroll(kScrollTimer);
    m_scrollDirection += -scrollValue;
  }
  
  virtual void OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
  {
    if (m_list == nullptr) return;
   
    if(keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
      m_scrollDirection += 1.0;
      this->unschedule(CC_SCHEDULE_SELECTOR(ListController::TimerForScroll));
    }
    else if(keyCode == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
      m_scrollDirection -= 1.0;
      this->unschedule(CC_SCHEDULE_SELECTOR(ListController::TimerForScroll));
    }
  }
  
  virtual void OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
  {
    if (m_list == nullptr) return;
    
    if(keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
      m_scrollDirection -= 1.0;
      this->schedule(CC_SCHEDULE_SELECTOR(ListController::TimerForScroll), kScrollTimer, kRepeatForever, 0);
    }
    else if(keyCode == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
      m_scrollDirection += 1.0;
      this->schedule(CC_SCHEDULE_SELECTOR(ListController::TimerForScroll), kScrollTimer, kRepeatForever, 0);
    }
  }

  void TimerForScroll(float updateTime)
  {
    float minY  = m_list->getContentSize().height - m_list->getInnerContainerSize().height;
    float h = -minY;
    float prevPercent = 100.0 * (m_list->getInnerContainer()->getPosition().y - minY) / h;
    float percent = 100.0 * (m_list->getInnerContainer()->getPosition().y + kScrollStep * m_scrollDirection - minY) / h;
    s_scrollPosition = percent;
    
    if ((prevPercent <= 0 && m_scrollDirection < 0) ||
        (prevPercent >= 100 && m_scrollDirection > 0))
    {
      s_scrollPosition = prevPercent;
      return;
    }
    
    percent = std::min(110.0f, percent);
    percent = std::max(-10.0f, percent);
    
    s_scrollPosition = percent;
    m_list->scrollToPercentVertical(percent, updateTime, 0);
  }
  
  void ScrollToPosition(float updateTime)
  {
    m_list->scrollToPercentVertical(s_scrollPosition, 0.01, 0);
  }
  
protected:
  void SetListView(cocos2d::ui::ListView* list) {m_list = list;}
private:
  float m_scrollDirection;
  cocos2d::ui::ListView* m_list;
};

#endif
