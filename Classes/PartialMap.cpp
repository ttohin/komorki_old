//
//  PartialMap.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "PartialMap.h"
#include "DeadCellsLayer.h"
#include "PixelDebugView.h"
#include "StaticLightsLayer.h"
#include "CellsLayer.h"
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


#ifdef DEBUG_PARTIAL_MAP
#define LOG_W(...) KOMORKI_LOG(__VA_ARGS__)
#else
#define LOG_W(...) {}
#endif

namespace komorki
{
namespace graphic
{
PartialMap::PartialMap()
{
}

PartialMap::~PartialMap()
{
  
  LOG_W("%s, %s", __FUNCTION__, Description().c_str());

  
  for (auto cell : m_outgoingCells)
  {
    if (cell->userData == nullptr) {
      continue;
    }
    
    auto context = static_cast<ObjectContext*>(cell->userData);
    // Remove only context that hasn't been adopted by other pratial maps
    if (context->m_owner == this || context->m_owner == nullptr)
    {
      Vec2 pos (cell->parent->x, cell->parent->y);
      LOG_W("%s, %s delete m_outgoingCells [%s]", __FUNCTION__, Description().c_str(), pos.Description().c_str());
      cell->userData = nullptr;
      context->Destory(this);
    }
  }
  
  m_outgoingCells.clear();
  
  for (int i = m_a1; i < m_a2; ++i)
  {
    for (int j = m_b1; j < m_b2; ++j)
    {
      auto pd = m_provider->GetDescriptor(i, j);
      if (pd->m_cellDescriptor && pd->m_cellDescriptor->userData != nullptr)
      {
        auto context = static_cast<ObjectContext*>(pd->m_cellDescriptor->userData);
        assert(context->m_owner);
        if (context->m_owner == this)
        {
          Vec2 pos (pd->x, pd->y);
          LOG_W("%s, %s delete all items [%s]", __FUNCTION__, Description().c_str(), pos.Description().c_str());
          context->Destory(this);
          pd->m_cellDescriptor->userData = nullptr;
        }
      }
    }
  }
  
  m_cellMap->removeFromParentAndCleanup(true);
  m_background->removeFromParentAndCleanup(true);
  m_lightOverlay->removeFromParentAndCleanup(true);
  m_terrainSprite->removeFromParentAndCleanup(true);
  m_terrainBgSprite->removeFromParentAndCleanup(true);
}
  
bool PartialMap::Init(int a, int b, int width, int height,
                      IPixelWorld* provider,
                      cocos2d::Node* superView,
                      cocos2d::Node* lightNode,
                      const cocos2d::Vec2& offset)
{
  ChangeAABB(a, b, width, height);
  m_width = width;
  m_height = height;
  m_provider = provider;
  
  m_cellMap = std::make_shared<CellsLayer>();
  m_background = std::make_shared<DeadCellsLayer>(a, b, width, height);
  m_terrainSprite = TerrainSprite::create(a, b, width, height, "fg");
  m_terrainBgSprite = TerrainSprite::create(a, b, width, height, "bg");
  m_lightOverlay = StaticLightsLayer::create(a, b, width, height, "light_map");
  
  m_cellMap->init();
  m_background->init();
  
  m_cellMap->setPosition(offset);
  m_lightOverlay->setPosition(offset);
  m_background->setPosition(offset);
  m_terrainSprite->setPosition(offset);
  m_terrainBgSprite->setPosition(offset);
  
  superView->addChild(m_terrainBgSprite, -1);
  superView->addChild(m_background.get(), 0);
  superView->addChild(m_cellMap.get(), 1);
  superView->addChild(m_terrainSprite, 2);
  lightNode->addChild(m_lightOverlay, 3);
 
  m_cellMap->SetUpdateTime(0.2);
  
  for (int i = m_a1; i < m_a2; ++i)
  {
    for (int j = m_b1; j < m_b2; ++j)
    {
      auto pd = m_provider->GetDescriptor(i, j);
      InitPixel(pd);
    }
  }
  
  return true;
}
  
void PartialMap::AdoptIncomingItems()
{
  if (kRedrawEachUpdate) return;
  
  for (auto context : m_upcomingContexts)
  {
    context->BecomeOwner(this);
  }
  
  m_upcomingContexts.clear();
}
  
void PartialMap::DeleteOutgoingItems()
{
  LOG_W("%s, %s", __FUNCTION__, Description().c_str());
  if (kRedrawEachUpdate) return;
  
  for (auto cell : m_outgoingCells)
  {
    if (cell->userData == nullptr) {
      continue;
    }
    
    auto context = static_cast<ObjectContext*>(cell->userData);
    // Remove only context that hasn't been adopted by other pratial maps
    if (context->m_owner == this || context->m_owner == nullptr)
    {
      Vec2 pos (cell->parent->x, cell->parent->y);
      LOG_W("%s, %s delete m_outgoingCells [%s]", __FUNCTION__, Description().c_str(), pos.Description().c_str());
      cell->userData = nullptr;
      context->Destory(this);
    }
    
  }
  
  m_outgoingCells.clear();
}
  
void PartialMap::HandleItemsOnBounds(const WorldUpdateList& updateResult, float updateTime)
{
}

void PartialMap::Update(WorldUpdateList& updateResult, float updateTime)
{
  if (kRedrawEachUpdate)
  {
    Reset();
    
    for (int i = m_a1; i < m_a2; ++i)
    {
      for (int j = m_b1; j < m_b2; ++j)
      {
        auto pd = m_provider->GetDescriptor(i, j);
        InitPixel(pd);
      }
    }
    return;
  }
  
  m_background->Update(updateResult,updateTime);
  m_cellMap->SetUpdateTime(updateTime);
  
  for (auto& u : updateResult)
  {
    std::string operationType;
    
    Vec2 initialPos = Vec2(u.desc->x, u.desc->y);

    auto m = u.movement;
    auto a = u.action;
   
    Vec2 destinationPos(0,0);
    GreatPixel* destinationDesc = nullptr;
    ObjectContext* context = nullptr;
    if(u.addCreature == true)
    {
      destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
      destinationDesc = u.addCreature.value.destinationDesc;
    }
    else if(m == true)
    {
      context = static_cast<ObjectContext*>(u.userData);
      destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
      destinationDesc = m.value.destinationDesc;
    }
    else
    {
      context = static_cast<ObjectContext*>(u.userData);
    }
    
    if (u.morph == true && m == false && !IsInAABB(initialPos))
    {
      continue;
    }
    
    if (!IsInAABB(initialPos) && !IsInAABB(destinationPos))
    {
      continue;
    }
    
    if (u.deleteCreature == true)
    {
      if(!IsInAABB(initialPos))
      {
        continue;
      }
      Delete(context);
    }
    else if (u.changeRect == true)
    {
      if(!IsInAABB(initialPos))
      {
        continue;
      }
      
      static_cast<CellContext*>(context)->ChangeRect(u.desc->m_cellDescriptor, u.changeRect.value.rect, updateTime * 0.9);
      continue;
    }
    else if (u.morph == true && m == false)
    {
      auto aabb = u.desc->m_cellDescriptor->GetShape()->GetAABB();
      static_cast<AmorphCellContext*>(context)->MoveAmorphCells(initialPos, u.morph.value, aabb, updateTime * 0.9);
    }
    else if (m == true || u.addCreature == true)
    {
      if (IsInAABB(initialPos) && !IsInAABB(destinationPos)) // outgoing
      {
        LOG_W("%s [%s] outgoing: [%s]->[%s]", __FUNCTION__,
                     Description().c_str(),
                     initialPos.Description().c_str(),
                     destinationPos.Description().c_str());
        
        if (context && (context->m_owner == this || context->m_owner == nullptr))
        {
          m_outgoingCells.push_back(destinationDesc->m_cellDescriptor);
//          int duration = 0;
//          if (m == true)
//          {
//            duration = m.value.duration;
//            
//            if (duration > 5)
//            {
//              duration /= 2;
//            }
//          }
//          Move(initialPos, destinationPos, context, updateTime, duration, u.morph.value, destinationDesc->m_cellDescriptor);
          context->Free(this);
        }
      }
      else
      {
        if (context && !IsInAABB(initialPos) && IsInAABB(destinationPos)) // incoming
        {
          LOG_W("%s [%s] income: context:[%s][%s]->[%s]", __FUNCTION__,
                       Description().c_str(),
                       context->Description().c_str(),
                       initialPos.Description().c_str(),
                       destinationPos.Description().c_str());
          
          if (context->m_owner != this || context->m_owner == nullptr)
          {
            context->BecomeOwner(this);
          }
        }
        else
        {
          if (context == nullptr)
          {
            context = AddCreature(destinationPos, destinationDesc, u.morph.value, updateTime);
          }
        }
       
        int duration = 0;
        if (m == true)
        {
          duration = m.value.duration;
          
          if (duration > 5)
          {
            duration /= 2;
          }
        }
      
        Move(initialPos, destinationPos, context, updateTime, duration, u.morph.value, destinationDesc->m_cellDescriptor);
      }
    }
    else if (a == true)
    {
      if(!IsInAABB(initialPos))
      {
        continue;
      }
      assert(context);
      Attack(context, initialPos, a.value.delta, updateTime);
    }
    else
    {
      assert(0);
    }
  }
  

}
  
  void PartialMap::Reset()
  {
    m_cellMap->Reset();
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
    snprintf(buf, 1024 - 1, "%p x:[%d,%d]y:[%d,%d] %p", this, m_a1, m_a2, m_b1, m_b2, m_cellMap.get());
    return std::string(buf);
  }
  
  void PartialMap::InitPixel(GreatPixel* pd)
  {
    if (pd->m_type == GreatPixel::CreatureType)
    {
      if (pd->m_cellDescriptor->parent == pd)
      {
        // We are creating map current cell is incomming
        if (pd->m_cellDescriptor->userData != nullptr)
        {
          auto context = static_cast<ObjectContext*>(pd->m_cellDescriptor->userData);
          Vec2 initialPos = Vec2(pd->x, pd->y);
          LOG_W("%s [%s] adopting: context:[%s][%s]", __FUNCTION__,
                       Description().c_str(),
                       context->Description().c_str(),
                       initialPos.Description().c_str());
          if (context->m_owner == nullptr)
          {
            context->BecomeOwner(this);
          }
        }
        else // create new cell
        {
          CreateCell(pd);
        }
      }
      else if (kSimpleDraw && kRedrawEachUpdate)
      {
        CreateCell(pd);
      }
    }
  }
  
  ObjectContext* PartialMap::AddCreature(const Vec2& source, GreatPixel* dest, Morphing& morphing, float duration)
  {
    ObjectContext* context = nullptr;
    if (dest->m_cellDescriptor->GetShapeType() == eShapeTypeAmorph)
    {
      auto groupId = dest->m_cellDescriptor->m_genom.m_groupId;
      auto textureRect = SharedUIData::getInstance()->m_textureMap[groupId];
      auto c = new AmorphCellContext(this, textureRect);
      auto aabb = dest->m_cellDescriptor->GetShape()->GetAABB();
      
      dest->m_cellDescriptor->Shape([&](GreatPixel* pd, bool& stop)
                {
                  c->AddSprite(aabb, pd->GetPos());
                });
      
      c->MoveAmorphCells(source, morphing, aabb, duration * 0.9);
      c->EnableSmallAnimations(m_enableSmallAnimations);
      context = c;
    }
    else if (dest->m_cellDescriptor->GetShapeType() == eShapeTypeRect
             || dest->m_cellDescriptor->GetShapeType() == eShapeTypeSinglePixel
             || dest->m_cellDescriptor->GetShapeType() == eShapeTypeFixed)
    {
      auto groupId = dest->m_cellDescriptor->m_genom.m_groupId;
      auto textureRect = SharedUIData::getInstance()->m_textureMap[groupId];
      auto c = new CellContext(this,
                                               textureRect,
                                               dest->m_cellDescriptor->parent->GetPos(),
                                               dest->m_cellDescriptor->GetShape()->GetAABB());
      c->Move(source, dest->GetPos(), duration * 0.9);
      c->EnableSmallAnimations(m_enableSmallAnimations);
      context = c;
    }
    else
    {
      assert(0);
    }
    
    dest->m_cellDescriptor->userData = context;
    
    return context;
  }
  
  ObjectContext* PartialMap::CreateCell(GreatPixel* dest)
  {
    komorki::Morphing m;
    return AddCreature(Vec2(), dest, m, 0.f);
  }
  
  void PartialMap::Move(const komorki::Vec2 &source,
                        const komorki::Vec2 &dest,
                        ObjectContext *context,
                        float duration,
                        int steps,
                        komorki::Morphing &morphing,
                        komorki::CellDescriptor* cd)
  {
    if (context->GetType() == ContextType::ManyPixels)
    {
      auto aabb = cd->GetShape()->GetAABB();
      static_cast<AmorphCellContext*>(context)->MoveAmorphCells(source, morphing, aabb, duration*0.9*(steps + 1));
      return;
    }
    
    if (context->GetType() == ContextType::SinglePixel)
    {
      static_cast<CellContext*>(context)->Move(source, dest, duration*0.9*(steps + 1));
      return;
    }
    
    assert(0);
  }
  
  void PartialMap::Attack(ObjectContext* context, const Vec2& pos, const Vec2& offset, float animationDuration)
  {
    context->Attack(pos, offset, animationDuration);
  }
  
  void PartialMap::Delete(ObjectContext* context)
  {
    if (context == nullptr)
    {
      return;
    }
  
    assert(context->m_owner == this);
    if (context->m_owner == this)
    {
      LOG_W("%s, %s [%s]", __FUNCTION__, Description().c_str(), context->Description().c_str());
      context->Destory(this);
    }
  }
  

  inline bool PartialMap::IsInAABB(const Vec2& vec)
  {
    return IsInAABB(vec.x, vec.y);
  }
  
  inline bool PartialMap::IsInAABB(const int& x, const int& y)
  {
    if(x < m_a1 || x >= m_a2 ||
       y < m_b1 || y >= m_b2)
    {
      return false;
    }
    
    return true;
  }

  inline Vec2 PartialMap::LocalVector(const komorki::Vec2& input) const
  {
    return Vec2(input.x - m_a1, input.y - m_b1);
  }
}
}
