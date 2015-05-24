//
//  ListView.h
//  komorki
//
//  Created by ttohin on 25.04.15.
//
//

#ifndef komorki_ListView_h
#define komorki_ListView_h

#include "cocos2d.h"
#include "CocosGUI.h"

namespace komorki
{
  class ListView : public cocos2d::ui::ListView
  {
  public:
    
    ListView() : cocos2d::ui::ListView(), _prevOffset(0) {}
    
    bool isScrolling()
    {
      return false;
    }
    /**
     * Allocates and initializes.
     */
    static ListView* create()
    {
      ListView* widget = new (std::nothrow) ListView();
      if (widget && widget->init())
      {
        widget->autorelease();
        return widget;
      }
      CC_SAFE_DELETE(widget);
      return nullptr;
    }
    
    void endRecordSlidAction()
    {
      if(_acceleration <= 1.0f)
      {
        _slidTime = 0;
      }
      cocos2d::ui::ListView::endRecordSlidAction();
    }
    
    void handleMoveLogic(cocos2d::Touch *touch)
    {
      if (_bePressed)
      {
        _prevPos = this->convertToNodeSpace(touch->getLocation());
      }
      cocos2d::ui::ListView::handleMoveLogic(touch);
    }
    
    void recordSlidTime(float dt)
    {
      if (_bePressed)
      {
        cocos2d::Vec2 touchEndPositionInNodeSpace = _prevPos;
        cocos2d::Vec2 touchBeganPositionInNodeSpace = this->convertToNodeSpace(_touchBeganPosition);
        float totalDis = touchEndPositionInNodeSpace.y - touchBeganPositionInNodeSpace.y;
       
        float acceleration = std::fabs(_prevOffset - totalDis);
        _prevOffset = totalDis;
        _acceleration = acceleration;
        
        _slidTime += dt;
      }
    }
   
  protected:
    cocos2d::Vec2 _prevPos;
    float _prevOffset;
    float _acceleration;
    
  };
}


#endif
