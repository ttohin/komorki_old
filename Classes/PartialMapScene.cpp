#include "PartialMapScene.h"
#include "PixelDescriptorProvider.h"
#include "cocostudio/DictionaryHelper.h"
#include "UIButton.h"
#include "ConfigManager.h"

static const float kUpdateTime = 0.4;

static float kMaxMapScale= 3.0;
static float kMinMapScale = 0.1;
static const float kMaxBgScale= 2.5;
static const float kMinBgScale = 1.2;

static const float kZoomStep = 0.05;
static const float kMoveStep = 25.0;

static float kButtonScale = 1.f;
static float kButtonSize = 40.f;

#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
static const float kMoveTimer = 0.2;
#endif

static float kViewportUpdateTime = 0.1;

USING_NS_CC;
using namespace cocostudio;

Scene* PartialMapScene::createScene()
{
  auto scene = Scene::create();
  auto layer = PartialMapScene::create();
  scene->addChild(layer);
  return scene;
}

void PartialMapScene::CreateMap()
{
  auto config = komorki::ConfigManager::GetInstance()->GetCurrentConfig();

  Size visibleSize = Director::getInstance()->getVisibleSize();
  m_viewport = new komorki::ui::Viewport(m_rootNode, config.get(), visibleSize);
  
  m_mapScale = 0.1;
  m_mapPos = cocos2d::Vec2::ZERO;
  
}

bool PartialMapScene::init()
{
  if ( !Layer::init() )
  {
    return false;
  }
  
  auto glview = Director::getInstance()->getOpenGLView();
  bool isRetina = glview->isRetinaDisplay();
  if (isRetina)
  {
    kButtonScale *= 2.f;
    kButtonSize *= 2.f;
  }
  
  m_brushMode = eBrushModeGreen;
  m_touchMode = eTouchModeMove;
  m_moveDirection = Vec2::ZERO;
  m_eraseBrush = false;
  m_stopManager = false;
  m_updateTime = kUpdateTime;
  m_pause = false;
  
  m_brush = Sprite::create("cursor.png");
  addChild(m_brush, 2);
  m_brush->setVisible(false);
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  Size winSize = Director::getInstance()->getWinSize();
  
  m_bg = Sprite::create("background.png");
  addChild(m_bg, -999);
  
  m_rootNode = Node::create();
  addChild(m_rootNode);
  
  komorki::ConfigManager::GetInstance()->CreateNewConfig();
  CreateMap();
  
  SetBackgroundPosition();

  schedule(schedule_selector(PartialMapScene::timerForUpdate), m_updateTime, kRepeatForever, m_updateTime);
  schedule(schedule_selector(PartialMapScene::timerForViewportUpdate), kViewportUpdateTime, kRepeatForever, kViewportUpdateTime);

#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
  auto touchListener = EventListenerTouchOneByOne::create();
  touchListener->setSwallowTouches(true);
  
  touchListener->onTouchMoved = [this](Touch* touch, Event* event)
  {
    if (m_touchMode == eTouchModeMove)
    {
//      m_mapPos = m_mapPos - touch->getDelta();
      this->Move(touch->getDelta());
    }
    else if (m_touchMode == eTouchModeBrush)
    {
      if (m_eraseBrush)
      {
        #warning don't forget
//        m_mapManager->RemoveCreatureAtPostion(touch->getLocation());
      }
      else
      {
        #warning don't forget
//        m_mapManager->AddCreatureAtPosition(touch->getLocation(), this->GetCurretnCellType());
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
        #warning don't forget
//        m_mapManager->RemoveCreatureAtPostion(touch->getLocation());
      }
      else
      {
        #warning don't forget
//        m_mapManager->AddCreatureAtPosition(touch->getLocation(), this->GetCurretnCellType());
      }
    }
    else if (m_touchMode == eTouchModeMove)
    {
      this->m_viewport->Calculate();
    }
  };
  
#else // CC_TARGET_PLATFORM == CC_PLATFORM_IOS
  
  auto touchListener = EventListenerTouchAllAtOnce::create();
  touchListener->onTouchesMoved = [this](const std::vector<Touch*>& touches, Event* event)
  {
    if (touches.size() == 1)
    {
      if (m_touchMode == eTouchModeMove)
      {
        m_mapPos = m_rootNode->getPosition() + touches[0]->getDelta();
      }
      else if (m_touchMode == eTouchModeBrush)
      {
        if (m_eraseBrush)
        {
          m_mapManager->RemoveCreatureAtPostion(touches[0]->getLocation());
        }
        else
        {
          m_mapManager->AddCreatureAtPosition(touches[0]->getLocation(), this->GetCurretnCellType());
        }
      }
    }
    else if (touches.size() == 2)
    {
      Vec2 delta = (touches[0]->getDelta() + touches[1]->getDelta()) * 0.5;
      this->Move(delta);
      
      float initialDistance = touches[0]->getPreviousLocation().distance(touches[1]->getPreviousLocation());
      float currentDistance = touches[0]->getLocation().distance(touches[1]->getLocation());
      
      float scale = 0.5 * (currentDistance - initialDistance)/initialDistance;
      this->Zoom(scale);
    }
  };
  
#endif // CC_TARGET_PLATFORM != CC_PLATFORM_IOS

  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
  auto mouseListener = EventListenerMouse::create();
  mouseListener->onMouseScroll = [this](Event* event)
  {
    if(this->m_currenMenu)
    {
      m_currenMenu->OnMouseScroll(event);
      return;
    }
    
    
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    float scrollY = mouseEvent->getScrollY();
    
    if (scrollY > 0)
      scrollY = 0.02;
    else
      scrollY = -0.02;
    
//    m_rootNode->setScale(m_mapScale);

    float cursorX = mouseEvent->getCursorX();
    float cursorY = mouseEvent->getCursorY();
    
    m_viewport->Zoom({cursorX, cursorY}, scrollY);
  };
  
  mouseListener->onMouseMove = [this](Event* event) {
    
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    
    if (m_touchMode == eTouchModeBrush)
    {
      m_brush->setPosition(Vec2(mouseEvent->getCursorX(),
                                mouseEvent->getCursorY()));
      if(m_eraseBrush == false)
      {
        #warning don't forget
//        m_mapManager->HightlightCellOnCursorPos(Vec2(mouseEvent->getCursorX(),
//                                                     mouseEvent->getCursorY()), this->GetCurretnCellType());
      }
    }
  };
  
  _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
  
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event)
  {
    // pass move keys
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
    
    if(this->m_currenMenu)
    {
      m_currenMenu->OnKeyRelease(keyCode, event);
      return;
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
      if (m_touchMode == eTouchModeMove)
      {
        this->ShowMainMenu();
        m_moveDirection = Vec2::ZERO;
        this->unschedule(schedule_selector(PartialMapScene::timerForMove));
        return;
      }
      
      this->SetEraseMode(false);
      this->SetDrawingMode(false);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_EQUAL ||
             keyCode == EventKeyboard::KeyCode::KEY_PLUS)
    {
      this->ZoomIn();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_MINUS)
    {
      this->ZoomOut();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_Q)
    {
      this->Exit();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_O)
    {
      this->ShowOptionsMenu();
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
    
    if(this->m_currenMenu)
    {
      m_currenMenu->OnKeyPressed(keyCode, event);
      return;
    }
  };
  
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
  
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS
  
  auto menuButton = ui::Button::create("menuButton.png", "menuButtons_hl.png");
  menuButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                      {
                                        if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                          this->ShowMainMenu();
                                      });
  this->addChild(menuButton);
  menuButton->setPosition(Vec2(kButtonSize/2.f, visibleSize.height - kButtonSize/2.f));
  menuButton->setScale(kButtonScale);
  m_menuButton = menuButton;

  CreateToolBar();
  CreateSpeedToolBar();
  m_toolbarNode->setPosition(Vec2(origin.x + visibleSize.width,
                                  origin.y + visibleSize.height));
  m_speedToolbar->setPosition(Vec2(origin.x + visibleSize.width,
                                   origin.y));
  
  // shaider programm
  auto p = GLProgram::createWithFilenames("generic.vsh", "game.glsl");
  
  m_renderTexture = RenderTexture::create(visibleSize.width, visibleSize.height);
  addChild(m_renderTexture);
  
  m_rendTexSprite = Sprite::create();
  m_rendTexSprite->setTexture(m_renderTexture->getSprite()->getTexture());
  m_rendTexSprite->setTextureRect(Rect(0, 0,
                                       m_rendTexSprite->getTexture()->getContentSize().width,
                                       m_rendTexSprite->getTexture()->getContentSize().height));
  m_rendTexSprite->setPosition(Point::ZERO);
  m_rendTexSprite->setAnchorPoint(Point::ZERO);
  m_rendTexSprite->setFlippedY(true);
  addChild(m_rendTexSprite);
  
  m_rendTexSprite->setGLProgram(p);
  
  return true;
}

void PartialMapScene::ShowMainMenu()
{
  if (!m_mainMenu)
  {
    m_mainMenu = std::make_shared<MainMenu>();
    m_mainMenu->Init(std::bind(&PartialMapScene::ShowOptionsMenu, this),
                     std::bind(&PartialMapScene::Exit, this),
                     std::bind(&PartialMapScene::ShowMainScreen, this));
  }
  
  SetCurrentMenu(m_mainMenu);
}

void PartialMapScene::ShowOptionsMenu()
{
  if (!m_optionsMenu)
  {
    m_optionsMenu = std::make_shared<OptionsMenu>();
    m_optionsMenu->Init(std::bind(&PartialMapScene::ConfirmNewOptions, this),
                        std::bind(&PartialMapScene::CancelOptionSelection, this),
                        std::bind(&PartialMapScene::ShowSaveAsMenu, this),
                        std::bind(&PartialMapScene::ShowLoadMenu, this));
  }
  
  SetCurrentMenu(m_optionsMenu);
}

void PartialMapScene::ShowSaveAsMenu()
{
  if (!m_saveConfigMenu)
  {
    m_saveConfigMenu = std::make_shared<SaveConfigMenu>();
    m_saveConfigMenu->Init(std::bind(&PartialMapScene::ShowOptionsMenu, this),
                           std::bind(&PartialMapScene::ShowOptionsMenu, this));
  }
  
  SetCurrentMenu(m_saveConfigMenu);
}

void PartialMapScene::ShowLoadMenu()
{
  if (!m_loadConfigMenu)
  {
    m_loadConfigMenu = std::make_shared<LoadConfigMenu>();
    m_loadConfigMenu->Init(std::bind(&PartialMapScene::ShowOptionsMenu, this),
                           std::bind(&PartialMapScene::ShowOptionsMenu, this));
  }
  
  SetCurrentMenu(m_loadConfigMenu);
}

void PartialMapScene::SetCurrentMenu(const std::shared_ptr<IFullScreenMenu> menu)
{
  if (m_currenMenu == menu)
  {
    return;
  }
  
  if (m_currenMenu)
  {
    m_currenMenu->Hide();
  }
  
  m_currenMenu = menu;
  
  m_currenMenu->ShowInView(this);
  m_pause = true;
}

void PartialMapScene::Exit()
{
  exit(0);
}

void PartialMapScene::ShowMainScreen()
{
  m_pause = false;
  m_currenMenu->Hide();
  m_currenMenu = nullptr;
  m_optionsMenu = nullptr;
  m_mainMenu = nullptr;
  m_loadConfigMenu = nullptr;
  m_saveConfigMenu = nullptr;
}

void PartialMapScene::ConfirmNewOptions()
{
  m_restartManagerFromOptionMenu = true;
  ShowMainScreen();
}

void PartialMapScene::CancelOptionSelection()
{
  if (m_mainMenu)
  {
    ShowMainMenu();
  }
  else
  {
    ShowMainScreen();
  }
}

void PartialMapScene::SetBackgroundPosition(float animationDuration)
{
#warning don't forget
  return;
  
  float scaleRatio = (m_mapScale - kMinMapScale)/(kMaxMapScale - kMinMapScale);
  float bgScale = scaleRatio * (kMaxBgScale - kMinBgScale) + kMinBgScale;
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  
  float bgAspect = 1.0;
  {
    bgAspect = AspectToFill(m_bg->getContentSize(), visibleSize);
  }
  
  Vec2 centralPos = Vec2(visibleSize.width/2.f, visibleSize.height/2.f);
  
  Size mapSize = m_viewport->GetTotalMapSize();
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
    m_menuButton->setPosition(Vec2(kButtonSize/2.f, visibleSize.height - kButtonSize/2.f));
    if (m_currenMenu) m_currenMenu->Resize(visibleSize);
  }
  
  Layer::visit(renderer, parentTransform, parentFlags);
  
//  m_renderTexture->beginWithClear(0, 0, 0, 0);
//  for (auto child : getChildren())
//  {
//    if (child != m_renderTexture && child != m_rendTexSprite)
//      child->visit(renderer, parentTransform, parentFlags);
//  }
//  m_renderTexture->end();
//  
//  m_rendTexSprite->visit(renderer, parentTransform, parentFlags);
}

void PartialMapScene::timerForUpdate(float dt)
{
  if (m_pause) return;
  
  if(!m_viewport->IsAvailable()) return;
  
  if (m_restartManagerFromOptionMenu)
  {
    delete m_viewport;
    
    komorki::ConfigManager::GetInstance()->ApplyPendingConfig();
    
    CreateMap();
    m_restartManagerFromOptionMenu = false;
    return;
  }
  
  if (m_stopManager)
  {
    delete m_viewport;
    CreateMap();
    m_stopManager = false;
    return;
  }

  if (m_speed == eSpeedNormal || m_speed == eSpeedDouble)
  {
    if (m_prevSpeed == eSpeedWarp)
    {
      m_viewport->Reset();
    }
    
    float updateTime = m_speed == eSpeedNormal ? m_updateTime : m_updateTime * 0.5;
    float updateTimeEstimated = updateTime;
    m_viewport->UpdateAsync(updateTimeEstimated);
    
    if (updateTimeEstimated - updateTime > 2)
    {
      updateTimeEstimated = updateTime;
    }
    schedule(schedule_selector(PartialMapScene::timerForUpdate), updateTimeEstimated, kRepeatForever, updateTimeEstimated);
  }
  else if (m_speed == eSpeedMax)
  {
    float updateTimeEstimated = m_updateTime;
    m_viewport->UpdateAsync(updateTimeEstimated);
    schedule(schedule_selector(PartialMapScene::timerForUpdate), 0, kRepeatForever, 0);
  }
  
  m_prevSpeed = m_speed;
}

void PartialMapScene::timerForViewportUpdate(float dt)
{
//  m_viewport->Calculate();
}

void PartialMapScene::timerForMove(float dt)
{
  if (m_currenMenu) return;
  
  Vec2 moveDirection = m_moveDirection;
  
  float scaleRatio = (m_mapScale - kMinMapScale)/(kMaxMapScale - kMinMapScale);
  float moveStepCorrected = scaleRatio * (kMoveStep*5.0f - kMoveStep) + kMoveStep;
  
  moveDirection *= moveStepCorrected;
  
  Move(moveDirection, dt);
}

void PartialMapScene::Zoom(float direction)
{
#warning don't forget
  return;
  
  if(m_mapScale + direction <= 0.1)
    return;
  if (m_mapScale + direction >= 3.0)
    return;
  
  m_mapScale += direction;
  
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
  Zoom(+kZoomStep);
}

void PartialMapScene::ZoomOut()
{
  Zoom(-kZoomStep);
}

void PartialMapScene::Move(const Vec2& direction, float animationDuration)
{
  if (animationDuration == 0.0f)
  {
    m_viewport->Move(direction);
//    SetBackgroundPosition(0.0f);
  }
  else
  {
    m_viewport->Move(direction);
//    SetBackgroundPosition(animationDuration);
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
                                          this->SetSpeed(eSpeedMax);
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
                                            m_viewport->CleanMap();
                                        });
  m_toolbarNode->addChild(cleanAllButton);
  cleanAllButton->setPosition(Vec2(-kButtonSize/2.f, -kButtonSize/2.f -2*kButtonSize));
  cleanAllButton->setScale(kButtonScale);
  
  auto restartButton = ui::Button::create("restartButton.png", "restartButton_hl.png");
  restartButton->addTouchEventListener([this](Ref*,ui::Widget::TouchEventType controlEvent)
                                        {
                                          if (controlEvent == ui::Widget::TouchEventType::ENDED)
                                            m_stopManager = true;
                                        });
  m_toolbarNode->addChild(restartButton);
  restartButton->setPosition(Vec2(-kButtonSize/2.f, -kButtonSize/2.f -3*kButtonSize));
  restartButton->setScale(kButtonScale);
  
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
  m_viewport->StopHightlighting();
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
  if (m_speed == eSpeedMax)
    m_speed10Button->loadTextureNormal("speed10.png");
  
  m_speed = speed;
  
  if (m_speed == eSpeedNormal)
    m_speed1Button->loadTextureNormal("speed1_sel.png");
  if (m_speed == eSpeedDouble)
    m_speed2Button->loadTextureNormal("speed2_sel.png");
  if (m_speed == eSpeedMax)
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
    return komorki::eCellTypeSalad;
  if (m_brushMode == eBrushModeImprovedSalad)
    return komorki::eCellTypeImprovedSalad;
  
  assert(0);
  return komorki::eCellTypeGreen;
}

float PartialMapScene::AspectToFit(const Size& source, const Size& target)
{
  float wAspect = target.width/source.width;
  float hAspect = target.height/source.height;
  return MIN(wAspect, hAspect);
}

float PartialMapScene::AspectToFill(const Size& source, const Size& target)
{
  float wAspect = target.width/source.width;
  float hAspect = target.height/source.height;
  return MAX(wAspect, hAspect);
}

