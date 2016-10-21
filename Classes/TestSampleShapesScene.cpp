//
//  TestSampleShapesScene.cpp
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#include "TestSampleShapesScene.h"
#include "CellCanvasSprite.h"
#include "LoadingScene.h"
#include "SharedUIData.h"
#include "ShapeAnalizer.hpp"


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
  
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  
  auto originalBuffer = std::make_shared<Buffer2D<bool>>(1, 1);
  originalBuffer->Fill(false);

  originalBuffer->Set(0, 0, true);
//  originalBuffer->Set(0, 1, true);
//  originalBuffer->Set(1, 1, true);
//  originalBuffer->Set(1, 2, true);
//  originalBuffer->Set(1, 3, true);
//  originalBuffer->Set(2, 4, true);
  
  komorki::ShapeAnalizer analizer(originalBuffer);
  
  
//  m_info = CreateLabel("Loading", Vec2(visibleSize.width / 2, visibleSize.height / 2));
//  addChild(m_info, 999);
  
  auto cellCanvas = new CellCanvasSprite();
  cellCanvas->init();
  
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 32; ++j) {
      cellCanvas->SetBuffer(analizer.m_result, komorki::Vec2(i * 4, j * 4));
    }
  }
  
  addChild(cellCanvas);
  
  auto renderer = _director->getRenderer();
  auto& parentTransform = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
  
  auto rt = RenderTexture::create(512,
                                  512);
  
  rt->begin();
  cellCanvas->visit(renderer, parentTransform, true);
  rt->end();
  
  std::string mapName = mapDirName + "/cells";
  mapName += ".png";
  
  rt->saveToFile(mapName, true, [&](RenderTexture*, const std::string& image)
                 {
//                    m_info->setString("Loading viewport");

                    schedule(schedule_selector(TestSampleShapesScene::CreateViewport), 0, 1, 0);
                 });
  
//  cellCanvas->setPosition(Vec2(visibleSize.width/3, visibleSize.height/3));
//  cellCanvas->setPosition(Vec2(visibleSize.width/3, visibleSize.height/3));
  
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
//  auto sharedUIData = komorki::ui::SharedUIData::getInstance();
//  sharedUIData->cellsTexture =
  
  auto mapScene = LoadingScene::createScene();
  Director::getInstance()->replaceScene(mapScene);
}

cocos2d::Label* TestSampleShapesScene::CreateLabel(const char* text, const cocos2d::Vec2& offset) const
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
