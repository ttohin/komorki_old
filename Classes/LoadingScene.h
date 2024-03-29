//
//  LoadingScene.h
//  prsv
//
//  Created by ttohin on 12.09.15.
//
//

#pragma once

#include "AsyncMapLoader.h"


namespace komorki
{
  namespace ui
  {
    class LoadingScene: public cocos2d::Layer
    {
    public:
      virtual bool init();
      virtual ~LoadingScene();
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

  cocos2d::LabelProtocol* CreateLabel(const char* text, const cocos2d::Vec2& offset);
  void timerForUpdate(float dt);
  void SaveTerrain(const TerrainAnalizer::Result& terrainAnalizerResult,
                   const komorki::Rect& rect,
                   const std::string& prefix,
                   const std::string& mapDirName);
  void LoadTerrainMaps(float dt);
  void LoadLightMaps(float dt);
  void LoadCellShapes(float dt);
  void CreateViewport(float dt);

private:
  cocos2d::LabelProtocol* m_info;
  std::shared_ptr<AsyncMapLoader> m_mapLoader;
  std::shared_ptr<komorki::IPixelWorld> m_provider;
  komorki::graphic::Viewport::Ptr m_viewport;
  std::vector<std::string> m_mapList;

};

}
}

