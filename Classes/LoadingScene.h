//
//  LoadingScene.h
//  prsv
//
//  Created by ttohin on 12.09.15.
//
//

#ifndef __prsv__LoadingScene__
#define __prsv__LoadingScene__

#include "AsyncMapLoader.h"
USING_NS_CC;

class LoadingScene: public cocos2d::Layer
{
public:
  virtual bool init();
  virtual ~LoadingScene(){}
  static cocos2d::Scene* createScene()
  {
    auto scene = cocos2d::Scene::create();
    
    LoadingScene *pRet = new(std::nothrow) LoadingScene();
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
 
  
  cocos2d::Label* CreateLabel(const char* text, const cocos2d::Vec2& offset) const;
  void timerForUpdate(float dt);
  void LoadTerrainMaps(float dt);
  void CreateViewport(float dt);
 
private:
  cocos2d::Label* m_info;
  std::shared_ptr<AsyncMapLoader> m_mapLoader;
  std::shared_ptr<komorki::IPixelDescriptorProvider> m_provider;
  komorki::ui::Viewport::Ptr m_viewport;
  std::vector<std::string> m_mapList;
  
};




#endif /* defined(__prsv__LoadingScene__) */
