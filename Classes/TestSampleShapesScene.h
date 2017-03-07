//
//  TestSampleShapesScene.h
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#pragma once

#include <cocos2d.h>

USING_NS_CC;

namespace komorki
{
  namespace ui
  {
    class TestSampleShapesScene : public cocos2d::Layer
    {
    public:
      virtual bool init();
      virtual ~TestSampleShapesScene(){}
      static cocos2d::Scene* createScene()
      {
        auto scene = cocos2d::Scene::create();
        
        TestSampleShapesScene *pRet = new(std::nothrow) TestSampleShapesScene();
        if (pRet && pRet->init())
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
      
  cocos2d::LabelProtocol* CreateLabel(const char* text, const cocos2d::Vec2& offset);
  void timerForUpdate(float dt);
  void LoadTerrainMaps(float dt);
  void CreateViewport(float dt);
  
private:
  cocos2d::Label* m_info;
  std::vector<std::string> m_mapList;
    };
  }
}
