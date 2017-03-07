//
//  LoadingScene.cpp
//  prsv
//
//  Created by ttohin on 12.09.15.
//
//

#include "LoadingScene.h"
#include "MainScene.h"
#include "Common.h"
#include "UIConfig.h"
#include "TerrainBatchSprite.h"
#include "StaticLightsSprite.h"

#include "SharedUIData.h"
#include "CellCanvasSprite.h"
#include "ShapeAnalizer.hpp"
#include "ShapesGenerator.h"

USING_NS_CC;

namespace komorki
{
  namespace ui
  {

bool LoadingScene::init()
{
  if ( !Layer::init() )
  {
    return false;
  }
  
  m_viewport = nullptr;
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  auto origin = Director::getInstance()->getVisibleOrigin();
  
  m_info = CreateLabel("Loading", cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));
  
  Director::getInstance()->getTextureCache()->addImage("ground.png");

  schedule(schedule_selector(LoadingScene::LoadCellShapes), 0, 0, 0);
  
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


void LoadingScene::SaveTerrain(const TerrainAnalizer::Result& terrainAnalizerResult,
                               const komorki::Rect& rect,
                               const std::string& prefix,
                               const std::string& mapDirName)
{
  auto terrainBatch = new komorki::graphic::TerrainBatchSprite();
  terrainBatch->init(terrainAnalizerResult);
  terrainBatch->autorelease();
  
  auto renderer = _director->getRenderer();
  auto& parentTransform = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
  
  int aspect = 4;
  
  auto rt = RenderTexture::create(32.f * komorki::graphic::kSegmentSize / aspect,
                                  32.f * komorki::graphic::kSegmentSize / aspect);
  
  terrainBatch->setScale(1.f / float(aspect));
  
  rt->begin();
  terrainBatch->visit(renderer, parentTransform, true);
  rt->end();
  
  std::string mapName = mapDirName + "/map";
  mapName += prefix + "_";
  mapName += std::to_string(rect.origin.x / 2) + "_" + std::to_string(rect.origin.y / 2);
  mapName += "_";
  mapName += std::to_string(rect.size.x / 2) + "_" + std::to_string(rect.size.y / 2);
  mapName += ".png";
  
  m_mapList.push_back(mapName);
  
  rt->saveToFile(mapName, true, [&](RenderTexture*, const std::string& image)
                 {
                   m_mapList.pop_back();
                   
                   if (m_mapList.empty())
                   {
                     m_info->setString("Loading light map");
                     schedule(schedule_selector(LoadingScene::LoadLightMaps), 0, 0, 0);
                   }

                 });
}

void LoadingScene::LoadTerrainMaps(float dt)
{
  auto terrain = m_provider->GetTerrain();
  
  komorki::Rect totalSize = {{0, 0}, {terrain.ground->GetWidth(), terrain.ground->GetHeight()}};
  std::vector<komorki::Rect> mapRects;
  komorki::SplitRectOnChunks(totalSize, {{0, 0}, {0, 0}}, komorki::graphic::kSegmentSize * 2, mapRects);
  
  auto sharedFileUtils = FileUtils::getInstance();
  
  const std::string mapDirName = "Komorki/tmp/terrain";
  const std::string mapDir = sharedFileUtils->getWritablePath() + mapDirName;
  bool ok = sharedFileUtils->createDirectory(mapDir);
  assert(ok && sharedFileUtils->isDirectoryExist(mapDir));
  
  for (const auto& rect : mapRects)
  {
    TerrainAnalizer::Result terrainAnalizerResult;
  
    terrainAnalizerResult.background = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    terrainAnalizerResult.ground = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    terrainAnalizerResult.foreground = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    
    terrain.ground->SubSet(rect.origin.x, rect.origin.y, *terrainAnalizerResult.ground.get());
    terrain.foreground->SubSet(rect.origin.x, rect.origin.y, *terrainAnalizerResult.foreground.get());
    
    SaveTerrain(terrainAnalizerResult, rect, "fg", mapDirName);
  }
  
  for (const auto& rect : mapRects)
  {
    TerrainAnalizer::Result terrainAnalizerResult;
    
    terrainAnalizerResult.background = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    terrainAnalizerResult.ground = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    terrainAnalizerResult.foreground = std::make_shared<Buffer2D<TerrainInfo>>(rect.size.x, rect.size.y);
    
    terrain.background->SubSet(rect.origin.x, rect.origin.y, *terrainAnalizerResult.background.get());
    
    SaveTerrain(terrainAnalizerResult, rect, "bg", mapDirName);
  }
}

void LoadingScene::LoadLightMaps(float dt)
{
  komorki::Rect totalSize = {{0, 0}, {m_provider->GetSize().x, m_provider->GetSize().y}};
  std::vector<komorki::Rect> mapRects;
  komorki::SplitRectOnChunks(totalSize, {{0, 0}, {0, 0}}, komorki::graphic::kSegmentSize, mapRects);
  
  auto sharedFileUtils = FileUtils::getInstance();
  
  const std::string mapDirName = "Komorki/tmp/light_maps";
  const std::string mapDir = sharedFileUtils->getWritablePath() + mapDirName;
  bool ok = sharedFileUtils->createDirectory(mapDir);
  assert(ok && sharedFileUtils->isDirectoryExist(mapDir));
  
  for (const auto& rect : mapRects)
  {
    auto staticLights = new StaticLightsSprite();
    staticLights->init(m_provider.get(), rect);
    staticLights->autorelease();
   
    auto renderer = _director->getRenderer();
    auto& parentTransform = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    
    int aspect = komorki::graphic::kLightMapScale;
    
    auto rt = RenderTexture::create(staticLights->textureSize().width / aspect,
                                    staticLights->textureSize().height / aspect);
    
    staticLights->setScale(1.f / float(aspect));
    
    rt->begin();
    staticLights->visit(renderer, parentTransform, true);
    rt->end();
    
    std::string mapName = mapDirName + "/light_map_";
    mapName += std::to_string(rect.origin.x) + "_" + std::to_string(rect.origin.y);
    mapName += "_";
    mapName += std::to_string(rect.size.x) + "_" + std::to_string(rect.size.y);
    mapName += ".png";
    
    m_mapList.push_back(mapName);
    
    rt->saveToFile(mapName, true, [&](RenderTexture*, const std::string& image)
                   {
                     m_mapList.pop_back();
                     
                     if (m_mapList.empty())
                     {
                       m_info->setString("Loading viewport");
                       schedule(schedule_selector(LoadingScene::CreateViewport), 0, 0, 0);
                     }
                     
                   });
  }
}

void DrawCellShapes(cocos2d::Renderer* renderer, const Mat4& parentTransform)
{
  komorki::ShapesGenerator gen;
  const komorki::Vec2 maxTextureSize = komorki::graphic::kCellsTextureSizeInPixels;
  
  komorki::Vec2 currentPos;
  komorki::PixelPos line = 0;
  
  while (1)
  {
    auto result = gen.GenerateNext();
    auto shape = result.shape;
    komorki::Rect aabb = shape->GetAABB();
    if (currentPos.x + aabb.size.x >= maxTextureSize.x)
    {
      currentPos.x = 0;
      currentPos.y = line;
    }
    if (currentPos.y + aabb.size.y >= maxTextureSize.y)
    {
      break;
    }
    
    auto cellCanvas = new CellCanvasSprite();
    cellCanvas->init();
    
    line = std::max(line, currentPos.y + aabb.size.y);
    
    auto originalBuffer = std::make_shared<Buffer2D<bool>>(aabb.size.x, aabb.size.y);
    originalBuffer->Fill(false);
    
    shape->ForEach([&](komorki::GreatPixel* pd, bool& stop)
                   {
                     auto pos = pd->GetPos();
                     pos = pos - aabb.origin;
                     originalBuffer->Set(pos.x, pos.y, true);
                   });
    
    unsigned int scale = komorki::graphic::kCellShapeSegments;
    komorki::ShapeAnalizer analizer(originalBuffer, scale);
    
    bool drowCorner = result.type != komorki::ShapeType::eShapeTypeAmorph;
    
    cellCanvas->SetBuffer(analizer.m_result, komorki::Vec2(currentPos.x * scale, currentPos.y * scale), drowCorner);
    
    cocos2d::Rect textureRect((currentPos.x) * scale * CellCanvasSprite::kSpriteSize,
                              (maxTextureSize.y - currentPos.y - aabb.size.y) * scale * CellCanvasSprite::kSpriteSize,
                              aabb.size.x * scale * CellCanvasSprite::kSpriteSize,
                              aabb.size.y * scale * CellCanvasSprite::kSpriteSize);
    
    currentPos.x += aabb.size.x;
    
    komorki::Genom::GroupIdType groupId;
    if (komorki::graphic::SharedUIData::getInstance()->m_genomsGenerator->AddShape(result, groupId))
    {
      komorki::graphic::SharedUIData::getInstance()->m_textureMap[groupId] = textureRect;
    }
    
    cellCanvas->visit(renderer, parentTransform, true);
    cellCanvas->release();
  }
}

void LoadingScene::LoadCellShapes(float dt)
{
  auto sharedFileUtils = FileUtils::getInstance();
  
  const std::string mapDirName = "Komorki/tmp";
  const std::string mapDir = sharedFileUtils->getWritablePath() + mapDirName;
  bool ok = sharedFileUtils->createDirectory(mapDir);
  assert(ok && sharedFileUtils->isDirectoryExist(mapDir));
  
  auto origin = Director::getInstance()->getVisibleOrigin();
  
  auto renderer = _director->getRenderer();
  auto& parentTransform = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
  
  auto rt = RenderTexture::create(komorki::graphic::kCellShapeSegments * komorki::graphic::kCellsTextureSizeInPixels.x * CellCanvasSprite::kSpriteSize,
                                  komorki::graphic::kCellShapeSegments * komorki::graphic::kCellsTextureSizeInPixels.y * CellCanvasSprite::kSpriteSize);
  
  rt->begin();
  DrawCellShapes(renderer, parentTransform);
  rt->end();
  
  std::string mapName = mapDirName + "/cells";
  mapName += ".png";
  
  rt->saveToFile(mapName, true, [&](RenderTexture*, const std::string& image)
                 {
                     m_mapLoader = std::make_shared<AsyncMapLoader>();
                     schedule(schedule_selector(LoadingScene::timerForUpdate), 0, kRepeatForever, 0);
                 });
}

void LoadingScene::CreateViewport(float dt)
{
  auto rootNode = cocos2d::Node::create();
  rootNode->retain();
  Size visibleSize = Director::getInstance()->getVisibleSize();
  m_viewport = std::make_shared<komorki::graphic::Viewport>(rootNode, visibleSize, m_provider);
  
  auto mapScene = MainScene::createScene(m_viewport);
  Director::getInstance()->replaceScene(mapScene);
}

cocos2d::LabelProtocol* LoadingScene::CreateLabel(const char* text, const cocos2d::Vec2& offset)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
  auto result = LabelAtlas::create(text, "font.png", 18, 24, 32);
  result->setPosition(offset);
  addChild(result);
  return result;
#else
  auto result = Label::createWithSystemFont(text, "Menlo", 24, Size::ZERO, TextHAlignment::RIGHT);
//  auto result = Label::createWithCharMap("font.png", 18, 24, 32);
  result->setString(text);
  result->setPosition(offset);
  result->setHorizontalAlignment(TextHAlignment::LEFT);
  addChild(result);
  return result;
#endif
}
  }
}
