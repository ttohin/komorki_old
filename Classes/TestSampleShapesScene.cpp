//
//  TestSampleShapesScene.cpp
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#include "TestSampleShapesScene.h"
#include "LoadingScene.h"
#include "SharedUIData.h"
#include "CellCanvasSprite.h"
#include "ShapeAnalizer.hpp"
#include "ShapesGenerator.h"
#include <iostream>
#include "UIConfig.h"


CellCanvasSprite* CreateSimpleCanvasWithSqareCells()
{
  auto originalBuffer = std::make_shared<Buffer2D<bool>>(1, 1);
  originalBuffer->Fill(false);
  
  originalBuffer->Set(0, 0, true);
  //  originalBuffer->Set(0, 1, true);
  //  originalBuffer->Set(1, 1, true);
  //  originalBuffer->Set(1, 2, true);
  //  originalBuffer->Set(1, 3, true);
  //  originalBuffer->Set(2, 4, true);
  
  unsigned int scale = 4;
  
  komorki::ShapeAnalizer analizer(originalBuffer, scale);
  
  //  m_info = CreateLabel("Loading", Vec2(visibleSize.width / 2, visibleSize.height / 2));
  //  addChild(m_info, 999);
  
  auto cellCanvas = new CellCanvasSprite();
  cellCanvas->init();
  
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 32; ++j) {
      cellCanvas->SetBuffer(analizer.m_result, komorki::Vec2(i * scale, j * scale), true);
    }
  }
  
  return cellCanvas;
}

CellCanvasSprite* CreateExperimentalCanvas()
{
  auto originalBuffer = std::make_shared<Buffer2D<bool>>(3, 5);
  originalBuffer->Fill(false);
  
  originalBuffer->Set(0, 0, true);
  originalBuffer->Set(0, 1, true);
  originalBuffer->Set(1, 1, true);
  originalBuffer->Set(1, 2, true);
  originalBuffer->Set(1, 3, true);
  originalBuffer->Set(2, 4, true);
  
  unsigned int scale = 4;
  
  komorki::ShapeAnalizer analizer(originalBuffer, scale);
  
  auto cellCanvas = new CellCanvasSprite();
  cellCanvas->init();
  
  cellCanvas->SetBuffer(analizer.m_result, komorki::Vec2(0, 0), true);
  cellCanvas->SetBuffer(analizer.m_result, komorki::Vec2(5 * scale, 0), true);
  
  return cellCanvas;
}

CellCanvasSprite* CreateCanvasWithGenomsGenerator()
{
  komorki::ShapesGenerator gen;
  auto cellCanvas = new CellCanvasSprite();
  cellCanvas->init();
  
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
    
    cellCanvas->SetBuffer(analizer.m_result, komorki::Vec2(currentPos.x * scale, currentPos.y * scale), true);
    
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
    
    std::cout << "group " << groupId << " textureRext " <<
    textureRect.origin.x << ", " << textureRect.origin.y << " " <<
    textureRect.size.width << ", " << textureRect.size.height <<
    std::endl << originalBuffer->Description() << std::endl;
  }
  
  return cellCanvas;
}

bool TestSampleShapesScene::init()
{
  if ( !Layer::init() )
  {
    return false;
  }
  
  auto sharedFileUtils = FileUtils::getInstance();
  
  const std::string mapDirName = "Komorki/tmp";
  const std::string mapDir = sharedFileUtils->getWritablePath() + mapDirName;
  bool ok = sharedFileUtils->createDirectory(mapDir);
  assert(ok && sharedFileUtils->isDirectoryExist(mapDir));
  
//  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  
  auto cellCanvas = CreateCanvasWithGenomsGenerator();
  
  addChild(cellCanvas);
  
  auto renderer = _director->getRenderer();
  auto& parentTransform = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
  
  auto rt = RenderTexture::create(komorki::graphic::kCellShapeSegments * komorki::graphic::kCellsTextureSizeInPixels.x * CellCanvasSprite::kSpriteSize,
                                  komorki::graphic::kCellShapeSegments * komorki::graphic::kCellsTextureSizeInPixels.y * CellCanvasSprite::kSpriteSize);
  
  rt->begin();
  cellCanvas->visit(renderer, parentTransform, true);
  rt->end();
  
  std::string mapName = mapDirName + "/cells";
  mapName += ".png";
  
  rt->saveToFile(mapName, true, [&](RenderTexture*, const std::string& image)
                 {
                    schedule(schedule_selector(TestSampleShapesScene::CreateViewport), 0, 1, 0);
                 });
  
  Director::getInstance()->getTextureCache()->addImage("tile_32x32.png");
  Director::getInstance()->getTextureCache()->addImage("ground.png");
  Director::getInstance()->getTextureCache()->addImage("debugFrames.png");
  
  return true;
}

void TestSampleShapesScene::timerForUpdate(float dt)
{
}

void TestSampleShapesScene::CreateViewport(float dt)
{
  auto mapScene = LoadingScene::createScene();
  Director::getInstance()->replaceScene(mapScene);
}

cocos2d::LabelProtocol* TestSampleShapesScene::CreateLabel(const char* text, const cocos2d::Vec2& offset)
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
  return result;
#endif
}
