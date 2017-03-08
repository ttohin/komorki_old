//
//  MainMenu.cpp
//  komorki
//
//  Created by ttohin on 15.04.15.
//
//

#include "MainMenu.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;
using namespace cocostudio;

namespace
{
  const float kDefaultButtonFontSize = 14.0;
  const float kDefaultTitleFontSize = 30.0;
  const float kDefaultVersionFontSize = 12.0;
}

MainMenu::MainMenu()
: m_optionCallback(nullptr)
, m_exitCallback(nullptr)
, m_backCallback(nullptr)
{
  
}

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
  
  bool doublSize = false;
 
  m_bg = CSLoader::createNode("MainMenu.csb")->getChildByName<ui::Layout*>("BackgroundPanel");
  m_bg->retain();
  m_bg->removeFromParent();
  root->addChild(m_bg, 99999);
  
  m_mainMenu = m_bg->getChildByName<ui::Layout*>("MainMenuPanel");
  if (doublSize) m_mainMenu->setContentSize(m_mainMenu->getContentSize() * 2);
  
  m_mainMenu->setAnchorPoint({0.5, 0.5});
  auto buttons = m_mainMenu->getChildByName("Buttons");
  buttons->setAnchorPoint({0.5, 0});
  if (doublSize) buttons->setPosition(buttons->getPosition() * 2);
 
  auto titleNode = m_mainMenu->getChildByName<Node*>("Title");
  m_titleNode = titleNode;
  if (doublSize) m_titleNode->setPosition(m_titleNode->getPosition() * 2);
  auto icon = titleNode->getChildByName<ui::ImageView*>("Icon");
  icon->setAnchorPoint({0.5, 0.5});
  if (doublSize) icon->setPosition(icon->getPosition() * 2);
//  icon->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.5, 1.1), ScaleTo::create(0.5, 1.0), nullptr)));
//  icon->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.3, -5), RotateTo::create(0.3, 3), nullptr)));
  if (doublSize) icon->setScale(2);
  
  auto restartButton = buttons->getChildByName<ui::Button*>("Options");
  restartButton->setTitleText("Restart");
  restartButton->setAnchorPoint({0.5, 0});


  restartButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                       {
                                         if (type == ui::Widget::TouchEventType::ENDED)
                                         {
                                           this->m_optionCallback();
                                         }
                                       });
  
  auto exitButton = buttons->getChildByName<ui::Button*>("Exit");
  exitButton->setAnchorPoint({0.5, 0});

  exitButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                    {
                                      if (type == ui::Widget::TouchEventType::ENDED)
                                      {
                                        this->m_exitCallback();
                                      }
                                    });
  
  auto backButton = buttons->getChildByName<ui::Button*>("Back");
  backButton->setAnchorPoint({0.5, 0});

  backButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                    {
                                      if (type == ui::Widget::TouchEventType::ENDED)
                                      {
                                        this->m_backCallback();
                                      }
                                    });
  
  auto version = m_mainMenu->getChildByName<ui::Text*>("Version");
  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  exitButton->setVisible(false);
  version->setPosition(version->getPosition() + Vec2(0, 40));
#else
  exitButton->setTitleText(exitButton->getTitleText() + " [Q]");
  backButton->setTitleText(backButton->getTitleText() + " [ESC]");
  restartButton->setTitleText(restartButton->getTitleText() + " [O]");
#endif
  
  if (doublSize) restartButton->setContentSize(restartButton->getContentSize() * 2);
  if (doublSize) restartButton->setTitleFontSize(kDefaultButtonFontSize * 2);
  if (doublSize) restartButton->setPosition(restartButton->getPosition() * 2);                               
  if (doublSize) exitButton->setContentSize(exitButton->getContentSize() * 2);
  if (doublSize) exitButton->setTitleFontSize(kDefaultButtonFontSize * 2);
  if (doublSize) exitButton->setPosition(exitButton->getPosition() * 2);
  if (doublSize) backButton->setContentSize(backButton->getContentSize() * 2);
  if (doublSize) backButton->setTitleFontSize(kDefaultButtonFontSize * 2);
  if (doublSize) backButton->setPosition(backButton->getPosition() * 2);
  
  
  auto title = titleNode->getChildByName<ui::Text*>("Komorki");
  if (doublSize) title->setPosition(title->getPosition() * 2);
  if (doublSize) title->setFontSize(kDefaultTitleFontSize * 2);
  if (doublSize) title->setContentSize(title->getContentSize() * 2);
  
  if (doublSize) version->setPosition(version->getPosition() * 2);
  if (doublSize) version->setFontSize(kDefaultVersionFontSize * 2);
  if (doublSize) version->setContentSize(version->getContentSize() * 2);
  
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
  m_mainMenu->setPosition({size.width/2.f, size.height/2.f});
}

void MainMenu::Hide()
{
  m_bg->removeFromParentAndCleanup(false);
}

MainMenu::~MainMenu()
{
  m_bg->release();
}

