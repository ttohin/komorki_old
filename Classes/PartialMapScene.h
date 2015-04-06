#ifndef __PARTIALMAP_SCENE_H__
#define __PARTIALMAP_SCENE_H__

#include "cocos2d.h"
#include "PixelMapManager.h"
#include "UIButton.h"
#include <vector>

class PartialMapScene : public cocos2d::Layer, cocos2d::TextFieldDelegate
{
public:
  static cocos2d::Scene* createScene(const char* configPath);
  virtual bool init();
  CREATE_FUNC(PartialMapScene);
  
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
    eSpeedWarp
  };
  
  TouchMode m_touchMode;
  BrushMode m_brushMode;
  Speed m_speed;
  Speed m_prevSpeed;
  bool m_eraseBrush;
  
  PixelMapManager* m_mapManager;
  Node* m_rootNode;
  Sprite* m_bg;
  Sprite* m_brush;
  Sprite* m_cross;
  Sprite* m_microscope;
  ui::Button* m_green;
  ui::Button* m_hunter;
  ui::Button* m_salad;
  ui::Button* m_improvedSalad;
  ui::Button* m_eraseButton;
  ui::Button* m_brushButton;
  ui::Button* m_speed1Button;
  ui::Button* m_speed2Button;
  ui::Button* m_speed10Button;
  Node* m_toolbarNode;
  Node* m_cellsSelectoToolbar;
  Node* m_speedToolbar;
  
  float m_mapScale;
  Vec2 m_mapPos;
  Vec2 m_moveDirection;
  float m_updateTime;
  bool m_stopManager;
  void ZoomIn();
  void ZoomOut();
  void Zoom(float direction);
  void Move(const Vec2& direction, float animationDuration = 0.0f);
  
  void timerForUpdate(float dt);
  void timerForMove(float dt);
  void CreateMap();
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
  
  std::vector<cocos2d::Touch*> zoomToches;
  
  void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags);
};

#endif // __PARTIALMAP_SCENE_H__
