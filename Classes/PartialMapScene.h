#ifndef __PARTIALMAP_SCENE_H__
#define __PARTIALMAP_SCENE_H__

#include <vector>

#include "cocos2d.h"
#include "UIButton.h"
#include "OptionsMenu.h"
#include "MainMenu.h"
#include "LoadConfigMenu.h"
#include "SaveConfigMenu.h"
#include "IFullScreenMenu.h"
#include "Viewport.h"
#include "PixelProviderConfig.h"

class PartialMapScene : public cocos2d::Layer, cocos2d::TextFieldDelegate
{
public:

  virtual bool init(const komorki::ui::Viewport::Ptr& viewport);
  static cocos2d::Scene* createScene(const komorki::ui::Viewport::Ptr& viewport)
  {
    auto scene = cocos2d::Scene::create();
    
    PartialMapScene *pRet = new(std::nothrow) PartialMapScene();
    if (pRet && pRet->init(viewport))
    {
      pRet->autorelease();
      scene->addChild(pRet);
      return scene;
    }
    else
    {
      delete pRet;
      pRet = NULL;
      return NULL;
    }
  }
  
  virtual ~PartialMapScene(){}
  
private:
  
  enum BrushMode
  {
    eBrushModeGreen,
    eBrushModeHunter,
    eBrushModeSalad,
    eBrushModeImprovedSalad,
    eBrushModeGround
  };
  
  enum TouchMode
  {
    eTouchModeMove,
    eTouchModeBrush
  };
  
  enum Speed
  {
    eSpeedNormal,
    eSpeedDouble,
    eSpeedMax,
    eSpeedWarp
  };
  
  TouchMode m_touchMode;
  BrushMode m_brushMode;
  Speed m_speed;
  Speed m_prevSpeed;
  bool m_eraseBrush;
  
//  PixelMapManager* m_mapManager;
  komorki::ui::Viewport::Ptr m_viewport;
  cocos2d::Node* m_rootNode;
  cocos2d::Sprite* m_bg;
  cocos2d::Sprite* m_brush;
  cocos2d::Sprite* m_cross;
  cocos2d::Sprite* m_microscope;
  cocos2d::ui::Button* m_green;
  cocos2d::ui::Button* m_hunter;
  cocos2d::ui::Button* m_salad;
  cocos2d::ui::Button* m_improvedSalad;
  cocos2d::ui::Button* m_eraseButton;
  cocos2d::ui::Button* m_brushButton;
  cocos2d::ui::Button* m_speed1Button;
  cocos2d::ui::Button* m_speed2Button;
  cocos2d::ui::Button* m_speed10Button;
  cocos2d::ui::Button* m_menuButton;
  cocos2d::Node* m_toolbarNode;
  cocos2d::Node* m_cellsSelectoToolbar;
  cocos2d::Node* m_speedToolbar;
  cocos2d::Node* m_menuNode;
  std::shared_ptr<OptionsMenu> m_optionsMenu;
  std::shared_ptr<MainMenu> m_mainMenu;
  std::shared_ptr<LoadConfigMenu> m_loadConfigMenu;
  std::shared_ptr<SaveConfigMenu> m_saveConfigMenu;
  std::shared_ptr<IFullScreenMenu> m_currenMenu;
  cocos2d::RenderTexture* m_mainTexture;
  cocos2d::RenderTexture* m_lightTexture;
  cocos2d::Sprite* m_rendTexSprite;
  cocos2d::Sprite* m_debugView;
  
  float m_mapScale;
  cocos2d::Vec2 m_mapPos;
  cocos2d::Vec2 m_moveDirection;
  float m_updateTime;
  bool m_pause;
  bool m_stopManager;
  bool m_restartManagerFromOptionMenu;
  void ZoomIn();
  void ZoomOut();
  void Zoom(float direction);
  void Move(const cocos2d::Vec2& direction, float animationDuration = 0.0f);
  
  void timerForUpdate(float dt);
  void timerForViewportUpdate(float dt);
  void timerForMove(float dt);
  void CreateMap(const komorki::ui::Viewport::Ptr& viewport);
  
  void ShowMainMenu();
  void ShowOptionsMenu();
  void ShowSaveAsMenu();
  void ShowLoadMenu();
  void SetCurrentMenu(const std::shared_ptr<IFullScreenMenu> menu);
  void Exit();
  void ShowMainScreen();
  
  void ConfirmNewOptions();
  void CancelOptionSelection();
  
  void SetBackgroundPosition(float animationDuration = 0.0f);
  void CreateSpeedToolBar();
  void CreateToolBar();
  
  void SelectBrushTouchDownAction(Ref *sender, cocos2d::ui::Widget::TouchEventType controlEvent);
  void SelectRemoveBrushTouchDownAction(Ref *sender, cocos2d::ui::Widget::TouchEventType controlEvent);
  void SetBrushEnabled(bool enabled);
  void SetDrawingMode(bool enabled);
  void SetBrushMode(BrushMode mode);
  void SetEraseMode(bool enabled);
  void SetSpeed(Speed speed);
  komorki::CellType GetCurretnCellType();
  void CreateRenderTextures(const cocos2d::Size& size);
  
  float AspectToFill(const cocos2d::Size& source, const cocos2d::Size& target);
  float AspectToFit(const cocos2d::Size& source, const cocos2d::Size& target);
  
  std::shared_ptr<komorki::Config> m_config;
  
  std::vector<cocos2d::Touch*> zoomToches;
  
  void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags);
};

#endif // __PARTIALMAP_SCENE_H__
