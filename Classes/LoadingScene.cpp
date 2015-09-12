//
//  LoadingScene.cpp
//  prsv
//
//  Created by ttohin on 12.09.15.
//
//

#include "LoadingScene.h"
#include "PartialMapScene.h"
#include "Common.h"
#include "UIConfig.h"
#include "TerrainBatchSprite.h"


bool LoadingScene::init()
{
  if ( !Layer::init() )
  {
    return false;
  }
  
  m_viewport = nullptr;
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  
 
  m_info = CreateLabel("Loading", Vec2(visibleSize.width / 2, visibleSize.height / 2));
  addChild(m_info, 999);
  
  m_mapLoader = std::make_shared<AsyncMapLoader>();
  
  Director::getInstance()->getTextureCache()->addImage("tile_32x32.png");
  Director::getInstance()->getTextureCache()->addImage("ground.png");
  Director::getInstance()->getTextureCache()->addImage("debugFrames.png");
  
  schedule(schedule_selector(LoadingScene::timerForUpdate), 0, kRepeatForever, 0);
  
  return true;
}

void LoadingScene::timerForUpdate(float dt)
{
  if (!m_mapLoader)
  {
    return;
  }
  
  m_info->setString(m_mapLoader->GetCurrentJobString());
  
  if (m_mapLoader->IsAvailable())
  {
    m_provider = m_mapLoader->GetProvider();
    m_info->setString("Loading terrain");
    m_mapLoader.reset();
    schedule(schedule_selector(LoadingScene::LoadTerrainMaps), 0, 0, 0);
  }
}

void LoadingScene::LoadTerrainMaps(float dt)
{
  auto terrain = m_provider->GetTerrain();
  
  komorki::Rect totalSize = {{0, 0}, {terrain.ground->GetWidth(), terrain.ground->GetHeight()}};
  std::vector<komorki::Rect> mapRects;
  komorki::SplitRectOnChunks(totalSize, {{0, 0}, {0, 0}}, komorki::ui::kSegmentSize * 2, mapRects);
  
  for (const auto& rect : mapRects)
  {
    TerrainAnalizer::Result terrainAnalizerResult;
  
    terrainAnalizerResult.background = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    terrainAnalizerResult.ground = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    terrainAnalizerResult.foreground = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    
    terrain.background->SubSet(rect.origin.x, rect.origin.y, *terrainAnalizerResult.background.get());
    terrain.ground->SubSet(rect.origin.x, rect.origin.y, *terrainAnalizerResult.ground.get());
    terrain.foreground->SubSet(rect.origin.x, rect.origin.y, *terrainAnalizerResult.foreground.get());
    
    auto terrainBatch = new TerrainBatchSprite();
    terrainBatch->init(terrainAnalizerResult);
    terrainBatch->autorelease();
    
    auto renderer = _director->getRenderer();
    auto& parentTransform = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    
    int aspect = 4;
    
    auto rt = RenderTexture::create(32.f * komorki::ui::kSegmentSize / aspect,
                                    32.f * komorki::ui::kSegmentSize / aspect);
    
    terrainBatch->setScale(1.f / float(aspect));
    
    rt->begin();
    terrainBatch->visit(renderer, parentTransform, true);
    rt->end();
    
    std::string mapName = "map";
    mapName += std::to_string(rect.origin.x / 2) + "_" + std::to_string(rect.origin.y / 2);
    mapName += "_";
    mapName += std::to_string(rect.size.x / 2) + "_" + std::to_string(rect.size.y / 2);
    mapName += ".png";
    
    cocos2d::log("%s", mapName.c_str());
    
    m_mapList.push_back(FileUtils::getInstance()->getWritablePath() + mapName);
    
    rt->saveToFile(mapName, true, [&](RenderTexture*, const std::string& image)
                   {
                     cocos2d::log("saved %s", image.c_str());
                     
                     m_mapList.pop_back();
                     
                     if (m_mapList.empty())
                     {
                       m_info->setString("Loading viewport");
                       schedule(schedule_selector(LoadingScene::CreateViewport), 0, 1, 0);
                     }
                     
                   });
  }
  

}

void LoadingScene::CreateViewport(float dt)
{
  auto rootNode = cocos2d::Node::create();
  rootNode->retain();
  Size visibleSize = Director::getInstance()->getVisibleSize();
  m_viewport = std::make_shared<komorki::ui::Viewport>(rootNode, visibleSize, m_provider);
  
  auto mapScene = PartialMapScene::createScene(m_viewport);
  Director::getInstance()->replaceScene(mapScene);
}

cocos2d::Label* LoadingScene::CreateLabel(const char* text, const cocos2d::Vec2& offset) const
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
  auto result = LabelAtlas::create(text, "font.png", 18, 24, 32);
  result->setPosition(offset);
  return result;
#else
  auto result = Label::createWithSystemFont(text, "Menlo", 24, Size::ZERO, TextHAlignment::RIGHT);
//  auto result = Label::createWithCharMap("font.png", 18, 24, 32);
  result->setString(text);
  result->setPosition(offset);
  result->setHorizontalAlignment(TextHAlignment::LEFT);
  return result;
#endif
}