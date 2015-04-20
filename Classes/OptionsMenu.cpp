//
//  OptionsMenu.cpp
//  komorki 
//
//  Created by ttohin on 12.04.15.
//
//

#include "OptionsMenu.h"
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

void OptionsMenu::Init(const OptionsMenuCallback& confirmCallback,
                       const OptionsMenuCallback& cancelCallback,
                       const OptionsMenuCallback& saveAsCallback,
                       const OptionsMenuCallback& loadCallback)
{
  m_scroolDirection = 0.0;
  m_mainLayer = nullptr;
  m_confirmCallback = confirmCallback;
  m_cancelCallback = cancelCallback;
  m_saveAsCallback = saveAsCallback;
  m_loadCallback = loadCallback;
}

void OptionsMenu::ShowInView(cocos2d::Node* root)
{
  Size size = Director::getInstance()->getVisibleSize();
  if (m_mainLayer)
  {
    root->addChild(this);
    Resize(size);
    for (auto f : m_updateValueList) {
      f(0);
    }
    m_save->setBright(komorki::ConfigManager::GetInstance()->CanSaveConfig());
    UpdateTitle();
    return;
  }
  
  m_rootNode = root;

  m_mainLayer = CSLoader::createNode("ParameterList.csb")->getChildByName<ui::Layout*>("BackgroundPanel");
  m_mainLayer->retain();
  m_mainLayer->removeFromParent();
  addChild(m_mainLayer);
  root->addChild(this, 99999);
  
  m_list = (ui::ListView*)m_mainLayer->getChildByName("ListView");
  m_list->setAnchorPoint({0.5, 0});
  m_controlPanel = (ui::Layout*)m_mainLayer->getChildByName("ControlPanel");
  m_confirmationPanel = (ui::Layout*)m_mainLayer->getChildByName("ConfirmationPanel");
  
  komorki::PixelDescriptorProvider::Config* currentConfig = komorki::ConfigManager::GetInstance()->GetPendingConfig().get();
  if (!currentConfig) {
    komorki::ConfigManager::GetInstance()->CreatePendingConfig();
    currentConfig = komorki::ConfigManager::GetInstance()->GetPendingConfig().get();
  }
  
//  AddParameterView("Dirt size", &currentConfig->terrainSize, 1, 999);
  AddParameterView("Map width", &currentConfig->mapWidth, 1, 999);
  AddParameterView("Map height", &currentConfig->mapHeight, 1, 999);
  AddParameterView("Percent of cells", &currentConfig->percentOfCreatures, 0, 100);
  AddParameterView("Percent of orange", &currentConfig->percentOfOrange, 0, 100);
  AddParameterView("Percent of green", &currentConfig->percentOfGreen, 0, 100);
  AddParameterView("Percent of salad", &currentConfig->percentOfSalad, 0, 100);
  AddParameterView("Percent of cyan", &currentConfig->percentOfCyan, 0, 100);
  AddParameterViewForSection("green", &currentConfig->green);
  AddParameterViewForSection("orange", &currentConfig->orange);
  AddParameterViewForSection("salad", &currentConfig->salad);
  AddParameterViewForSection("cyan", &currentConfig->cyan);
  
  ConfigureConfirmationPanel(m_confirmationPanel);
  ConfigureControlPanel(m_controlPanel);
  
  m_save->setBright(komorki::ConfigManager::GetInstance()->CanSaveConfig());
  UpdateTitle();
  
  Resize(size);
 
  SetListView(m_list);
  
  this->schedule(CC_SCHEDULE_SELECTOR(OptionsMenu::ScrollToPosition), 0, 0, 0.01);
}

void OptionsMenu::Hide()
{
  this->removeFromParentAndCleanup(true);
  ListController::Hide();
}

OptionsMenu::~OptionsMenu()
{
  m_mainLayer->release();
}

void OptionsMenu::Resize(const cocos2d::Size& size)
{
  kPanelWidth = std::min(kPanelWidth, size.width);
  m_mainLayer->setContentSize(size);
  
  m_list->setContentSize({kPanelWidth, size.height - kPanelSize - kPanelSize});
  m_list->setPosition({size.width/2.f, kPanelSize});
  
  m_confirmationPanel->setContentSize({size.width, kPanelSize});
  m_confirmationPanel->setPosition({0, size.height - kPanelSize});
  
  m_controlPanel->setContentSize({size.width, kPanelSize});
  m_controlPanel->setPosition({0, kPanelSize});
  
  float offset = m_load->getPosition().x;
  m_load->setPosition({size.width/2.f - kPanelWidth/2.f + kMargin, kMargin});
  offset -= m_load->getPosition().x;
  m_save->setPosition({m_save->getPosition().x - offset, kMargin});
  m_saveAs->setPosition({m_saveAs->getPosition().x - offset, kMargin});
  m_reset->setPosition({m_reset->getPosition().x - offset, kMargin});
  
  m_cancel->setPosition({size.width/2.f - kPanelWidth/2.f + kMargin, kMargin});
  m_ok->setPosition({size.width/2.f + kPanelWidth/2.f - kMargin - m_ok->getContentSize().width, kMargin});
  
  m_title->setPosition({size.width/2.f, kPanelSize/2.f});
  m_title->setContentSize({kPanelWidth - m_ok->getContentSize().width - m_cancel->getContentSize().width, kPanelSize});
}


void OptionsMenu::OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, Event* event)
{
  if(keyCode == EventKeyboard::KeyCode::KEY_ENTER)
  {
    m_confirmCallback();
  }
  else if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
  {
    m_cancelCallback();
  }
  else
  {
    ListController::OnKeyRelease(keyCode, event);
  }
}

void OptionsMenu::ResetToDefaults()
{
  *komorki::ConfigManager::GetInstance()->GetPendingConfig().get() = komorki::PixelDescriptorProvider::Config();
  for (auto f : m_updateValueList) {
    f(0);
  }
  ConfigChanged();
}

void OptionsMenu::Confirm()
{
  m_confirmCallback();
}

void OptionsMenu::Cancel()
{
  m_cancelCallback();
}

ui::Layout* OptionsMenu::CreateParameterView(const std::string& name,
                                             std::function<int(int step)> proccessStep,
                                             std::function<void(ui::Text* label, int value)> changeLabel,
                                             const std::string& textureName)
{
  auto result = CSLoader::createNode("ParameterView.csb")->getChildByName<ui::Layout*>("ParameterView");
  result->retain();
  result->removeFromParent();
  result->autorelease();
  
  result->setContentSize({kPanelWidth, kPanelSize});
  
  auto label = result->getChildByName<ui::Text*>("Label");
  label->setString(name);
  
  auto valueText = result->getChildByName<ui::Text*>("Value");
  
  auto plusButton = result->getChildByName<ui::Button*>("Plus");
  
  auto changeValue = [valueText, proccessStep, changeLabel](int step)
  {
    changeLabel(valueText, proccessStep(step));
  };
  
  changeValue(0);
  
  auto increaseValueCallback = [changeValue](float)
  {
    changeValue(1);
  };
  
  auto increasePlusValueCallback = [changeValue](float)
  {
    changeValue(10);
  };
  
  plusButton->addTouchEventListener([this, increaseValueCallback, increasePlusValueCallback](Ref*,ui::Widget::TouchEventType type)
                                    {
                                      if (type == ui::Widget::TouchEventType::ENDED || type == ui::Widget::TouchEventType::CANCELED)
                                      {
                                        this->m_mainLayer->unschedule("increase");
                                        this->m_mainLayer->unschedule("increase++");
                                      }
                                      else
                                      {
                                        if (type == ui::Widget::TouchEventType::BEGAN)
                                        {
                                          increaseValueCallback(0.0);
                                          this->m_mainLayer->schedule(increaseValueCallback, 0.1, kRepeatForever, 0.3, "increase");
                                          this->m_mainLayer->schedule(increasePlusValueCallback, 0.05, kRepeatForever, 4.0, "increase++");
                                        }
                                      }
                                    });
  
  float plusButtonOriginalOffset = plusButton->getPosition().x;
  plusButton->setPosition({kPanelWidth - plusButton->getContentSize().width - kMargin, kMargin});
  float offset = plusButton->getPosition().x - plusButtonOriginalOffset;
  
  auto minusButton = result->getChildByName<ui::Button*>("Minus");
  minusButton->setPosition({offset + minusButton->getPosition().x - kMargin, kMargin});
  valueText->setPosition({offset + valueText->getPosition().x - kMargin, valueText->getPosition().y});
  
  auto decreaseValueCallback = [changeValue](float)
  {
    changeValue(-1);
  };
  
  auto decreasePlusValueCallback = [changeValue](float)
  {
    changeValue(-10);
  };
  
  minusButton->addTouchEventListener([this, decreaseValueCallback, decreasePlusValueCallback](Ref*,ui::Widget::TouchEventType type)
                                     {
                                       if (type == ui::Widget::TouchEventType::ENDED || type == ui::Widget::TouchEventType::CANCELED)
                                       {
                                         this->m_mainLayer->unschedule("increase");
                                         this->m_mainLayer->unschedule("increase++");
                                       }
                                       else
                                       {
                                         if (type == ui::Widget::TouchEventType::BEGAN)
                                         {
                                           decreaseValueCallback(0.0);
                                           this->m_mainLayer->schedule(decreaseValueCallback, 0.1, kRepeatForever, 0.3, "increase");
                                           this->m_mainLayer->schedule(decreasePlusValueCallback, 0.05, kRepeatForever, 4.0, "increase++");
                                         }
                                       }
                                     });
 
  float leftOffset = 0;
  if (!textureName.empty())
  {
    auto icon = ui::ImageView::create(textureName+ ".png");
    icon->setContentSize({40, 40});
    icon->setAnchorPoint({0, 0});
    result->addChild(icon, 999);
    leftOffset = 40;
  }
  
  label->setPosition({leftOffset+ kMargin, 0});
  label->setContentSize({kPanelWidth - kMargin, kPanelSize});
  
  m_updateValueList.push_back(changeValue);
  
  return result;
}

void OptionsMenu::AddParameterView(const std::string& text, int* value, int minValue, int maxValue, const std::string& textureName)
{
  auto changeTextValue = [](ui::Text* valueText, int value)
  {
    valueText->setString(std::to_string(value));
  };
  
  auto changeIntValue = [this](int* value, int minValue, int maxValue, int step)
  {
    if (*value + step > maxValue || *value + step < minValue)
      return *value;
    
    if (step != 0)
    {
      this->ConfigChanged();
    }
    
    *value += step;
    return *value;
  };
  
  m_list->pushBackCustomItem(CreateParameterView(text,
                                                 std::bind(changeIntValue, value, minValue, maxValue, std::placeholders::_1),
                                                 changeTextValue,
                                                 textureName));
}

void OptionsMenu::AddParameterView(const std::string& text, float* value, int minValue, int maxValue, const std::string& textureName)
{
  auto changeFloatTextValue = [](ui::Text* valueText, int value)
  {
    valueText->setString(std::to_string(value) + "%");
  };
  
  auto changeFloatValue = [this](float* value, int minValue, int maxValue, int step)
  {
    int intValue = *value * 100.f + 0.5;
    if (intValue + step > maxValue || intValue + step < minValue)
      return intValue;
    
    intValue += step;
    
    if (step != 0)
    {
      this->ConfigChanged();
    }
    
    *value = (float)intValue/100.0f;
    return intValue;
  };
  
  m_list->pushBackCustomItem(CreateParameterView(text,
                                                 std::bind(changeFloatValue, value, minValue, maxValue, std::placeholders::_1),
                                                 changeFloatTextValue,
                                                 textureName));
}

void OptionsMenu::AddParameterViewForSection(const std::string& sectionName,
                                             komorki::PixelDescriptorProvider::Config::CellConfig* config)
{
  AddParameterView("Health", &config->health, 1, 9999, sectionName);
  AddParameterView("Sleep time", &config->sleepTime, 0, 9999, sectionName);
  AddParameterView("Attack", &config->attack, 0, 9999, sectionName);
  AddParameterView("Min passive health chunk", &config->passiveHealthChunkMin, -9999, 9999, sectionName);
  AddParameterView("Max passive health chunk", &config->passiveHealthChunkMax, -9999, 9999, sectionName);
  AddParameterView("Health per attach", &config->healthPerAttack, -9999, 9999, sectionName);
  AddParameterView("Armor", &config->armor, 1, -9999, sectionName);
  AddParameterView("Life time", &config->lifeTime, 1, 9999, sectionName);
  AddParameterView("Percent of mutations", &config->percentOfMutations, 0, 100, sectionName);
}

void OptionsMenu::insertWidgetFromAnotherNode(ui::ListView* listView, ui::Widget* widget)
{
  widget->retain();
  widget->removeFromParent();
  listView->pushBackCustomItem(widget);
  widget->release();
}

void OptionsMenu::ConfigureConfirmationPanel(ui::Layout* confirmationPanel)
{
  m_ok = confirmationPanel->getChildByName<ui::Button*>("Ok");
  m_ok->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                              {
                                if (type == ui::Widget::TouchEventType::ENDED)
                                {
                                  m_confirmCallback();
                                }
                              });
  
  m_cancel = confirmationPanel->getChildByName<ui::Button*>("Cancel");
  m_cancel->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                  {
                                    if (type == ui::Widget::TouchEventType::ENDED)
                                    {
                                      m_cancelCallback();
                                    }
                                  });
  
  auto label = confirmationPanel->getChildByName<ui::Text*>("Label");
  label->setAnchorPoint({0.5, 0.5});
  label->setString("Options");
  m_title = label;
}

void OptionsMenu::ConfigureControlPanel(ui::Layout* controlPanel)
{
  m_save = controlPanel->getChildByName<ui::Button*>("Save");
  m_save->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                              {
                                if (type == ui::Widget::TouchEventType::ENDED)
                                {
                                  komorki::ConfigManager::GetInstance()->SavePendingConfig();
                                  this->ConfigChanged();
                                }
                              });
  
  m_saveAs = controlPanel->getChildByName<ui::Button*>("Save as");
  m_saveAs->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                {
                                  if (type == ui::Widget::TouchEventType::ENDED)
                                  {
                                    m_saveAsCallback();
                                  }
                                });
  m_load = controlPanel->getChildByName<ui::Button*>("Load");
  m_load->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                {
                                  if (type == ui::Widget::TouchEventType::ENDED)
                                  {
                                    m_loadCallback();
                                  }
                                });
  m_reset = controlPanel->getChildByName<ui::Button*>("Reset");
  m_reset->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                                {
                                  if (type == ui::Widget::TouchEventType::ENDED)
                                  {
                                    this->ResetToDefaults();
                                  }
                                });
}

void OptionsMenu::UpdateTitle()
{
  auto configManager = komorki::ConfigManager::GetInstance();
  std::string configName = configManager->GetPendingConfigName();
  if (configName.empty())
  {
    configName = "new";
  }
  std::string title = "Options ";
  title += "[" + configName;
  if (configManager->IsPendingConfigChanged()) {
    title += "*";
  }
  title += "]";
  m_title->setString(title);
}

void OptionsMenu::ConfigChanged()
{
  komorki::ConfigManager::GetInstance()->PendingConfigChanged();
  m_save->setBright(komorki::ConfigManager::GetInstance()->CanSaveConfig());
  UpdateTitle();
}




