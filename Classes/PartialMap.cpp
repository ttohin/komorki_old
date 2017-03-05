//
//  PartialMap.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "PartialMap.h"
#include "StaticLightsLayer.h"
#include "DynamicLightsLayer.h"
#include "PixelWorld.h"
#include "Utilities.h"
#include "UIConfig.h"
#include <stdio.h>
#include <sstream> //for std::stringstream
#include <string>  //for std::string
#include "SharedUIData.h"
#include "ObjectContext.h"
#include "CellContext.h"
#include "AmorphCellContext.h"
#include "TerrainBatchSprite.h"
#include "TerrainSprite.h"
#include "UICommon.h"
#include "Logging.h"
#include "SpriteBatch.h"

#define DEBUG_PARTIAL_MAP
#ifdef DEBUG_PARTIAL_MAP
#define LOG_W(...) KOMORKI_LOG(__VA_ARGS__)
#else
#define LOG_W(...) {}
#endif

namespace komorki
{
  namespace graphic
  {
    uint PartialMap::instanceCounter = 0;
    
    PartialMap::PartialMap()
    {
      instanceCounter += 1;
      
      LOG_W("%s, %s instanceCounter %d", __FUNCTION__, Description().c_str(), instanceCounter);
    }
    
    PartialMap::~PartialMap()
    {
      instanceCounter -= 1;
      
      LOG_W("%s, %s instanceCounter %d", __FUNCTION__, Description().c_str(), instanceCounter);
      
      m_cellMap->removeFromParentAndCleanup(true);
      m_background->removeFromParentAndCleanup(true);
      m_lightOverlay->removeFromParentAndCleanup(true);
      m_terrainSprite->removeFromParentAndCleanup(true);
      m_terrainBgSprite->removeFromParentAndCleanup(true);
    }
    
    bool PartialMap::InitF(int a,
                          int b,
                          int width,
                          int height,
                          IPixelWorld* provider,
                          cocos2d::Node* superView,
                          cocos2d::Node* lightNode,
                          const cocos2d::Vec2& offset)
    {
      ChangeAABB(a, b, width, height);
      m_width = width;
      m_height = height;
      m_provider = provider;
      

      
      m_cellMap = new SpriteBatch(); m_cellMap->autorelease();
      m_background = new SpriteBatch();  m_background->autorelease();
      m_terrainSprite = TerrainSprite::create(a, b, width, height, "fg");
      m_terrainBgSprite = TerrainSprite::create(a, b, width, height, "bg");
      m_lightOverlay = StaticLightsLayer::create(a, b, width, height, "light_map");
      
      m_cellMap->setName("m_cellMap " + Description());
      m_background->setName("m_background " + Description());
      m_terrainSprite->setName("m_terrainSprite " + Description());
      m_terrainBgSprite->setName("m_terrainBgSprite " + Description());
      m_lightOverlay->setName("m_lightOverlay " + Description());
      
      std::string writablePath = cocos2d::FileUtils::getInstance()->getWritablePath();
      std::string cellTextureFileName = "Komorki/tmp/cells.png";
      m_cellMap->init(writablePath + cellTextureFileName);
      m_background->init(writablePath + cellTextureFileName);
      
      m_cellMap->setPosition(offset);
      m_lightOverlay->setPosition(offset);
      m_background->setPosition(offset);
      m_terrainSprite->setPosition(offset);
      m_terrainBgSprite->setPosition(offset);
      
      superView->addChild(m_terrainBgSprite, -1);
      superView->addChild(m_background, 0);
      superView->addChild(m_cellMap, 1);
      superView->addChild(m_terrainSprite, 2);
      lightNode->addChild(m_lightOverlay, 3);
      
      if (kRandomColorPerPartialMap)
        m_terrainBgSprite->setColor(randomColor(100, 255));
      return true;
    }
    
    void PartialMap::EnableSmallAnimations(bool enable)
    {
      if (m_enableSmallAnimations == enable) {
        return;
      }
      
      m_enableSmallAnimations = enable;
      
      for (int i = m_a1; i < m_a2; ++i)
      {
        for (int j = m_b1; j < m_b2; ++j)
        {
          auto pd = m_provider->GetDescriptor(i, j);
          if (pd->m_type == GreatPixel::CreatureType)
          {
            if (pd->m_cellDescriptor->parent == pd)
            {
              // We are creating map current cell is incomming
              if (pd->m_cellDescriptor->userData != nullptr)
              {
                auto context = static_cast<ObjectContext*>(pd->m_cellDescriptor->userData);
                context->EnableSmallAnimations(m_enableSmallAnimations);
              }
            }
          }
        }
      }
    }
    
    void PartialMap::EnableAnimations(bool enable)
    {
      m_enableAnimations = enable;
    }
    
    void PartialMap::Transfrorm(const cocos2d::Vec2& pos, float scale)
    {
      m_cellMap->setPosition(pos);
      m_background->setPosition(pos);
      m_lightOverlay->setPosition(pos);
      m_terrainSprite->setPosition(pos);
      m_terrainBgSprite->setPosition(pos);
      
      m_cellMap->setScale(scale);
      m_background->setScale(scale);
      m_lightOverlay->setScale(scale * kLightMapScale);
      m_terrainSprite->setScale(scale * 4.f);
      m_terrainBgSprite->setScale(scale * 4.f);
    }
    
    void PartialMap::ChangeAABB(int a, int b, int width, int height)
    {
      m_a1 = a;
      m_a2 = a + width;
      m_b1 = b;
      m_b2 = b + height;
    }
    
    std::string PartialMap::Description()
    {
      char buf[1024];
      snprintf(buf, 1024 - 1, "%p x:[%d,%d] y:[%d,%d] %p", this, m_a1, m_a2, m_b1, m_b2, m_cellMap);
      return std::string(buf);
    }
  }
}
