//
//  LoadConfigMenu.cpp
//  komorki
//
//  Created by ttohin on 19.04.15.
//
//

#include "LoadConfigMenu.h"
#include "cocostudio/CocoStudio.h"
#include "ConfigManager.h"

USING_NS_CC;
using namespace cocostudio;

namespace
{
  const float kDefaultMargin = 2.0;
  const float kDefaultPanelSize = 40.0;
  const float kDefaultPanelWidth = 600;
  const float kDefaultTitleFontSize = 18;
  const float kDefaultValueFontSize = 12;
  float kMargin = kDefaultMargin;
  float kPanelSize = kDefaultPanelSize;
  float kPanelWidth = kDefaultPanelWidth;
}

LoadConfigMenu::LoadConfigMenu()
: m_mainLayer(nullptr)
, m_cancelCallback(nullptr)
, m_loadConfigCallback(nullptr)
{
  
}

void LoadConfigMenu::Init(const LoadConfigCallback& loadConfigCallback,
                          const LoadConfigMenuCallback& cancelCallback)
{
  m_cancelCallback = cancelCallback;
  m_loadConfigCallback = loadConfigCallback;
}

void LoadConfigMenu::ShowInView(cocos2d::Node* root)
{
  if(komorki::ConfigManager::GetInstance()->RequiredDoubleSize())
  {
    kPanelSize = kDefaultPanelSize * 2;
    kPanelWidth = kDefaultPanelWidth * 2;
    kMargin = kDefaultMargin * 2;
  }
  
  Size size = Director::getInstance()->getVisibleSize();
  kPanelWidth = std::min(kPanelWidth, size.width);
  
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
  
  m_list = komorki::ListView::create();
  m_list->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
  m_list->setBackGroundColor(Color3B::BLACK);
  m_list->setClippingEnabled(false);
  m_list->setBounceEnabled(true);
  m_list->setBackGroundColorOpacity((unsigned char)(0.4 * 255));
  m_list->setAnchorPoint({0.5, 0});
  m_mainLayer->addChild(m_list);
  
  m_confirmationPanel = (ui::Layout*)m_mainLayer->getChildByName("ConfirmationPanel");
  m_confirmationPanel->setAnchorPoint({0, 1});
  addChildFromAnotherNode(m_confirmationPanel);
  
  insertWidgetFromAnotherNode(m_list, CreateNewSaveConfigView());
  for (auto configName : komorki::ConfigManager::GetInstance()->GetConfigs())
  {
    insertWidgetFromAnotherNode(m_list, CreateLoadConfigView(configName));
  }
  
  ListController::ConfigureConfirmationPanel();
  m_title->setString("Load Config");
 
  Resize(size);
}

void LoadConfigMenu::Hide()
{
  this->removeFromParentAndCleanup(true);
  ListController::Hide();
}

void LoadConfigMenu::Resize(const cocos2d::Size& size)
{
  kPanelWidth = std::min(kPanelWidth, size.width);
  
  m_mainLayer->setContentSize(size);
 
  m_list->setContentSize({kPanelWidth, size.height - kPanelSize});
  m_list->setPosition({size.width/2.f, 0});
  
  ListController::Resize(size);
}

void LoadConfigMenu::OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, Event* event)
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

LoadConfigMenu::~LoadConfigMenu()
{
  m_mainLayer->unschedule("increase");
  m_mainLayer->unschedule("increase++");
  m_mainLayer->removeFromParentAndCleanup(true);
  m_mainLayer->release();
}

void LoadConfigMenu::RemoveConfig(const std::string& name)
{
  komorki::ConfigManager::GetInstance()->RemoveConfig(name);
  ShowInView(m_rootNode);
}

void LoadConfigMenu::LoadConfig(const std::string& name)
{
  komorki::ConfigManager::GetInstance()->LoadConfig(name);
  m_loadConfigCallback(name);
}

void LoadConfigMenu::SaveNewConfig()
{
  komorki::ConfigManager::GetInstance()->CreatePendingConfig();
  komorki::ConfigManager::GetInstance()->SavePendingConfigToNewFile();
  m_loadConfigCallback(komorki::ConfigManager::GetInstance()->GetPendingConfigName());
}

void LoadConfigMenu::Cancel()
{
  m_cancelCallback();
}

void LoadConfigMenu::OnLeftConfirmationButtonPressed()
{
  m_cancelCallback();
}

void LoadConfigMenu::addChildFromAnotherNode(Node* node)
{
  node->retain();
  node->removeFromParent();
  m_mainLayer->addChild( node );
  node->release();
}

ui::Layout* LoadConfigMenu::CreateLoadConfigView(const std::string& name)
{
  auto result = CSLoader::createNode("LoadConfigView.csb")->getChildByName<ui::Layout*>("LoadConfigView");
  result->retain();
  result->removeFromParent();
  result->autorelease();
  result->setContentSize({kPanelWidth, kPanelSize});
  
  auto label = result->getChildByName<ui::Text*>("Label");
  label->setString(name);
  label->setTextVerticalAlignment(TextVAlignment::CENTER);
  
  auto loadButton = result->getChildByName<ui::Button*>("Load");
  loadButton->setAnchorPoint({1,0});
  loadButton->setPosition({kPanelWidth - kMargin, kMargin});
  loadButton->addTouchEventListener([this, name](Ref*,ui::Widget::TouchEventType type)
                                    {
                                      if (type == ui::Widget::TouchEventType::ENDED)
                                      {
                                        this->LoadConfig(name);
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
  
  bool doubleSize = komorki::ConfigManager::GetInstance()->RequiredDoubleSize();
  if (doubleSize) label->setContentSize(label->getContentSize() * 2.0);
  if (doubleSize) label->setFontSize(kDefaultTitleFontSize * 2);
  if (doubleSize) loadButton->setContentSize(loadButton->getContentSize() * 2.0);
  if (doubleSize) loadButton->setTitleFontSize(kDefaultValueFontSize * 2);
  if (doubleSize) removeButton->setContentSize(removeButton->getContentSize() * 2.0);
  if (doubleSize) removeButton->setTitleFontSize(kDefaultValueFontSize * 2);
  
  float leftOffset = 0;
  label->setPosition({leftOffset+ kMargin, 0});
  
  loadButton->setAnchorPoint({1, 0});
  loadButton->setPosition({kPanelWidth - kMargin, kMargin});
  
  float rightOffset = kMargin + loadButton->getContentSize().width + kMargin;
  
  removeButton->setAnchorPoint({1, 0});
  removeButton->setPosition({kPanelWidth - rightOffset, kMargin});
  rightOffset += removeButton->getContentSize().width + kMargin;
  
  label->setContentSize({kPanelWidth - rightOffset, kPanelSize});
  
  return result;
}

void LoadConfigMenu::insertWidgetFromAnotherNode(ui::ListView* listView, ui::Widget* widget)
{
  widget->retain();
  widget->removeFromParent();
  listView->pushBackCustomItem(widget);
  widget->release();
}

void LoadConfigMenu::ConfigureConfirmationPanel(ui::Layout* confirmationPanel)
{
  addChildFromAnotherNode(confirmationPanel);

  auto label = confirmationPanel->getChildByName<ui::Text*>("Label");
  label->setAnchorPoint({0.5, 0.5});
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

cocos2d::ui::Layout* LoadConfigMenu::CreateNewSaveConfigView()
{
  auto result = CreateLoadConfigView("New config");
  auto removeButton = result->getChildByName<ui::Button*>("Remove");
  removeButton->setVisible(false);
  
  auto loadButton = result->getChildByName<ui::Button*>("Load");
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


