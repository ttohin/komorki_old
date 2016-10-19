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
  const float kDefaultMargin = 2.0;
  const float kDefaultPanelSize = 40.0;
  const float kDefaultPanelWidth = 600;
  const float kDefaultTitleFontSize = 18;
  const float kDefaultValueFontSize = 12;
  float kMargin = kDefaultMargin;
  float kPanelSize = kDefaultPanelSize;
  float kPanelWidth = kDefaultPanelWidth;
}

OptionsMenu::OptionsMenu()
: m_cancelEditing(nullptr)
, m_confirmCallback(nullptr)
, m_save(nullptr)
, m_saveAs(nullptr)
, m_loadCallback(nullptr)
, m_hiddenTextField(nullptr)
, m_applyEditing(nullptr)
{
  
}

void OptionsMenu::Init(const OptionsMenuCallback& confirmCallback,
                       const OptionsMenuCallback& cancelCallback,
                       const OptionsMenuCallback& saveAsCallback,
                       const OptionsMenuCallback& loadCallback)
{
  m_scroolDirection = 0.0;
  m_mainLayer = nullptr;
  m_editingTitleBg = nullptr;
  m_editingTitle = nullptr;
  m_confirmCallback = confirmCallback;
  m_cancelCallback = cancelCallback;
  m_saveAsCallback = saveAsCallback;
  m_loadCallback = loadCallback;
}

void OptionsMenu::ShowInView(cocos2d::Node* root)
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
    root->addChild(this);
    Resize(size);
    for (auto f : m_updateValueList) {
      f(0);
    }
    UpdateView();
    return;
  }
  
  m_rootNode = root;

  m_mainLayer = CSLoader::createNode("ParameterList.csb")->getChildByName<ui::Layout*>("BackgroundPanel");
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
  
  m_controlPanel = (ui::Layout*)m_mainLayer->getChildByName("ControlPanel");
  m_confirmationPanel = (ui::Layout*)m_mainLayer->getChildByName("ConfirmationPanel");
  
  komorki::Config* currentConfig = komorki::ConfigManager::GetInstance()->GetPendingConfig().get();
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
 
  addChildFromAnotherNode(m_confirmationPanel);
  
  ListController::ConfigureConfirmationPanel();
  ConfigureControlPanel(m_controlPanel);
  
  bool doubleSize = komorki::ConfigManager::GetInstance()->RequiredDoubleSize();
  if (doubleSize) m_load->setContentSize(m_load->getContentSize() * 2);
  if (doubleSize) m_save->setContentSize(m_save->getContentSize() * 2);
  if (doubleSize) m_saveAs->setContentSize(m_saveAs->getContentSize() * 2);
  if (doubleSize) m_reset->setContentSize(m_reset->getContentSize() * 2);
  if (doubleSize) m_load->setTitleFontSize(kDefaultValueFontSize * 2);
  if (doubleSize) m_save->setTitleFontSize(kDefaultValueFontSize * 2);
  if (doubleSize) m_saveAs->setTitleFontSize(kDefaultValueFontSize * 2);
  if (doubleSize) m_reset->setTitleFontSize(kDefaultValueFontSize * 2);
  
  m_save->setBright(komorki::ConfigManager::GetInstance()->CanSaveConfig());
  UpdateView();
  
  Resize(size);
 
  this->schedule(CC_SCHEDULE_SELECTOR(OptionsMenu::ScrollToPosition), 0, 0, 0.01);
}

void OptionsMenu::Hide()
{
  if (m_hiddenTextField)
  {
    auto glView = Director::getInstance()->getOpenGLView();
    if (glView)
    {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WP8 && CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
      glView->setIMEKeyboardState(false);
#endif
    }
    
    m_applyEditing();
  }
  
  this->removeFromParentAndCleanup(true);
  ListController::Hide();
}

OptionsMenu::~OptionsMenu()
{
  m_mainLayer->release();
}

void OptionsMenu::Resize(const cocos2d::Size& size)
{
  ListController::Resize(size);
  
  kPanelWidth = std::min(kPanelWidth, size.width);
  
  m_mainLayer->setContentSize(size);
  
  m_controlPanel->setContentSize({size.width, kPanelSize});
  m_controlPanel->setPosition({0, kPanelSize});
  
  m_list->setContentSize({kPanelWidth, size.height - kPanelSize - kPanelSize});
  m_list->setPosition({size.width/2.f, kPanelSize});
  
  float leftOffset = size.width/2.f - kPanelWidth/2.f + kMargin;
  m_load->setPosition({leftOffset, kMargin});
  leftOffset += m_load->getContentSize().width + kMargin;
  m_save->setPosition({leftOffset, kMargin});
  leftOffset += m_save->getContentSize().width + kMargin;
  m_saveAs->setPosition({leftOffset, kMargin});
  leftOffset += m_saveAs->getContentSize().width + kMargin;
  m_reset->setPosition({leftOffset, kMargin});
  leftOffset += m_reset->getContentSize().width + kMargin;
}

void OptionsMenu::OnKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, Event* event)
{
  if(keyCode == EventKeyboard::KeyCode::KEY_ENTER)
  {
    m_confirmCallback();
  }
  else if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
  {
    if(m_cancelEditing != nullptr)
    {
      m_cancelEditing();
      return;
    }
    
    m_cancelCallback();
  }
  else if(keyCode == EventKeyboard::KeyCode::KEY_KP_ENTER)
  {
    if (m_hiddenTextField != nullptr) m_hiddenTextField->setDetachWithIME(true);
  }
  else
  {
    ListController::OnKeyRelease(keyCode, event);
  }
}

void OptionsMenu::ResetToDefaults()
{
  komorki::ConfigManager::GetInstance()->ResetPendingConfig();
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

void OptionsMenu::OnLeftConfirmationButtonPressed()
{
  Cancel();
}

void OptionsMenu::OnRightConfirmationButtonPressed()
{
  Confirm();
}

ui::Layout* OptionsMenu::CreateParameterView(const std::string& name,
                                             int digitTimit,
                                             std::function<int(int step)> proccessStep,
                                             std::function<void(ui::Text* label, int value)> changeLabel,
                                             std::function<void(cocos2d::ui::Text* valueText, const std::string& stringValue)> changeLabelString,
                                             std::function<void(const std::string& stringValue)> chnageValueString,
                                             const std::string& textureName)
{
  bool doubleSize = komorki::ConfigManager::GetInstance()->RequiredDoubleSize();
  
  auto result = CSLoader::createNode("ParameterView.csb")->getChildByName<ui::Layout*>("ParameterView");
  result->retain();
  result->removeFromParent();
  result->autorelease();
  result->setContentSize({kPanelWidth, kPanelSize});
  
  auto label = result->getChildByName<ui::Text*>("Label");
  label->setString(name);
  if (doubleSize) label->setFontSize(kDefaultTitleFontSize * 1.5);
  
  auto valueText = result->getChildByName<ui::Text*>("Value");
  if (doubleSize) valueText->setFontSize(kDefaultValueFontSize * 2);
  auto valueBg = result->getChildByName<ui::Layout*>("ValueBg");
  auto valueEditField = result->getChildByName<ui::TextField*>("TextField");
//  valueEditField->setVisible(true);
  addChildFromAnotherNode(valueEditField);
  valueEditField->setMaxLength(digitTimit);
  
  auto cancelEditing = [valueEditField, valueText, changeLabel, proccessStep]()
  {
    changeLabel(valueText, proccessStep(0));
    valueEditField->setString("");
    valueEditField->setDetachWithIME(true);
  };
  
  auto applyValue = [chnageValueString, changeLabel, valueEditField, valueText, proccessStep]()
  {
    chnageValueString(valueEditField->getString());
    changeLabel(valueText, proccessStep(0));
  };
  
  valueEditField->addEventListener([this, cancelEditing, applyValue, changeLabelString, valueText, valueBg, valueEditField](Ref*, ui::TextField::EventType type )
                                   {
                                     if (type == ui::TextField::EventType::INSERT_TEXT ||
                                         type == ui::TextField::EventType::DELETE_BACKWARD)
                                     {
                                       std::string s = valueEditField->getString();
                                       size_t initialLenght = s.length();
                                       s.erase(std::remove_if(s.begin(), s.end(), [initialLenght](char c) {
                                         if (initialLenght == 1)
                                         {
                                           return !::isdigit(c) && c != '-';
                                         }
                                         
                                         return !::isdigit(c);
                                       }), s.end());
                                       
                                       valueEditField->setString(s);
                                       
                                       changeLabelString(valueText, s);
                                     }
                                     
                                     if (type == ui::TextField::EventType::ATTACH_WITH_IME)
                                     {
                                       m_applyEditing = applyValue;
                                       m_cancelEditing = cancelEditing;
                                       m_hiddenTextField = valueEditField;
                                       auto fadeIn = FadeIn::create(0.4);
                                       auto fadeOut = FadeOut::create(0.4);
                                       valueBg->runAction(RepeatForever::create(Sequence::createWithTwoActions(fadeOut, fadeIn)));
                                     }
                                     if (type == ui::TextField::EventType::DETACH_WITH_IME)
                                     {
                                       applyValue();
                                       valueBg->stopAllActions();
                                       valueBg->setOpacity(255);

                                       if (m_hiddenTextField == valueEditField)
                                       {
                                         m_hiddenTextField = nullptr;
                                       }
                                     }
                                   });
  
  valueText->addTouchEventListener([valueBg, valueText, valueEditField, this](Ref*,ui::Widget::TouchEventType type)
  {
    valueBg->stopAllActions();
    valueBg->setOpacity(255);
    m_editingTitle = valueText;
    m_editingTitleBg = valueBg;
    
    valueEditField->attachWithIME();
    valueEditField->setString("");
  });

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
                                          if (m_list->isScrolling()) return;
                                          
                                          increaseValueCallback(0.0);
                                          this->m_mainLayer->schedule(increaseValueCallback, 0.1, kRepeatForever, 0.3, "increase");
                                          this->m_mainLayer->schedule(increasePlusValueCallback, 0.05, kRepeatForever, 4.0, "increase++");
                                        }
                                      }
                                    });

  auto minusButton = result->getChildByName<ui::Button*>("Minus");
  
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
                                           if (m_list->isScrolling()) return;
                                           
                                           decreaseValueCallback(0.0);
                                           this->m_mainLayer->schedule(decreaseValueCallback, 0.1, kRepeatForever, 0.3, "increase");
                                           this->m_mainLayer->schedule(decreasePlusValueCallback, 0.05, kRepeatForever, 4.0, "increase++");
                                         }
                                       }
                                     });
 
  float leftOffset = kMargin;
  if (!textureName.empty())
  {
    auto icon = ui::ImageView::create(textureName+ ".png");
    icon->setContentSize({kPanelSize, kPanelSize});
    if (doubleSize) icon->setScale(2.0);
    icon->setAnchorPoint({0, 0});
    result->addChild(icon, 999);
    leftOffset += kPanelSize;
  }
 
  label->setTextVerticalAlignment(TextVAlignment::CENTER);
  label->setPosition({leftOffset, 0});
 
  plusButton->setAnchorPoint({1, 0});
  plusButton->setPosition({kPanelWidth - kMargin, kMargin});
  plusButton->setContentSize({kPanelSize, kPanelSize});
 
  float rightOffset = kMargin + plusButton->getContentSize().width + kMargin;

  if (doubleSize) valueText->setContentSize(valueText->getContentSize() * 2.0);
  if (doubleSize) valueBg->setContentSize(valueBg->getContentSize() * 2.0);
  
  valueBg->setAnchorPoint({0.5, 0.5});
  valueBg->setPosition({kPanelWidth - valueBg->getContentSize().width/2.f - rightOffset, kPanelSize/2.f});
  
  valueText->setPosition(valueBg->getPosition());
  valueText->setAnchorPoint({0.5, 0.5});
 
  rightOffset += valueBg->getContentSize().width + kMargin;
  
  minusButton->setAnchorPoint({1, 0});
  minusButton->setPosition({kPanelWidth - rightOffset, kMargin});
  minusButton->setContentSize({kPanelSize, kPanelSize});
  rightOffset += minusButton->getContentSize().width + kMargin;
  label->setContentSize({kPanelWidth - rightOffset - leftOffset, kPanelSize});
  
  m_updateValueList.push_back(changeValue);
  
  return result;
}

void OptionsMenu::AddParameterView(const std::string& text, int* value, int minValue, int maxValue, const std::string& textureName)
{
  auto changeTextValue = [](ui::Text* valueText, int value)
  {
    valueText->setString(std::to_string(value));
  };
  
  auto changeIntValue = [this, value, minValue, maxValue](int step)
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

  auto applyStringValue = [this, value, minValue, maxValue](const std::string& stringValue)
  {
    if (stringValue.empty()) {
      return ;
    }
    
    int intValue = std::stoi(stringValue);
    int prevValue = *value;
    if (intValue > maxValue)
      intValue = maxValue;
    
    if (intValue < minValue)
      intValue = minValue;
  
    *value = intValue;
    if (prevValue != *value)
    {
      this->ConfigChanged();
    }
  };

  auto changeTextValueWithString = [](ui::Text* valueText, const std::string& stringValue)
  {
    valueText->setString(stringValue);
  };
  
  m_list->pushBackCustomItem(CreateParameterView(text,
                                                 5,
                                                 changeIntValue,
                                                 changeTextValue,
                                                 changeTextValueWithString,
                                                 applyStringValue,
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
  
  
  auto applyStringValue = [this, value, minValue, maxValue](const std::string& stringValue)
  {
    if (stringValue.empty()) {
      return;
    }
    
    int intValue = std::stoi(stringValue);
    int prevValue = *value * 100.0;
    if (intValue > maxValue)
      intValue = maxValue;
    
    if (intValue < minValue)
      intValue = minValue;
  
    if (prevValue != intValue)
    {
      *value = intValue/100.f;
      this->ConfigChanged();
    }
  };
  
  auto changeTextValueWithString = [](ui::Text* valueText, const std::string& stringValue)
  {
    valueText->setString(stringValue + "%");
  };
  
  m_list->pushBackCustomItem(CreateParameterView(text,
                                                 3,
                                                 std::bind(changeFloatValue, value, minValue, maxValue, std::placeholders::_1),
                                                 changeFloatTextValue,
                                                 changeTextValueWithString,
                                                 applyStringValue,
                                                 textureName));
}

void OptionsMenu::AddParameterViewForSection(const std::string& sectionName,
                                             komorki::Config::CellConfig* config)
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

void OptionsMenu::ConfigureControlPanel(ui::Layout* controlPanel)
{
  controlPanel->retain();
  controlPanel->removeFromParent();
  m_mainLayer->addChild(controlPanel);
  controlPanel->release();
  m_save = controlPanel->getChildByName<ui::Button*>("Save");
  m_save->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType type)
                              {
                                if (type == ui::Widget::TouchEventType::ENDED)
                                {
                                  komorki::ConfigManager::GetInstance()->SavePendingConfig();
                                  this->UpdateView();
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

void OptionsMenu::UpdateView()
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
  m_save->setBright(komorki::ConfigManager::GetInstance()->CanSaveConfig());
}

void OptionsMenu::ConfigChanged()
{
  komorki::ConfigManager::GetInstance()->PendingConfigChanged();
  UpdateView();
}

void OptionsMenu::addChildFromAnotherNode(Node* node)
{
  node->retain();
  node->removeFromParent();
  m_mainLayer->addChild( node );
  node->release();
}



