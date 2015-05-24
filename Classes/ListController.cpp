//
//  ListController.cpp
//  prsv
//
//  Created by Anton Simakov on 20.04.15.
//
//


#include "ListController.h"
#include "ConfigManager.h"

USING_NS_CC;

#define SAFE_CALL(name) if(name) name

namespace
{
  static float s_scrollPosition = 0;
  const float kDefaultMargin = 2.0;
  const float kDefaultPanelSize = 40.0;
  const float kDefaultPanelWidth = 600;
  const float kDefaultTitleFontSize = 18;
  const float kDefaultValueFontSize = 12;
  float kMargin = kDefaultMargin;
  float kPanelSize = kDefaultPanelSize;
  float kPanelWidth = kDefaultPanelWidth;
  const float kScrollStep = 40;
  const float kScrollTimer = 0.1;
}

ListController::ListController()
{
}

ListController::~ListController()
{
}

void ListController::ShowInView(cocos2d::Node* root)
{
}

void ListController::Hide()
{
  m_scrollDirection = 0;
  TimerForScroll(kScrollTimer);
  this->unschedule(CC_SCHEDULE_SELECTOR(ListController::TimerForScroll));
}

void ListController::Resize(const cocos2d::Size& size)
{
  if(komorki::ConfigManager::GetInstance()->RequiredDoubleSize())
  {
    kPanelSize = kDefaultPanelSize * 2;
    kPanelWidth = kDefaultPanelWidth * 2;
    kMargin = kDefaultMargin * 2;
  }
  
  kPanelWidth = std::min(kPanelWidth, size.width);
  
  m_confirmationPanel->setContentSize({size.width, kPanelSize});
  m_confirmationPanel->setPosition({0, size.height - kPanelSize});
  
  SAFE_CALL(m_cancel)->setPosition({size.width/2.f - kPanelWidth/2.f + kMargin, kMargin});
  SAFE_CALL(m_ok)->setPosition({size.width/2.f + kPanelWidth/2.f - kMargin - m_ok->getContentSize().width, kMargin});
  float leftOffset = m_ok ? m_ok->getContentSize().width : 0;
  float rightOffset = m_cancel ? m_cancel->getContentSize().width : 0;

  SAFE_CALL(m_title)->setPosition({size.width/2.f, kPanelSize/2.f});
  SAFE_CALL(m_title)->setContentSize({kPanelWidth -leftOffset - rightOffset, kPanelSize});
}

void ListController::OnMouseScroll(cocos2d::Event* event)
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

void ListController::OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
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

void ListController::OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
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

void ListController::TimerForScroll(float updateTime)
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

void ListController::ScrollToPosition(float updateTime)
{
  m_list->scrollToPercentVertical(s_scrollPosition, 0.01, 0);
}

void ListController::ConfigureConfirmationPanel()
{
  m_confirmationPanel->setAnchorPoint({0, 0});
  m_ok = m_confirmationPanel->getChildByName<ui::Button*>("Ok");
  if (m_ok) m_ok->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                        {
                                          if (type == ui::Widget::TouchEventType::ENDED)
                                          {
                                            OnRightConfirmationButtonPressed();
                                          }
                                        });
  
  m_cancel = m_confirmationPanel->getChildByName<ui::Button*>("Cancel");
  if (m_cancel) m_cancel->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                                {
                                                  if (type == ui::Widget::TouchEventType::ENDED)
                                                  {
                                                    OnLeftConfirmationButtonPressed();
                                                  }
                                                });
  
  auto label = m_confirmationPanel->getChildByName<ui::Text*>("Label");
  label->setAnchorPoint({0.5, 0.5});
  m_title = label;
  
  bool doubleSize = komorki::ConfigManager::GetInstance()->RequiredDoubleSize();
  if (doubleSize) SAFE_CALL(m_title)->setFontSize(kDefaultTitleFontSize * 2);
  if (doubleSize) SAFE_CALL(m_ok)->setTitleFontSize(kDefaultValueFontSize * 2);
  if (doubleSize) SAFE_CALL(m_cancel)->setTitleFontSize(kDefaultValueFontSize * 2);
  if (doubleSize) SAFE_CALL(m_ok)->setContentSize(m_ok->getContentSize() * 2);
  if (doubleSize) SAFE_CALL(m_cancel)->setContentSize(m_cancel->getContentSize() * 2);
}

