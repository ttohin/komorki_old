#include "PartialMapScene.h"
#include "PixelDescriptorProvider.h"
#include "cocostudio/DictionaryHelper.h"
#include "UIButton.h"

static const float kInitialMapScale = 0.2;

static const float kUpdateTime = 0.2;

static const float kMaxMapScale= 3.0;
static const float kMinMapScale = 0.1;
static const float kMaxBgScale= 2.5;
static const float kMinBgScale = 1.2;

static const float kZoomStep = 0.05;
static const float kMoveStep = 25.0;
static const float kMoveTimer = 0.2;

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
static const float kButtonScale = 2.f;
static const float kButtonSize = 80.f;
#else
static const float kButtonScale = 1.f;
static const float kButtonSize = 40.f;
#endif

static const char* s_configPath = nullptr;

USING_NS_CC;
using namespace cocostudio;

Scene* PartialMapScene::createScene(const char* configPath)
{
  s_configPath = configPath;
  auto scene = Scene::create();
  auto layer = PartialMapScene::create();
  scene->addChild(layer);
  return scene;
}

void PartialMapScene::CreateMap()
{
  std::string path;
  if(s_configPath)
    path = s_configPath;
  
  std::string contentStr = FileUtils::getInstance()->getStringFromFile(path);
  
  rapidjson::Document doc;
  doc.Parse<0>(contentStr.c_str());
  
  komorki::PixelDescriptorProvider::Config config;
  
  config.terrainSize = DICTOOL->getIntValue_json(doc, "maxTerrainSize", config.terrainSize);
  config.mapWidth = DICTOOL->getIntValue_json(doc, "width", config.mapWidth);
  config.mapHeight = DICTOOL->getIntValue_json(doc, "height", config.mapHeight);
  
  config.greenHealth = DICTOOL->getIntValue_json(doc, "greenHealth", config.greenHealth);
  config.hunterHealth = DICTOOL->getIntValue_json(doc, "hunterHealth", config.hunterHealth);
  config.basehealth = DICTOOL->getIntValue_json(doc, "basehealth", config.basehealth);
  
  config.hunterHealthIncome = DICTOOL->getIntValue_json(doc, "hunterHealthIncome", config.hunterHealthIncome);
  config.hunterAttack = DICTOOL->getIntValue_json(doc, "hunterAttack", config.hunterAttack);
  config.hunterHungryDamage = DICTOOL->getIntValue_json(doc, "hunterHungryDamage", config.hunterHungryDamage);
  
  config.greenHealthIncome = DICTOOL->getIntValue_json(doc, "greenHealthIncome", config.greenHealthIncome);
  config.greenAttack = DICTOOL->getIntValue_json(doc, "greenAttack", config.greenAttack);
  config.greenHugryDamage = DICTOOL->getIntValue_json(doc, "greenHugryDamage", config.greenHugryDamage);
  config.greenSleepTime = DICTOOL->getIntValue_json(doc, "greenSleepTime", config.greenSleepTime);
  
  config.percentOfCreatures = DICTOOL->getFloatValue_json(doc, "percentOfCreatures", config.percentOfCreatures);
  config.percentOfHunters = DICTOOL->getFloatValue_json(doc, "percentOfHunters", config.percentOfHunters);
  config.percentOfGreen = DICTOOL->getFloatValue_json(doc, "percentOfGreen", config.percentOfGreen);
  config.percentOfSalat = DICTOOL->getFloatValue_json(doc, "percentOfSalat", config.percentOfSalat);
  
  config.salatIncomeMin = DICTOOL->getIntValue_json(doc, "salatIncomeMin", config.salatIncomeMin);
  config.salatIncomeMax = DICTOOL->getIntValue_json(doc, "salatIncomeMax", config.salatIncomeMax);
  config.imporvedSalatIncomeMin = DICTOOL->getIntValue_json(doc, "imporvedSalatIncomeMin", config.imporvedSalatIncomeMin);
  config.improvedSalatIncomeMax = DICTOOL->getIntValue_json(doc, "improvedSalatIncomeMax", config.improvedSalatIncomeMax);
  
  config.improvedSalatArmor = DICTOOL->getIntValue_json(doc, "improvedSalatArmor", config.improvedSalatArmor);
  config.salatArmor = DICTOOL->getIntValue_json(doc, "salatArmor", config.salatArmor);
  config.salatSleepTime = DICTOOL->getIntValue_json(doc, "salatSleepTime", config.salatSleepTime);
  config.maxLifeTime = DICTOOL->getIntValue_json(doc, "maxLifeTime", config.maxLifeTime);
  config.hunterLifeTime = DICTOOL->getIntValue_json(doc, "hunterLifeTime", config.hunterLifeTime);
  config.percentOfMMutations = DICTOOL->getFloatValue_json(doc, "percentOfMMutations", config.percentOfMMutations);
 
  int mapSegmentSize = DICTOOL->getIntValue_json(doc, "mapSegmentSize", 100);
  
  m_mapManager = new PixelMapManager(&config, mapSegmentSize);
  m_mapManager->CreateMap(m_rootNode);
}

bool PartialMapScene::init()
{
  if ( !Layer::init() )
  {
    return false;
  }
  
  m_brushMode = eBrushModeGreen;
  m_touchMode = eTouchModeMove;
  m_moveDirection = Vec2::ZERO;
  m_eraseBrush = false;
  m_stopManager = false;
  m_updateTime = kUpdateTime;
  
  m_brush = Sprite::create("cursor.png");
  addChild(m_brush, 2);
  m_brush->setVisible(false);
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  
  m_mapScale = kInitialMapScale;
  m_mapPos = Vec2();

  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  Size winSize = Director::getInstance()->getWinSize();
  
  m_bg = Sprite::create("background.png");
  addChild(m_bg, -999);
  
  m_rootNode = Node::create();
  addChild(m_rootNode);
  
  m_rootNode->setScale(m_mapScale);
  m_rootNode->setPosition(m_mapPos);

  CreateMap();
  
  SetBackgroundPosition();

  schedule(schedule_selector(PartialMapScene::timerForUpdate), m_updateTime, kRepeatForever, m_updateTime);

  auto touchListener = EventListenerTouchOneByOne::create();
  touchListener->setSwallowTouches(true);
  
  touchListener->onTouchMoved = [this](Touch* touch, Event* event)
  {
    if (m_touchMode == eTouchModeMove)
    {
      this->Move(touch->getDelta());
    }
    else if (m_touchMode == eTouchModeBrush)
    {
      if (m_eraseBrush)
      {
        m_mapManager->RemoveCreatureAtPostion(touch->getLocation());
      }
      else
      {
        m_mapManager->AddCreatureAtPosition(touch->getLocation(), this->GetCurretnCellType());
      }
    }
  };
  
  touchListener->onTouchBegan = [this](Touch* touch, Event* event)
  {
    return true;
  };
  
  touchListener->onTouchEnded = [this](Touch* touch, Event* event)
  {
    if (m_touchMode == eTouchModeBrush)
    {
      if (m_eraseBrush)
      {
        m_mapManager->RemoveCreatureAtPostion(touch->getLocation());
      }
      else
      {
        m_mapManager->AddCreatureAtPosition(touch->getLocation(), this->GetCurretnCellType());
      }
    }
  };

  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

  auto mouseListener = EventListenerMouse::create();
  mouseListener->onMouseScroll = [this](Event* event)
  {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    float scrollY = mouseEvent->getScrollY();
    
    if (scrollY > 0)
      scrollY = 0.01;
    else
      scrollY = -0.01;
    
    if(m_mapScale - scrollY <= 0.1)
      return;
    if (m_mapScale - scrollY >= 3.0)
      return;
    
    m_mapScale -= scrollY;
    
    float scaleRatio = m_mapScale/m_rootNode->getScale();
    
    m_rootNode->setScale(m_mapScale);

    float cursorX = mouseEvent->getCursorX();
    float cursorY = mouseEvent->getCursorY();
    
    float newX = cursorX - scaleRatio * (cursorX - m_mapPos.x);
    float newY = cursorY - scaleRatio * (cursorY - m_mapPos.y);
    
    m_mapPos = Point(newX, newY);
    m_rootNode->setPosition(m_mapPos);
    
    SetBackgroundPosition();
  };
  
  mouseListener->onMouseMove = [this](Event* event) {
    
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    
    if (m_touchMode == eTouchModeBrush)
    {
      m_brush->setPosition(Vec2(mouseEvent->getCursorX(),
                                mouseEvent->getCursorY()));
      if(m_eraseBrush == false)
      {
        m_mapManager->HightlightCellOnCursorPos(Vec2(mouseEvent->getCursorX(),
                                                     mouseEvent->getCursorY()), this->GetCurretnCellType());
      }
    }
  };
  
  _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
  
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event)
  {
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
      this->SetEraseMode(false);
      this->SetDrawingMode(false);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_EQUAL)
    {
      this->ZoomIn();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_MINUS)
    {
      this->ZoomOut();
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
    {
      m_moveDirection -= Vec2(1, 0);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
    {
      m_moveDirection -= Vec2(-1, 0);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
      m_moveDirection -= Vec2(0, -1);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
      m_moveDirection -= Vec2(0, 1);
    }
    
    if (m_moveDirection == Vec2::ZERO)
    {
      this->unschedule(schedule_selector(PartialMapScene::timerForMove));
    }
  };
  
  keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
  {
    Vec2 oldMoveDirection = m_moveDirection;
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
    {
      m_moveDirection += Vec2(1, 0);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
    {
      m_moveDirection += Vec2(-1, 0);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
      m_moveDirection += Vec2(0, -1);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
      m_moveDirection += Vec2(0, 1);
    }
    
    if (oldMoveDirection == Vec2::ZERO && m_moveDirection != Vec2::ZERO)
    {
      this->timerForMove(kMoveTimer);
      schedule(schedule_selector(PartialMapScene::timerForMove), kMoveTimer, kRepeatForever, 0);
    }
  };
  
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  CreateToolBar();
  CreateSpeedToolBar();
  m_toolbarNode->setPosition(Vec2(origin.x + visibleSize.width,
                                  origin.y + visibleSize.height));
  m_speedToolbar->setPosition(Vec2(origin.x + visibleSize.width,
                                   origin.y));
  
  return true;
}

void PartialMapScene::SetBackgroundPosition(float animationDuration)
{
  float scaleRatio = (m_mapScale - kMinMapScale)/(kMaxMapScale - kMinMapScale);
  float bgScale = scaleRatio * (kMaxBgScale - kMinBgScale) + kMinBgScale;
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  
  float bgAspect = 1.0;
  {
    Size contentSize = m_bg->getContentSize();
    float wAspect = visibleSize.width/contentSize.width;
    float hAspect = visibleSize.height/contentSize.height;
    bgAspect = MAX(wAspect, hAspect);
  }
  
  Vec2 centralPos = Vec2(visibleSize.width/2.f, visibleSize.height/2.f);
  
  Size mapSize = m_mapManager->GetTotalMapSize();
  Vec2 positionOffset = m_mapPos + Vec2(m_mapScale*mapSize.width/2.f,
                                        m_mapScale*mapSize.height/2.f) - Vec2(visibleSize.width/2.f,
                                                                              visibleSize.height/2.f);
  positionOffset *= 0.1;
  bgScale *= bgAspect;
  
  Vec2 newBgPos = centralPos + positionOffset;
  Size bgSize = m_bg->getTexture()->getContentSize() * bgScale;
  
  if (newBgPos.x - bgSize.width/2.0f > 0)
  {
    newBgPos.x = bgSize.width/2.0f;
  }
  
  if (newBgPos.x + bgSize.width/2.0 < visibleSize.width)
  {
    newBgPos.x = visibleSize.width - bgSize.width/2.0;
  }
  
  if (newBgPos.y - bgSize.height/2.0f > 0)
  {
    newBgPos.y = bgSize.height/2.0f;
  }
  
  if (newBgPos.y + bgSize.height/2.0 < visibleSize.height)
  {
    newBgPos.y = visibleSize.height - bgSize.height/2.0;
  }
  
  if (animationDuration == 0.0)
  {
    m_bg->setScale(bgScale);
    m_bg->setPosition(newBgPos);
  }
  else
  {
    auto scale = ScaleTo::create(animationDuration, bgScale);
    auto move = MoveTo::create(animationDuration, newBgPos);
    m_bg->runAction(Spawn::create(scale, move, nullptr));
  }

}

void PartialMapScene::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags)
{  
  Size viewSize = Director::getInstance()->getVisibleSize();
  Size contentSize = getContentSize();
  
  if (!viewSize.equals(contentSize))
  {
    // before we visit we are going to set our size to the visible size.
    setContentSize(Director::getInstance()->getVisibleSize());
   
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    SetBackgroundPosition();
    m_toolbarNode->setPosition(Vec2(origin.x + visibleSize.width,
                                    origin.y + visibleSize.height));
    m_speedToolbar->setPosition(Vec2(origin.x + visibleSize.width,
                                     origin.y));
  }
  
  Layer::visit(renderer, parentTransform, parentFlags);
}

void PartialMapScene::timerForUpdate(float dt)
{
  if(!m_mapManager->IsAvailable())
  {
    return;
  }
  
  if (m_stopManager)
  {
    delete m_mapManager;
    CreateMap();
    m_stopManager = false;
    return;
  }

  if (m_speed == eSpeedNormal || m_speed == eSpeedDouble)
  {
    if (m_prevSpeed == eSpeedWarp)
    {
      m_mapManager->Reset();
    }
    
    float updateTimeEstimated = m_speed == eSpeedNormal ? m_updateTime : m_updateTime * 0.5;
    m_mapManager->UpdateAsync(updateTimeEstimated);
    schedule(schedule_selector(PartialMapScene::timerForUpdate), updateTimeEstimated, kRepeatForever, updateTimeEstimated);
  }
  else if (m_speed == eSpeedWarp)
  {
    float updateTimeEstimated = 0.0;
    m_mapManager->UpdateWarp(updateTimeEstimated, 10);
    schedule(schedule_selector(PartialMapScene::timerForUpdate), updateTimeEstimated, kRepeatForever, updateTimeEstimated);
  }
  
  m_prevSpeed = m_speed;
}

void PartialMapScene::timerForMove(float dt)
{
  Vec2 moveDirection = m_moveDirection;
  
  float scaleRatio = (m_mapScale - kMinMapScale)/(kMaxMapScale - kMinMapScale);
  float moveStepCorrected = scaleRatio * (kMoveStep*5.0f - kMoveStep) + kMoveStep;
  
  moveDirection *= moveStepCorrected;
  
  Move(moveDirection, dt);
}

void PartialMapScene::Zoom(float direction)
{
  if(m_mapScale + direction*kZoomStep <= 0.1)
    return;
  if (m_mapScale + direction*kZoomStep >= 3.0)
    return;
  
  m_mapScale += direction*kZoomStep;
  
  float scaleRatio = m_mapScale/m_rootNode->getScale();
  
  m_rootNode->setScale(m_mapScale);
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  float cursorX = visibleSize.width/2;
  float cursorY = visibleSize.height/2;
  
  float newX = cursorX - scaleRatio * (cursorX - m_mapPos.x);
  float newY = cursorY - scaleRatio * (cursorY - m_mapPos.y);
  
  m_mapPos = Point(newX, newY);
  m_rootNode->setPosition(m_mapPos);
  
  SetBackgroundPosition();
}

void PartialMapScene::ZoomIn()
{
  Zoom(+1.0);
}

void PartialMapScene::ZoomOut()
{
  Zoom(-1.0);
}

void PartialMapScene::Move(const Vec2& direction, float animationDuration)
{
  m_mapPos = m_rootNode->getPosition() + direction;
  
  if (animationDuration == 0.0f)
  {
    m_rootNode->setPosition(m_mapPos);
    SetBackgroundPosition(0.0f);
  }
  else
  {
    m_rootNode->runAction(MoveTo::create(animationDuration, m_mapPos));
    SetBackgroundPosition(animationDuration);
  }
}

void PartialMapScene::CreateSpeedToolBar()
{
  m_speedToolbar = Node::create();
  addChild(m_speedToolbar, 999);
  
  auto speed1Button = ui::Button::create("speed1.png", "speed1_hl.png");
  speed1Button->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                        {
                                          if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                            this->SetSpeed(eSpeedNormal);
                                        });
  m_speedToolbar->addChild(speed1Button);
  speed1Button->setPosition(Vec2(-kButtonSize/2.f - kButtonSize*2, +kButtonSize/2.f));
  speed1Button->setScale(kButtonScale);
  m_speed1Button = speed1Button;
  
  auto speed2Button = ui::Button::create("speed2.png", "speed2_hl.png");
  speed2Button->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                      {
                                        if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                          this->SetSpeed(eSpeedDouble);
                                      });
  m_speedToolbar->addChild(speed2Button);
  speed2Button->setPosition(Vec2(-kButtonSize/2.f - kButtonSize*1, +kButtonSize/2.f));
  speed2Button->setScale(kButtonScale);
  m_speed2Button = speed2Button;
  
  auto speedWarpButton = ui::Button::create("speed10.png", "speed10_hl.png");
  speedWarpButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                      {
                                        if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                          this->SetSpeed(eSpeedWarp);
                                      });
  m_speedToolbar->addChild(speedWarpButton);
  speedWarpButton->setPosition(Vec2(-kButtonSize/2.f - kButtonSize*0, +kButtonSize/2.f));
  speedWarpButton->setScale(kButtonScale);
  m_speed10Button = speedWarpButton;
  
  SetSpeed(eSpeedNormal);
}

void PartialMapScene::CreateToolBar()
{
  m_toolbarNode = Node::create();
  addChild(m_toolbarNode, 999);
  
  auto brushButton = ui::Button::create("add_icon.png", "add_icon_hl.png");
  brushButton->addTouchEventListener(CC_CALLBACK_2(PartialMapScene::SelectBrushTouchDownAction, this));
  m_toolbarNode->addChild(brushButton);
  brushButton->setPosition(Vec2(-kButtonSize/2.f, -kButtonSize/2.f));
  brushButton->setScale(kButtonScale);
  m_brushButton = brushButton;
  
  auto removeBrushButton = ui::Button::create("remove_brush.png", "remove_brush_hl.png");
  removeBrushButton->addTouchEventListener(CC_CALLBACK_2(PartialMapScene::SelectRemoveBrushTouchDownAction, this));
  m_toolbarNode->addChild(removeBrushButton);
  removeBrushButton->setPosition(Vec2(-kButtonSize/2.f, -kButtonSize/2.f -kButtonSize));
  removeBrushButton->setScale(kButtonScale);
  m_eraseButton = removeBrushButton;
  
  auto cleanAllButton = ui::Button::create("remove.png", "remove_hl.png");
  cleanAllButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                        {
                                          if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                            m_mapManager->CleanMap();
                                        });
  m_toolbarNode->addChild(cleanAllButton);
  cleanAllButton->setPosition(Vec2(-kButtonSize/2.f, -kButtonSize/2.f -2*kButtonSize));
  cleanAllButton->setScale(kButtonScale);
  
  auto restartButton = ui::Button::create("restartButton.png", "restartButton_hl.png");
  restartButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                        {
                                          if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                          {
                                            m_stopManager = true;
                                          }
                                        });
  m_toolbarNode->addChild(restartButton);
  restartButton->setPosition(Vec2(-kButtonSize/2.f, -kButtonSize/2.f -3*kButtonSize));
  restartButton->setScale(kButtonScale);
  
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
  auto zoomIn = ui::Button::create("plusButton.png", "plusButton_hl.png");
  zoomIn->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                       {
                                         if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                         {
                                           this->zoomIn();
                                         }
                                       });
  m_toolbarNode->addChild(zoomIn);
  zoomIn->setPosition(Vec2(-buttonSize/2.f, -buttonSize/2.f -4*buttonSize));
  zoomIn->setScale(buttonScale);
  
  auto zoomOut = ui::Button::create("minusButton.png", "minusButton_hl.png");
  zoomOut->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                       {
                                         if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                         {
                                           this->zoomOut();
                                         }
                                       });
  m_toolbarNode->addChild(zoomOut);
  zoomOut->setPosition(Vec2(-buttonSize/2.f, -buttonSize/2.f -5*buttonSize));
  zoomOut->setScale(buttonScale);
#endif
  
  //
  // Create brush selector
  //
  m_cellsSelectoToolbar = Node::create();
  m_toolbarNode->addChild(m_cellsSelectoToolbar);
  m_cellsSelectoToolbar->setPosition(-kButtonSize, 0);
  
  auto greenCell = ui::Button::create("greenButton.png", "greenButton_hl.png");
  greenCell->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                   {
                                     if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                      this->SetBrushMode(eBrushModeGreen);
                                   });
  m_cellsSelectoToolbar->addChild(greenCell);
  greenCell->setPosition(Vec2(-kButtonSize/2.f - kButtonSize*3, -kButtonSize/2.f));
  greenCell->setScale(kButtonScale);
  m_green = greenCell;
  
  auto hunterCell = ui::Button::create("hunterButton.png", "hunterButton_hl.png");
  hunterCell->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                    {
                                      if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                        this->SetBrushMode(eBrushModeHunter);
                                    });
  m_cellsSelectoToolbar->addChild(hunterCell);
  hunterCell->setPosition(Vec2(-kButtonSize/2.f - kButtonSize*2, -kButtonSize/2.f));
  hunterCell->setScale(kButtonScale);
  m_hunter = hunterCell;
  
  auto saladCell = ui::Button::create("saladButton.png", "saladButton_hl.png");
  saladCell->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                   {
                                     if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                       this->SetBrushMode(eBrushModeSalad);
                                   });
  m_cellsSelectoToolbar->addChild(saladCell);
  saladCell->setPosition(Vec2(-kButtonSize/2.f - kButtonSize*1, -kButtonSize/2.f));
  saladCell->setScale(kButtonScale);
  m_salad = saladCell;
  
  auto improvedSaladCell = ui::Button::create("improvedSaladButton.png", "improvedSaladButton_hl.png");
  improvedSaladCell->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                           {
                                             if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                               this->SetBrushMode(eBrushModeImprovedSalad);
                                           });
  m_cellsSelectoToolbar->addChild(improvedSaladCell);
  improvedSaladCell->setPosition(Vec2(-kButtonSize/2.f, -kButtonSize/2.f));
  improvedSaladCell->setScale(kButtonScale);
  m_improvedSalad = improvedSaladCell;
  
  SetDrawingMode(false);
  SetBrushMode(eBrushModeGreen);
}

void PartialMapScene::SelectBrushTouchDownAction(Ref *sender, ui::Widget::TouchEventType controlEvent)
{
  if (controlEvent == ui::Widget::TouchEventType::ENDED)
  {
    bool enableBrush = (m_touchMode == eTouchModeMove) || m_eraseBrush == true;
    SetDrawingMode(enableBrush);
  }
}

void PartialMapScene::SelectRemoveBrushTouchDownAction(Ref *sender, ui::Widget::TouchEventType controlEvent)
{
  if (controlEvent == ui::Widget::TouchEventType::ENDED)
  {
    SetEraseMode(!m_eraseBrush);
  }
}

void PartialMapScene::SetBrushEnabled(bool enabled)
{
  if (enabled)
    m_touchMode = eTouchModeBrush;
  else
    m_touchMode = eTouchModeMove;
  
  m_brush->setVisible(enabled);
  m_mapManager->StopHightlighting();
}

void PartialMapScene::SetDrawingMode(bool enabled)
{
  if(enabled)
  {
    SetEraseMode(false);
  }
  
  SetBrushEnabled(enabled);
  m_cellsSelectoToolbar->setVisible(enabled);
}

void PartialMapScene::SetEraseMode(bool enabled)
{
  if (enabled)
  {
    m_eraseBrush = true;
    SetDrawingMode(false);
    SetBrushEnabled(true);
    m_eraseButton->loadTextureNormal( "remove_brush_sel.png");
  }
  else
  {
    m_eraseBrush = false;
    SetBrushEnabled(false);
    m_eraseButton->loadTextureNormal( "remove_brush.png");
  }
}

void PartialMapScene::SetSpeed(Speed speed)
{
  if (m_speed == eSpeedNormal)
    m_speed1Button->loadTextureNormal("speed1.png");
  if (m_speed == eSpeedDouble)
    m_speed2Button->loadTextureNormal("speed2.png");
  if (m_speed == eSpeedWarp)
    m_speed10Button->loadTextureNormal("speed10.png");
  
  m_speed = speed;
  
  if (m_speed == eSpeedNormal)
    m_speed1Button->loadTextureNormal("speed1_sel.png");
  if (m_speed == eSpeedDouble)
    m_speed2Button->loadTextureNormal("speed2_sel.png");
  if (m_speed == eSpeedWarp)
    m_speed10Button->loadTextureNormal("speed10_sel.png");
  
  if (m_speed != eSpeedNormal)
  {
    SetBrushEnabled(false);
    SetDrawingMode(false);
    SetEraseMode(false);
    m_brushButton->setEnabled(false);
    m_eraseButton->setEnabled(false);
    m_brushButton->loadTextureNormal("add_icon_dis.png");
    m_eraseButton->loadTextureNormal("remove_brush_dis.png");
  }
  else
  {
    m_brushButton->setEnabled(true);
    m_eraseButton->setEnabled(true);
    m_brushButton->loadTextureNormal("add_icon.png");
    m_eraseButton->loadTextureNormal("remove_brush.png");
  }
}

void PartialMapScene::SetBrushMode(BrushMode mode)
{
  if (m_brushMode == eBrushModeGreen)
    m_green->loadTextureNormal("greenButton.png");
  if (m_brushMode == eBrushModeHunter)
    m_hunter->loadTextureNormal("hunterButton.png");
  if (m_brushMode == eBrushModeSalad)
    m_salad->loadTextureNormal("saladButton.png");
  if (m_brushMode == eBrushModeImprovedSalad)
    m_improvedSalad->loadTextureNormal("improvedSaladButton.png");
  
  m_brushMode = mode;
  
  if (m_brushMode == eBrushModeGreen)
    m_green->loadTextureNormal("greenButton_sel.png");
  if (m_brushMode == eBrushModeHunter)
    m_hunter->loadTextureNormal("hunterButton_sel.png");
  if (m_brushMode == eBrushModeSalad)
    m_salad->loadTextureNormal("saladButton_sel.png");
  if (m_brushMode == eBrushModeImprovedSalad)
    m_improvedSalad->loadTextureNormal("improvedSaladButton_sel.png");
}

komorki::CellType PartialMapScene::GetCurretnCellType()
{
  if (m_brushMode == eBrushModeGreen)
    return komorki::eCellTypeGreen;
  if (m_brushMode == eBrushModeHunter)
    return komorki::eCellTypeHunter;
  if (m_brushMode == eBrushModeSalad)
    return komorki::eCellTypeSalat;
  if (m_brushMode == eBrushModeImprovedSalad)
    return komorki::eCellTypeImprovedSalat;
  
  assert(0);
  return komorki::eCellTypeGreen;
}

