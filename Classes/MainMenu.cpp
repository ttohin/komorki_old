//
//  MainMenu.cpp
//  komorki
//
//  Created by ttohin on 15.04.15.
//
//

#include "MainMenu.h"
#include "cocostudio/CocoStudio.h"
#include "ConfigManager.h"

USING_NS_CC;
using namespace cocostudio;

void MainMenu::Init(const MainMenuCallback& optionCallback,
                    const MainMenuCallback& exitCallback,
                    const MainMenuCallback& backCallback)
{
  m_bg = nullptr;
  m_optionCallback = optionCallback;
  m_exitCallback = exitCallback;
  m_backCallback = backCallback;
}

void MainMenu::ShowInView(cocos2d::Node* root)
{
  Size size = Director::getInstance()->getVisibleSize();
  
  if (m_bg)
  {
    root->addChild(m_bg, 99999);
    Resize(size);
    return;
  }
 
  m_bg = CSLoader::createNode("MainMenu.csb")->getChildByName<ui::Layout*>("BackgroundPanel");
  m_bg->retain();
  m_bg->removeFromParent();
  root->addChild(m_bg, 99999);
  
  m_mainMenu = m_bg->getChildByName<ui::Layout*>("MainMenuPanel");
  auto buttons = m_mainMenu->getChildByName("Buttons");
  
  auto icon = m_mainMenu->getChildByName<ui::ImageView*>("Icon");
  icon->setAnchorPoint({0.5, 0.5});
  icon->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.5, 1.1), ScaleTo::create(0.5, 1.0), nullptr)));
  icon->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.3, -5), RotateTo::create(0.3, 3), nullptr)));
  
  auto optionsButton = buttons->getChildByName<ui::Button*>("Options");
  optionsButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                       {
                                         if (type == ui::Widget::TouchEventType::ENDED)
                                         {
                                           this->m_optionCallback();
                                         }
                                       });
  
  auto exitButton = buttons->getChildByName<ui::Button*>("Exit");
  exitButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                       {
                                         if (type == ui::Widget::TouchEventType::ENDED)
                                         {
                                           this->m_exitCallback();
                                         }
                                       });
  
  auto backButton = buttons->getChildByName<ui::Button*>("Back");
  backButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                       {
                                         if (type == ui::Widget::TouchEventType::ENDED)
                                         {
                                           this->m_backCallback();
                                         }
                                       });
  Resize(size);
}

void MainMenu::OnMouseScroll(cocos2d::Event* event)
{
  
}

void MainMenu::OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, Event* event)
{
  if(keyCode == EventKeyboard::KeyCode::KEY_O)
  {
    m_optionCallback();
  }
  else if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
  {
    m_backCallback();
  }
  else if(keyCode == EventKeyboard::KeyCode::KEY_Q)
  {
    m_exitCallback();
  }
}

void MainMenu::OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
  
}

void MainMenu::Resize(const Size& size)
{
  m_bg->setContentSize(size);
  Size mainPanelSize = m_mainMenu->getContentSize();
  m_mainMenu->setPosition(Vec2(size.width, size.height)/2.f - Vec2(mainPanelSize.width, mainPanelSize.height)/2.f);
}

void MainMenu::Hide()
{
  m_bg->removeFromParentAndCleanup(false);
}

MainMenu::~MainMenu()
{
  m_bg->release();
}

