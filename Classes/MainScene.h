#ifndef __PARTIALMAP_SCENE_H__
#define __PARTIALMAP_SCENE_H__

#include <vector>

#include "cocos2d.h"
#include "cocos2d/cocos/ui/CocosGUI.h"
#include "MainMenu.h"
#include "IFullScreenMenu.h"
#include "Viewport.h"


class MainScene : public cocos2d::Layer, cocos2d::TextFieldDelegate
{
public:

  virtual bool init(const komorki::graphic::Viewport::Ptr& viewport);
  static cocos2d::Scene* createScene(const komorki::graphic::Viewport::Ptr& viewport)
  {
    auto scene = cocos2d::Scene::create();
    
    MainScene *pRet = new(std::nothrow) MainScene();
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
  
  virtual ~MainScene(){}
  
private:
  
  enum Speed
  {
    eSpeedPause,
    eSpeedNormal,
    eSpeedDouble,
    eSpeedMax,
    eSpeedWarp
  };
  
  Speed m_speed;
  Speed m_prevSpeed;
  
  komorki::graphic::Viewport::Ptr m_viewport;
  cocos2d::Node* m_rootNode;
  cocos2d::Sprite* m_bg;
  cocos2d::ui::Button* m_pauseButton;
  cocos2d::ui::Button* m_speed1Button;
  cocos2d::ui::Button* m_speed2Button;
  cocos2d::ui::Button* m_speed10Button;
  cocos2d::ui::Button* m_menuButton;
  cocos2d::Node* m_speedToolbar;
  cocos2d::Node* m_menuNode;
  std::shared_ptr<MainMenu> m_mainMenu;
  std::shared_ptr<IFullScreenMenu> m_currenMenu;
  cocos2d::RenderTexture* m_mainTexture;
  cocos2d::RenderTexture* m_lightTexture;
  cocos2d::Sprite* m_rendTexSprite;
  cocos2d::Sprite* m_debugView;
  std::shared_ptr<komorki::Config> m_config;
  
  float m_mapScale;
  cocos2d::Vec2 m_mapPos;
  cocos2d::Vec2 m_moveDirection;
  float m_updateTime;
  bool m_pause;
  bool m_stopManager;
  
  
  void ZoomIn();
  void ZoomOut();
  void Zoom(float direction);
  void Move(const cocos2d::Vec2& direction, float animationDuration = 0.0f);
  
  void timerForUpdate(float dt);
  void timerForViewportUpdate(float dt);
  void timerForMove(float dt);
  void CreateMap(const komorki::graphic::Viewport::Ptr& viewport);
  
  void ShowMainMenu();
  void SetCurrentMenu(const std::shared_ptr<IFullScreenMenu> menu);
  void Exit();
  void ShowMainScreen();
  
  void CreateSpeedToolBar();
  
  void SetSpeed(Speed speed);
  void CreateRenderTextures(const cocos2d::Size& size);
  
  float AspectToFill(const cocos2d::Size& source, const cocos2d::Size& target);
  float AspectToFit(const cocos2d::Size& source, const cocos2d::Size& target);
  
  void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags);
};

#endif // __PARTIALMAP_SCENE_H__
