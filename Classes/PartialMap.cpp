//
//  PartialMap.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "PartialMap.h"
#include "PixelMapBackground.h"
#include "PixelDebugView.h"
#include "PixelMapLightOverlay.hpp"
#include "PixelMapPartial.h"
#include "GlowMapOverlay.h"
#include "PixelDescriptorProvider.h"
#include "Utilities.h"

namespace komorki
{
namespace ui
{
  
PartialMap::PartialMap()
{
}

PartialMap::~PartialMap()
{
  m_cellMap->removeFromParentAndCleanup(true);
  m_background->removeFromParentAndCleanup(true);
  m_lightOverlay->removeFromParentAndCleanup(true);
  m_debugView->removeFromParentAndCleanup(true);
  m_glow->removeFromParentAndCleanup(true);
  
  for (auto cell : m_outgoingCells)
  {
    if (cell->userData == nullptr) {
      continue;
    }
    
    assert(cell->userData != nullptr);
    auto context = static_cast<Context*>(cell->userData);
    if (context->sprite->getParent() == m_cellMap.get())
    {
      cell->userData = nullptr;
      m_cellMap->Delete(context);
      m_glow->Delete(context);
      delete context;
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
        delete static_cast<komorki::ui::PartialMap::Context*>(pd->m_cellDescriptor->userData);
        pd->m_cellDescriptor->userData = nullptr;
      }
    }
  }
}
  
bool PartialMap::Init(int a, int b, int width, int height,
                      PixelDescriptorProvider* provider, cocos2d::Node* superView, const cocos2d::Vec2& offset)
{
  m_a1 = a;
  m_a2 = a + width;
  m_b1 = b;
  m_b2 = b + height;
  m_width = width;
  m_height = height;
  m_provider = provider;
  
  m_cellMap = std::make_shared<PixelMapPartial>();
  m_debugView = std::make_shared<PixelDebugView>(a, b, width, height, provider);
  m_lightOverlay = std::make_shared<PixelMapLightOverlay>(a, b, width, height, provider);
  m_background = std::make_shared<PixelMapBackground>(a, b, width, height);
  m_glow = std::make_shared<GlowMapOverlay>();
  
  m_cellMap->init();
  m_debugView->init();
  m_lightOverlay->init();
  m_background->init();
  m_glow->init();
  
  m_cellMap->setPosition(offset);
  m_debugView->setPosition(offset);
  m_lightOverlay->setPosition(offset);
  m_background->setPosition(offset);
  m_glow->setPosition(offset);
  
  superView->addChild(m_background.get(), 0);
  superView->addChild(m_cellMap.get(), 1);
  superView->addChild(m_lightOverlay.get(), 2);
  superView->addChild(m_glow.get(), 3);
  superView->addChild(m_debugView.get(), 4);
 
  m_cellMap->SetUpdateTime(0.2);
  m_glow->SetUpdateTime(0.2);
  
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
  
void PartialMap::PreUpdate1(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
{
  for (auto context : m_upcomingContexts)
  {
    m_cellMap->AdoptSprite(context);
    m_glow->AdoptSprite(context);
  }
  
  m_upcomingContexts.clear();
}
  
void PartialMap::PreUpdate2(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
{
  for (auto cell : m_outgoingCells)
  {
    assert(cell->userData != nullptr);
    auto context = static_cast<Context*>(cell->userData);
    if (context->sprite->getParent() == m_cellMap.get())
    {
      cell->userData = nullptr;
      m_cellMap->Delete(context);
      m_glow->Delete(context);
      delete context;
    }
  }
  
  m_outgoingCells.clear();
}
  
void PartialMap::PostUpdate(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
{
  for (auto u : updateResult)
  {
    Vec2 destinationPos(0,0);
    Vec2 initialPos = Vec2(u.desc->x, u.desc->y);
    
    auto m = u.movement;
    auto a = u.action;
    
    if (u.deleteCreature == true || a == true)
      continue;
    
    if(u.addCreature == true)
    {
      destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
    }
    
    if (m == true) {
      destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
    }
    
    komorki::PixelDescriptor* newDescriptor = nullptr;
    
    if (m == true)
    {
      newDescriptor = m.value.destinationDesc;
    }
    else if (u.addCreature == true)
    {
      newDescriptor = u.addCreature.value.destinationDesc;
    }
    
    auto context = static_cast<Context*>(newDescriptor->m_cellDescriptor->userData);
    
    if(!IsInAABB(initialPos))
    {
      if(IsInAABB(destinationPos))
      {
        if (context == nullptr)
        {
          context = CreateCell(newDescriptor);
          Move(initialPos, destinationPos, context);
          continue;
        }
        
        context->pos = LocalVector(destinationPos);
        
        m_upcomingContexts.push_back(context);
      }
    }
    
    if (IsInAABB(initialPos) && !IsInAABB(destinationPos))
    {
      assert(context);
      assert(newDescriptor->m_cellDescriptor);
      m_outgoingCells.push_back(newDescriptor->m_cellDescriptor);
    }
  }
  
  m_cellMap->PostUpdate(updateResult, updateTime);
}

void PartialMap::Update(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
{
  m_cellMap->SetUpdateTime(updateTime);
  m_glow->SetUpdateTime(updateTime);
  
  for (auto u : updateResult)
  {
    std::string operationType;

    Vec2 destinationPos(0,0);
    Vec2 initialPos = Vec2(u.desc->x, u.desc->y);
    komorki::Vec2 pos(u.desc->x, u.desc->y);
    
    auto m = u.movement;
    auto a = u.action;
    
    if(u.movement == true)
      operationType = "movement";
    else if(u.action == true)
      operationType = "action";
    else if (u.addCreature == true)
      operationType = "addCreature";
    else if (u.deleteCreature == true)
      operationType = "delete";
    
    if(u.addCreature == true)
    {
      destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
    }
    
    if(m == true)
    {
      destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
    }
    
    if(!IsInAABB(initialPos))
    {
      continue;
    }
    
    Context* context = static_cast<Context*>(u.userData);
    if (u.deleteCreature == true)
    {
      Delete(context);
    }
    else if (u.addCreature == true)
    {
      auto newDescriptor = u.addCreature.value.destinationDesc;
      AddCreature(pos, newDescriptor);
      continue;
    }
    else if (m == true)
    {
      if (context == nullptr)
      {
        context = CreateCell(m.value.destinationDesc);
      }
      
      komorki::Vec2 dest(m.value.destinationDesc->x, m.value.destinationDesc->y);
      Move(pos, dest, context);
    }
    else if (a == true)
    {
      assert(context);
      Attack(context, pos, a.value.delta);
    }
    else
    {
      assert(0);
    }
  }
  
  m_background->Update(updateResult,updateTime);
  m_debugView->Update(updateResult,updateTime);
}
  
  void PartialMap::HightlightCellOnPos(int x, int y, komorki::CellType type)
  {
    m_cellMap->HightlightCellOnPos(x, y, type);
  }
  void PartialMap::StopHightlighting()
  {
    m_cellMap->StopHightlighting();
  }
  void PartialMap::Reset()
  {
    m_cellMap->Reset();
  }
 
  void PartialMap::Transfrorm(const cocos2d::Vec2& pos, float scale)
  {
    m_cellMap->setPosition(pos);
    m_background->setPosition(pos);
    m_debugView->setPosition(pos);
    m_lightOverlay->setPosition(pos);
    m_glow->setPosition(pos);
    
    m_cellMap->setScale(scale);
    m_background->setScale(scale);
    m_debugView->setScale(scale);
    m_lightOverlay->setScale(scale);
    m_glow->setScale(scale);
  }
  
  void PartialMap::InitPixel(PixelDescriptor* pd)
  {
    if (pd->m_type == PixelDescriptor::CreatureType)
    {
      if (pd->m_cellDescriptor->parent == pd)
      {
        CreateCell(pd);
      }
    }
    else
    {
      m_cellMap->AddSprite(pd, LocalVector(Vec2(pd->x, pd->y)));
    }
  }
  
  void PartialMap::AddCreature(const Vec2& source, PixelDescriptor* dest)
  {
    Context* context = new Context();
    context->pos = LocalVector(source);
    
    dest->m_cellDescriptor->userData = context;
    
    m_cellMap->AddCreature(dest->m_cellDescriptor, context);
    m_cellMap->MoveCreature(context, LocalVector(source), LocalVector(Vec2(dest->x, dest->y)));
    m_glow->AddCreature(dest->m_cellDescriptor, context);
    m_glow->MoveCreature(context, LocalVector(source), LocalVector(Vec2(dest->x, dest->y)));
  }
  
  PartialMap::Context* PartialMap::CreateCell(PixelDescriptor* dest)
  {
    Context* context = new Context();
    context->pos = LocalVector(Vec2(dest->x, dest->y));
    assert(dest->m_cellDescriptor->userData == nullptr);
    dest->m_cellDescriptor->userData = context;
    m_cellMap->AddCreature(dest->m_cellDescriptor, context);
    m_glow->AddCreature(dest->m_cellDescriptor, context);
    
    return context;
  }
  
  void PartialMap::Delete(Context* context)
  {
    if (context == nullptr)
    {
      return;
    }
    
    m_cellMap->Delete(context);
    m_glow->Delete(context);
    
    delete context;
  }
  
  void PartialMap::Move(const Vec2& source, const Vec2& dest, Context* context)
  {
    m_cellMap->MoveCreature(context, LocalVector(source), LocalVector(dest));
    m_glow->MoveCreature(context, LocalVector(source), LocalVector(dest));
  }
 
  void PartialMap::Attack(Context* context, const Vec2& pos, const Vec2& offset)
  {
    m_cellMap->Attack(context, LocalVector(pos), offset);
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