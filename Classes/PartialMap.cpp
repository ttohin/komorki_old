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
#include "UIConfig.h"
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
  , type(eContextTypeDefault)
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
  
  void PartialMap::Context::ForceDestory(PartialMap* _owner)
  {
    LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner);
    delete this;
  }
  
  PartialMap::PolymorphShapeContext PartialMap::Context::GetSprite(int x, int y)
  {
    std::string key = GetKey(x, y);
    auto s = spriteMap[key];
    return s;
  }
  
  PartialMap::PolymorphShapeContext PartialMap::Context::PopSprite(int x, int y)
  {
    std::string key = GetKey(x, y);
    auto s = spriteMap[key];
    spriteMap.erase(key);
    return s;
  }
  
  void PartialMap::Context::SetSprite(cocos2d::Sprite* s, int x, int y)
  {
    PartialMap::PolymorphShapeContext c;
    c.sprite = s;
    c.pos = Vec2(x, y);
    spriteMap[GetKey(x, y)] = c;
  }
  
 std::string PartialMap::Context::GetKey(int x, int y) const
  {
//    const unsigned kMaxPolymorphSize = 256;
//    unsigned long long key = (x + kMaxPolymorphSize) + (y + kMaxPolymorphSize) * kMaxPolymorphSize;
//    return key;
    return std::to_string(x) + "_" + std::to_string(y);
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
//  m_terrain->removeFromParentAndCleanup(true);
  m_terrainSprite->removeFromParentAndCleanup(true);
  
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
                      PixelDescriptorProvider* provider, cocos2d::Node* superView, cocos2d::Node* lightNode, const cocos2d::Vec2& offset)
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
//  m_terrain = std::make_shared<TerrainBatchSprite>();
  m_terrainSprite = TerrainSprite::create(a, b, width, height);
  
  m_cellMap->init();
  m_debugView->init();
  m_lightOverlay->init();
  m_background->init();
  m_glow->init();
  
//  auto terrain = provider->GetTerrain();
//  TerrainAnalizer::Result result;
//  
//  result.background = std::make_shared<Buffer2D<TerrainInfo>>(width * 2, height * 2);
//  result.ground = std::make_shared<Buffer2D<TerrainInfo>>(width * 2, height * 2);
//  result.foreground = std::make_shared<Buffer2D<TerrainInfo>>(width * 2, height * 2);
//  
//  terrain.background->SubSet(a * 2, b * 2, *result.background.get());
//  terrain.ground->SubSet(a * 2, b * 2, *result.ground.get());
//  terrain.foreground->SubSet(a * 2, b * 2, *result.foreground.get());
//  
//  m_terrain->init(result);
  
  m_cellMap->setPosition(offset);
  m_debugView->setPosition(offset);
  m_lightOverlay->setPosition(offset);
  m_background->setPosition(offset);
  m_glow->setPosition(offset);
//  m_terrain->setPosition(offset);
  m_terrainSprite->setPosition(offset);
  
  superView->addChild(m_background.get(), -1);
//  superView->addChild(m_terrain.get(), 0);
  superView->addChild(m_terrainSprite, 0);
  superView->addChild(m_cellMap.get(), 1);
  lightNode->addChild(m_lightOverlay.get(), 2);
  lightNode->addChild(m_glow.get(), 3);
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
  if (kRedrawEachUpdate) return;
  
  for (auto context : m_upcomingContexts)
  {
    context->BecomeOwner(this);
    
    m_cellMap->AdoptSprite(context, Vec2(m_a1, m_b1));
    m_glow->AdoptSprite(context);
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

void PartialMap::Update(std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
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

    m_debugView->Update(updateResult,updateTime);
    return;
  }
  
  m_cellMap->SetUpdateTime(updateTime);
  m_glow->SetUpdateTime(updateTime);
  
  for (auto& u : updateResult)
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
    else if (u.morph == true && m == false)
    {
      if (context->type == Context::eContextTypePolymorph)
      {
        m_cellMap->MovePolymorphCells(context, initialPos, u.morph.value, Vec2(m_a1, m_b1), u.desc->m_cellDescriptor);
      }
      else
      {
        m_cellMap->MoveAmorphCells(context, initialPos, u.morph.value, Vec2(m_a1, m_b1), u.desc->m_cellDescriptor);
      }
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
          int duration = 0;
          if (m == true)
          {
            duration = m.value.duration;
            
            if (duration > 5)
            {
              duration /= 2;
            }
          }
          Move(initialPos, destinationPos, context, duration, u.morph.value, destinationDesc->m_cellDescriptor);
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
            m_cellMap->AdoptSprite(context, Vec2(m_a1, m_b1));
            m_glow->AdoptSprite(context);
            context->BecomeOwner(this);
          }
        }
        else
        {
          if (context == nullptr)
          {
            context = AddCreature(destinationPos, destinationDesc, u.morph.value);
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
      
        Move(initialPos, destinationPos, context, duration, u.morph.value, destinationDesc->m_cellDescriptor);
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
    m_glow->Reset();
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
//    m_terrain->setPosition(pos);
    m_terrainSprite->setPosition(pos);
    
    m_cellMap->setScale(scale);
    m_background->setScale(scale);
    m_debugView->setScale(scale);
    m_lightOverlay->setScale(scale);
    m_glow->setScale(scale);
//    m_terrain->setScale(scale);
    m_terrainSprite->setScale(scale * 4.f);
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
            
            m_cellMap->AdoptSprite(context, Vec2(m_a1, m_b1));
            m_glow->AdoptSprite(context);
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
    else
    {
      m_cellMap->AddSprite(pd, LocalVector(Vec2(pd->x, pd->y)));
    }
  }
  
  PartialMap::Context::ContextType GetContextType(CellDescriptor* cd)
  {
    if (cd->GetShapeType() == CellDescriptor::eShapeTypeAmorph) {
      return PartialMap::Context::eContextTypeAmorph;
    }
    if (cd->GetShapeType() == CellDescriptor::eShapeTypePolymorph) {
      return PartialMap::Context::eContextTypePolymorph;
    }
    if (cd->GetShapeType() == CellDescriptor::eShapeTypeRect) {
      return PartialMap::Context::eContextTypeRect;
    }
    return PartialMap::Context::eContextTypeDefault;
  }
  
  PartialMap::Context* PartialMap::AddCreature(const Vec2& source, PixelDescriptor* dest, Morphing& morphing)
  {
    Context* context = new Context(this);
    context->type = GetContextType(dest->m_cellDescriptor);
    
    morphing.proccessed = true;
    
    context->pos = LocalVector(source);
    
    dest->m_cellDescriptor->userData = context;
    
    m_cellMap->AddCreature(dest->m_cellDescriptor, context, Vec2(m_a1, m_b1));
    m_glow->AddCreature(dest->m_cellDescriptor, context);
    if (source != Vec2()) {
      m_cellMap->MoveCreature(context, LocalVector(source), LocalVector(Vec2(dest->x, dest->y)), 0, morphing, Vec2(m_a1, m_b1), dest->m_cellDescriptor);
      m_glow->MoveCreature(context, LocalVector(source), LocalVector(Vec2(dest->x, dest->y)));
    }
    
    return context;
  }
  
  PartialMap::Context* PartialMap::CreateCell(PixelDescriptor* dest)
  {
    Context* context = new Context(this);
    context->type = GetContextType(dest->m_cellDescriptor);
    context->pos = LocalVector(Vec2(dest->x, dest->y));

    assert(dest->m_cellDescriptor->userData == nullptr);
    m_cellMap->AddCreature(dest->m_cellDescriptor, context, Vec2(m_a1, m_b1));
    m_glow->AddCreature(dest->m_cellDescriptor, context);
    
    if (!kRedrawEachUpdate)
    {
      dest->m_cellDescriptor->userData = context;
    }
    else
    {
      context->Destory(this);
      return nullptr;
    }
    
    return context;
  }
  
  PartialMap::Context* PartialMap::CreatePolymorphCell(PixelDescriptor* dest)
  {
    return nullptr;
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
  
  void PartialMap::Move(const Vec2& source, const Vec2& dest, Context* context, int duration, Morphing& morphing, CellDescriptor* cd)
  {
    m_cellMap->MoveCreature(context, LocalVector(source), LocalVector(dest), duration, morphing, Vec2(m_a1, m_b1), cd);
    m_glow->MoveCreature(context, LocalVector(source), LocalVector(dest), duration);
  }
 
  void PartialMap::Attack(Context* context, const Vec2& pos, const Vec2& offset)
  {
    m_cellMap->Attack(context, LocalVector(pos), offset, Vec2(m_a1, m_b1));
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