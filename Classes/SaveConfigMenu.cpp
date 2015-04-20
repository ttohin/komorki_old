//
//  SaveConfigMenu.cpp
//  komorki
//
//  Created by ttohin on 19.04.15.
//
//

#include "SaveConfigMenu.h"
#include "cocostudio/CocoStudio.h"
#include "ConfigManager.h"

USING_NS_CC;
using namespace cocostudio;

namespace
{
  const float kMargin = 2.0;
  const float kPanelSize = 40.0;
  float kPanelWidth = 600;
}

void SaveConfigMenu::Init(const SaveConfigMenuCallback& savedCallback,
                          const SaveConfigMenuCallback& cancelCallback)
{
  m_mainLayer = nullptr;
  m_cancelCallback = cancelCallback;
  m_savedCallback = savedCallback;
}

void SaveConfigMenu::ShowInView(cocos2d::Node* root)
{
  if (m_mainLayer)
  {
    m_mainLayer->removeFromParentAndCleanup(true);
    m_mainLayer->release();
    m_mainLayer = nullptr;
  }
  
  m_rootNode = root;

  m_mainLayer = CSLoader::createNode("LoadConfig.csb")->getChildByName<ui::Layout*>("BackgroundPanel");
  m_mainLayer->retain();
  m_mainLayer->removeFromParent();
  addChild(m_mainLayer);
  root->addChild(this, 99999);
  
  Size size = Director::getInstance()->getVisibleSize();
  
  m_list = (ui::ListView*)m_mainLayer->getChildByName("ListView");
  m_list->setAnchorPoint({0.5, 0});
  m_confirmationPanel = (ui::Layout*)m_mainLayer->getChildByName("ConfirmationPanel");
  m_confirmationPanel->setAnchorPoint({0, 1});
  
  insertWidgetFromAnotherNode(m_list, CreateNewSaveConfigView());
  
  for (auto configName : komorki::ConfigManager::GetInstance()->GetConfigs())
  {
    insertWidgetFromAnotherNode(m_list, CreateLoadConfigView(configName));
  }
  
  ConfigureConfirmationPanel(m_confirmationPanel);
  
  Resize(size);
  
  SetListView(m_list);
}

void SaveConfigMenu::Hide()
{
  this->removeFromParentAndCleanup(true);
  ListController::Hide();
}

void SaveConfigMenu::Resize(const cocos2d::Size& size)
{
  kPanelWidth = std::min(kPanelWidth, size.width);
  
  m_mainLayer->setContentSize(size);
  
  m_list->setContentSize({kPanelWidth, size.height - kPanelSize});
  m_list->setPosition({size.width/2.f, 0});
  
  m_confirmationPanel->setContentSize({size.width, kPanelSize});
  m_confirmationPanel->setPosition({0, size.height});
  
  m_cancel->setPosition({size.width/2.f - kPanelWidth/2.f + kMargin, kMargin});
  
  m_title->setPosition({size.width/2.f, kPanelSize/2.f});
}

void SaveConfigMenu::OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, Event* event)
{
  if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
  {
    m_cancelCallback();
  }
  else
  {
    ListController::OnKeyRelease(keyCode, event);
  }
}

SaveConfigMenu::~SaveConfigMenu()
{
  m_mainLayer->unschedule("increase");
  m_mainLayer->unschedule("increase++");
  m_mainLayer->removeFromParentAndCleanup(true);
  m_mainLayer->release();
}

void SaveConfigMenu::SaveConfig(const std::string& name)
{
  komorki::ConfigManager::GetInstance()->SavePendingConfig(name);
  m_savedCallback();
}

void SaveConfigMenu::RemoveConfig(const std::string& name)
{
  komorki::ConfigManager::GetInstance()->RemoveConfig(name);
  ShowInView(m_rootNode);
}

void SaveConfigMenu::SaveNewConfig()
{
  komorki::ConfigManager::GetInstance()->SavePendingConfigToNewFile();
  m_savedCallback();
}

void SaveConfigMenu::Cancel()
{
  Hide();
  m_cancelCallback();
}

void SaveConfigMenu::addChildFromAnotherNode(Node* node)
{
  node->retain();
  node->removeFromParent();
  m_mainLayer->addChild( node );
  node->release();
}

ui::Layout* SaveConfigMenu::CreateLoadConfigView(const std::string& name)
{
  auto result = CSLoader::createNode("LoadConfigView.csb")->getChildByName<ui::Layout*>("LoadConfigView");
  result->retain();
  result->removeFromParent();
  result->autorelease();
  result->setContentSize({kPanelWidth, kPanelSize});
  
  auto label = result->getChildByName<ui::Text*>("Label");
  label->setString(name);
  
  auto loadButton = result->getChildByName<ui::Button*>("Load");
  loadButton->setTitleText("Save");
  loadButton->setAnchorPoint({1,0});
  loadButton->setPosition({kPanelWidth - kMargin, kMargin});
  loadButton->addTouchEventListener([this, name](Ref*,ui::Widget::TouchEventType type)
                                    {
                                      if (type == ui::Widget::TouchEventType::ENDED)
                                      {
                                        this->SaveConfig(name);
                                      }
                                    });
  
  auto removeButton = result->getChildByName<ui::Button*>("Remove");
  removeButton->setAnchorPoint({1,0});
  removeButton->setPosition({kPanelWidth - kMargin - loadButton->getContentSize().width, kMargin});
  removeButton->addTouchEventListener([this, name](Ref*,ui::Widget::TouchEventType type)
                                      {
                                        if (type == ui::Widget::TouchEventType::ENDED)
                                        {
                                          this->RemoveConfig(name);
                                        }
                                      });
  
  return result;
}

cocos2d::ui::Layout* SaveConfigMenu::CreateNewSaveConfigView()
{
  auto result = CSLoader::createNode("LoadConfigView.csb")->getChildByName<ui::Layout*>("LoadConfigView");
  result->retain();
  result->removeFromParent();
  result->autorelease();
  result->setContentSize({kPanelWidth, kPanelSize});
  
  auto label = result->getChildByName<ui::Text*>("Label");
  label->setString("New config");
 

  auto removeButton = result->getChildByName<ui::Button*>("Remove");
  removeButton->setVisible(false);
  
  auto loadButton = result->getChildByName<ui::Button*>("Load");
  loadButton->setAnchorPoint({1,0});
  loadButton->setPosition({kPanelWidth - kMargin, kMargin});
  loadButton->setTitleText("Create");
  loadButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                    {
                                      if (type == ui::Widget::TouchEventType::ENDED)
                                      {
                                        this->SaveNewConfig();
                                      }
                                    });
  return result;
}

void SaveConfigMenu::insertWidgetFromAnotherNode(ui::ListView* listView, ui::Widget* widget)
{
  widget->retain();
  widget->removeFromParent();
  listView->pushBackCustomItem(widget);
  widget->release();
}

void SaveConfigMenu::ConfigureConfirmationPanel(ui::Layout* confirmationPanel)
{
  addChildFromAnotherNode(confirmationPanel);

  auto label = confirmationPanel->getChildByName<ui::Text*>("Label");
  label->setAnchorPoint({0.5, 0.5});
  label->setString("Save config as...");
  m_title = label;
  
  m_cancel = confirmationPanel->getChildByName<ui::Button*>("Cancel");
  m_cancel->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                  {
                                    if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                      m_cancelCallback();
                                    }
                                  });
}


