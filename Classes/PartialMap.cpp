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
#include <stdio.h>
#include <sstream> //for std::stringstream
#include <string>  //for std::string


#ifdef DEBUG_PARTIAL_MAP
#define LOG_W(...) KOMORKI_LOG(__VA_ARGS__)
#else
#define LOG_W(...) {}
#endif

namespace komorki
{
namespace ui
{
  void PartialMap::Context::BecomeOwner(PartialMap* _owner)
  {
    LOG_W("%s, %s. New owner: %p", __FUNCTION__, Description().c_str(), _owner);
    owner = _owner;
  }
  
  void PartialMap::Context::Free(PartialMap* _owner)
  {
    LOG_W("%s, %s: argument: %p", __FUNCTION__, Description().c_str(), _owner);
    if (owner == _owner)
    {
      owner = nullptr;
    }
  }
  
  PartialMap::Context::Context(PartialMap *_owner)
  : owner(_owner)
  , sprite(nullptr)
  , glow(nullptr)
  {
    LOG_W("%s, %s", __FUNCTION__, Description().c_str());
  }
  
  std::string PartialMap::Context::Description() const
  {
    std::stringstream ss;
    ss <<
    static_cast<const void*>(this) <<
    " ownwer " <<
    static_cast<const void*>(owner) <<
    " sprite: " <<
    static_cast<const void*>(sprite);
    return ss.str();
  }
  
  void PartialMap::Context::Destory(PartialMap* _owner)
  {
    LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner);
    assert(owner == _owner || owner == nullptr);
    delete this;
  }
  
PartialMap::Context::~Context()
{
  LOG_W("%s, %s", __FUNCTION__, Description().c_str());
}
  
PartialMap::PartialMap()
{
}

PartialMap::~PartialMap()
{
  LOG_W("%s, %s", __FUNCTION__, Description().c_str());
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
    
    auto context = static_cast<Context*>(cell->userData);
    // Remove only context that hasn't been adopted by other pratial maps
    if (context->owner == this || context->owner == nullptr)
    {
      Vec2 pos (cell->parent->x, cell->parent->y);
      LOG_W("%s, %s delete m_outgoingCells [%s]", __FUNCTION__, Description().c_str(), pos.Description().c_str());
      cell->userData = nullptr;
      m_cellMap->Delete(context);
      m_glow->Delete(context);
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
        auto context = static_cast<Context*>(pd->m_cellDescriptor->userData);
        assert(context->owner);
        if (context->owner == this)
        {
          Vec2 pos (pd->x, pd->y);
          LOG_W("%s, %s delete all items [%s]", __FUNCTION__, Description().c_str(), pos.Description().c_str());
          context->Destory(this);
          pd->m_cellDescriptor->userData = nullptr;
        }
      }
    }
  }
}
  
bool PartialMap::Init(int a, int b, int width, int height,
                      PixelDescriptorProvider* provider, cocos2d::Node* superView, const cocos2d::Vec2& offset)
{
  ChangeAABB(a, b, width, height);
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
  
void PartialMap::AdoptIncomingItems()
{
  for (auto context : m_upcomingContexts)
  {
    context->BecomeOwner(this);
    
    m_cellMap->AdoptSprite(context);
    m_glow->AdoptSprite(context);
  }
  
  m_upcomingContexts.clear();
}
  
void PartialMap::DeleteOutgoingItems()
{
  LOG_W("%s, %s", __FUNCTION__, Description().c_str());
  for (auto cell : m_outgoingCells)
  {
    if (cell->userData == nullptr) {
      continue;
    }
    
    auto context = static_cast<Context*>(cell->userData);
    // Remove only context that hasn't been adopted by other pratial maps
    if (context->owner == this || context->owner == nullptr)
    {
      Vec2 pos (cell->parent->x, cell->parent->y);
      LOG_W("%s, %s delete m_outgoingCells [%s]", __FUNCTION__, Description().c_str(), pos.Description().c_str());
      cell->userData = nullptr;
      m_cellMap->Delete(context);
      m_glow->Delete(context);
      context->Destory(this);
    }
  }
  
  m_outgoingCells.clear();
}
  
void PartialMap::HandleItemsOnBounds(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
{
}

void PartialMap::Update(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
{
  m_cellMap->SetUpdateTime(updateTime);
  m_glow->SetUpdateTime(updateTime);
  
  for (auto u : updateResult)
  {
    std::string operationType;
    
    Vec2 initialPos = Vec2(u.desc->x, u.desc->y);

    auto m = u.movement;
    auto a = u.action;
   
    Vec2 destinationPos(0,0);
    PixelDescriptor* destinationDesc = nullptr;
    Context* context = nullptr;
    if(u.addCreature == true)
    {
      destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
      destinationDesc = u.addCreature.value.destinationDesc;
    }
    else if(m == true)
    {
      context = static_cast<Context*>(u.userData);
      destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
      destinationDesc = m.value.destinationDesc;
    }
    else
    {
      context = static_cast<Context*>(u.userData);
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
    else if (m == true || u.addCreature == true)
    {
      if (IsInAABB(initialPos) && !IsInAABB(destinationPos)) // outgoing
      {
        LOG_W("%s [%s] outgoing: [%s]->[%s]", __FUNCTION__,
                     Description().c_str(),
                     initialPos.Description().c_str(),
                     destinationPos.Description().c_str());
        
        if (context && (context->owner == this || context->owner == nullptr))
        {
          context->Free(this);
          m_outgoingCells.push_back(destinationDesc->m_cellDescriptor);
          Move(initialPos, destinationPos, context);
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
          
          if (context->owner != this || context->owner == nullptr)
          {
            m_cellMap->AdoptSprite(context);
            m_glow->AdoptSprite(context);
            context->BecomeOwner(this);
          }
        }
        else
        {
          if (context == nullptr)
          {
            context = AddCreature(destinationPos, destinationDesc);
          }
        }
        
        Move(initialPos, destinationPos, context);
      }
    }
    else if (a == true)
    {
      if(!IsInAABB(initialPos))
      {
        continue;
      }
      assert(context);
      Attack(context, initialPos, a.value.delta);
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

  
  void PartialMap::StopSmallAnimations()
  {
    m_cellMap->StopSmallAnimations();
  }
  
  void PartialMap::StartSmallAnimations()
  {
    m_cellMap->StartSmallAnimations();
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
    snprintf(buf, 1024 - 1, "%p x:[%d,%d]y:[%d,%d]", this, m_a1, m_a2, m_b1, m_b2);
    return std::string(buf);
  }
  
  void PartialMap::InitPixel(PixelDescriptor* pd)
  {
    if (pd->m_type == PixelDescriptor::CreatureType)
    {
      if (pd->m_cellDescriptor->parent == pd)
      {
        // We are creating map current cell is incomming
        if (pd->m_cellDescriptor->userData != nullptr)
        {
          auto context = static_cast<Context*>(pd->m_cellDescriptor->userData);
          Vec2 initialPos = Vec2(pd->x, pd->y);
          LOG_W("%s [%s] adopting: context:[%s][%s]", __FUNCTION__,
                       Description().c_str(),
                       context->Description().c_str(),
                       initialPos.Description().c_str());
          if (context->owner == nullptr)
          {
            context->BecomeOwner(this);
            context->pos = initialPos;
            
            m_cellMap->AdoptSprite(context);
            m_glow->AdoptSprite(context);
          }
        }
        else
        {
          CreateCell(pd);
        }
      }
    }
    else
    {
      m_cellMap->AddSprite(pd, LocalVector(Vec2(pd->x, pd->y)));
    }
  }
  
  PartialMap::Context* PartialMap::AddCreature(const Vec2& source, PixelDescriptor* dest)
  {
    Context* context = new Context(this);
    context->pos = LocalVector(source);
    
    dest->m_cellDescriptor->userData = context;
    
    m_cellMap->AddCreature(dest->m_cellDescriptor, context);
    m_glow->AddCreature(dest->m_cellDescriptor, context);
    if (source != Vec2()) {
      m_cellMap->MoveCreature(context, LocalVector(source), LocalVector(Vec2(dest->x, dest->y)));
      m_glow->MoveCreature(context, LocalVector(source), LocalVector(Vec2(dest->x, dest->y)));
    }
    
    return context;
  }
  
  PartialMap::Context* PartialMap::CreateCell(PixelDescriptor* dest)
  {
    Context* context = new Context(this);
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
  
    assert(context->owner == this);
    if (context->owner == this)
    {
      LOG_W("%s, %s [%s]", __FUNCTION__, Description().c_str(), context->Description().c_str());
      m_cellMap->Delete(context);
      m_glow->Delete(context);
      context->Destory(this);
    }
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