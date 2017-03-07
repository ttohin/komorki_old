//
//  PartialMapsManager.cpp
//  Komorki
//
//  Created on 04.03.17.
//

#include "PartialMapsManager.h"
#include "ObjectContext.h"
#include "PartialMap.h"
#include "CellDescriptor.h"
#include "CellContext.h"
#include "AmorphCellContext.h"
#include "SharedUIData.h"
#include "UIConfig.h"
#include "Logging.h"

namespace komorki
{
  namespace graphic
  {
    Vec2 GetMapOriginFromPos(const Vec2& pos)
    {
      return Vec2((pos.x/kSegmentSize) * kSegmentSize,
                  (pos.y/kSegmentSize) * kSegmentSize);
    }
    
    //********************************************************************************************
    void PartialMapsManager::Update(const CreateMapArgs &createMapArgs,
                                    const RemoveMapArgs& mapsToRemove,
                                    const WorldUpdateResult &worldUpdate,
                                    float animationDuration)
    {
      uint stepId = worldUpdate.updateId;
      
      LOG_W("PartialMapsManager::Update. step %d", stepId);
      
      for (const auto& createMapArg : createMapArgs)
      {
        auto map = CreateMap(createMapArg);
      }
      
      for (auto& m : mapsToRemove)
      {
        
        auto it = m_map.find(GetMapOriginFromPos(m));
        assert(it != m_map.end());
        LOG_W("PartialMapsManager::Update. Delete maps. step %d. Map: %s", stepId, it->second->Description().c_str());
        RemoveMap(it->second);
        m_map.erase(it);
      }
      
      for (const auto& u : worldUpdate.list)
      {
        Vec2 initialPos = Vec2(u.desc->x, u.desc->y);
        
        auto m = u.movement;
        
        Vec2 destinationPos = initialPos;
        CellDescriptor* cell = u.cell;
        
        // PEDANTIC
        assert(cell);
        
        ObjectContext* context = static_cast<ObjectContext*>(cell->userData);
        
        LOG_W("Upate cell %p %s step %d cell %p", cell, context ? context->Description().c_str() : "null", stepId, cell);
        
        if(u.addCreature == true)
        {
          destinationPos = initialPos;
          initialPos = Vec2(u.addCreature.value.sourcePixel->x, u.addCreature.value.sourcePixel->y);
          assert(destinationPos != initialPos);
        }
        else if(m == true)
        {
          destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
        }
        
        PartialMapPtr initialMap = GetMap(GetMapOriginFromPos(initialPos));
        PartialMapPtr destinationMap = GetMap(GetMapOriginFromPos(destinationPos));
        
        if (!initialMap && !destinationMap)
        {
          LOG_W("DeleteFromMap outside! %p %s step %d cell %p", cell, context ? context->Description().c_str() : "null", stepId, cell);
          DeleteFromMap(cell, context, initialMap);
          continue;
        }
        
        // if cell is going outside viewport - just remove it
        if (destinationPos != initialPos && destinationMap == nullptr)
        {
          LOG_W("DeleteFromMap %p %s step %d %p", cell, context ? context->Description().c_str() : "null", stepId, cell);
          DeleteFromMap(cell, context, initialMap);
          continue;
        }
      
        
        // Add creature and move it
        if (u.addCreature == true)
        {
          // create context if it's needed
          if (context == nullptr)
          {
//            if (cell->GetShapeType() == eShapeTypeAmorph)
//            {
//              context = BornNewAmorphCell(cell, initialPos, destinationMap, animationDuration);
//            }
//            else
//            {
              context = CreateObjectContext(cell,
                                            destinationMap);
              Move(cell,
                   initialPos,
                   destinationPos,
                   u.morph.value,
                   destinationMap,
                   0,
                   animationDuration);
//            }
          }
          else
          {
            Move(cell,
                 initialPos,
                 destinationPos,
                 u.morph.value,
                 destinationMap,
                 0,
                 animationDuration);
          }

          continue;
        }
        
        // refactor this part in PixelWorld:
        // we should have only one option for movement
        if (u.morph == true || m == true)
        {
          PartialMapPtr mapForAction = destinationMap ? destinationMap : initialMap;
          
          // create context if it's needed
          if (context == nullptr)
          {
            context = CreateObjectContext(cell,
                                          mapForAction);
          }
          
          LOG_W("Upate cell move %p %s step %d cell %p", cell, context ? context->Description().c_str() : "null", stepId, cell);
          
          int steps = 0;
          if (m == true)
          {
            steps = m.value.duration;
            
            if (steps > 5)
            {
              steps /= 2;
            }
          }
          
          Move(cell,
               initialPos,
               destinationPos,
               u.morph.value,
               mapForAction,
               steps,
               animationDuration);
          
          continue;
        }
        
        if (u.changeRect == true)
        {
          static_cast<CellContext*>(context)->ChangeRect(cell, u.changeRect.value.rect, animationDuration * 0.9);
          continue;
        }
        
        if (u.action == true)
        {
          if (!context)
          {
            continue;
          }
          
          assert(context);
          Attack(context, initialPos, u.action.value.delta, animationDuration);
          continue;
        }
        
        if (u.deleteCreature == true)
        {
          LOG_W("Upate cell delete %p %s step %d cell %p", cell, context ? context->Description().c_str() : "null", stepId, cell);
          
          if (!context)
          {
            continue;
          }
          
          assert(context);
          context->CellDead();
          DeleteFromMap(cell, context, initialMap);
        }
        
      }
      
      auto instanceCounter = PartialMap::instanceCounter;
      assert(instanceCounter == m_map.size());
    }
    
    //********************************************************************************************
    PartialMapPtr PartialMapsManager::CreateMap(const CreateMapArg& args)
    {
      auto map = std::make_shared<graphic::PartialMap>();
      map->Init(args.rect.origin.x,
                args.rect.origin.y,
                args.rect.size.x,
                args.rect.size.y,
                m_provider.get(),
                m_mainNode,
                m_lightNode,
                cocos2d::Vec2::ZERO);

      map->Transfrorm(args.graphicPos,
                      1.0);
      map->EnableAnimations(m_enableAnimations);
      map->EnableFancyAnimations(m_enableFancyAnimaitons);
      
      auto it = m_map.insert(std::make_pair(GetMapOriginFromPos(args.rect.origin), map));
      assert(it.second);
      
      for (int i = map->m_a1; i < map->m_a2; ++i)
      {
        for (int j = map->m_b1; j < map->m_b2; ++j)
        {
          auto pd = m_provider->GetDescriptor(i, j);
          assert(pd);
          if (pd->m_cellDescriptor && pd->m_cellDescriptor->parent == pd)
          {
            CreateObjectContext(pd->m_cellDescriptor, map);
          }
        }
      }
      
      return map;
    }
    
    //********************************************************************************************
    const Maps& PartialMapsManager::GetMaps() const
    {
      return m_map;
    }
    
    //********************************************************************************************
    PartialMapPtr PartialMapsManager::GetMap(Vec2ConstRef pos)
    {
      auto it = m_map.find(pos);
      if (m_map.end() == it)
        return nullptr;
      else
        return it->second;
    }
    
    //********************************************************************************************
    void PartialMapsManager::EnableAnimation(bool enableAnimations, bool enableFancyAnimations)
    {
      if (m_enableAnimations == enableAnimations &&
          m_enableFancyAnimaitons == enableFancyAnimations)
        return;
      
      m_enableAnimations = enableAnimations;
      m_enableFancyAnimaitons = enableFancyAnimations;
      
      for (auto& m : m_map)
      {
        m.second->EnableAnimations(m_enableAnimations);
        m.second->EnableFancyAnimations(m_enableFancyAnimaitons);
      }
      
      for (int i = m_visibleArea.origin.x; i < m_visibleArea.origin.x + m_visibleArea.size.x; ++i)
      {
        for (int j = m_visibleArea.origin.y; j < m_visibleArea.origin.y + m_visibleArea.size.y; ++j)
        {
          auto pd = m_provider->GetDescriptor(i, j);
          if (pd->m_cellDescriptor && pd->m_cellDescriptor->parent == pd && pd->m_cellDescriptor->userData != nullptr)
          {
            ObjectContext* context = static_cast<ObjectContext*>(pd->m_cellDescriptor->userData);
            context->ToggleAnimation();
          }
        }
      }
    }
    
    //********************************************************************************************
    ObjectContext* PartialMapsManager::BornNewAmorphCell(CellDescriptor* cell,
                                                         Vec2ConstRef source,
                                                          const PartialMapPtr& map,
                                                         float animationDuration)
    {
      if (cell->userData != nullptr)
      {
        ObjectContext* context = static_cast<ObjectContext*>(cell->userData);
        context->BecomeOwner(map);
        return context;
      }
      
      auto groupId = cell->m_genom.m_groupId;
      auto textureRect = SharedUIData::getInstance()->m_textureMap[groupId];
      
      assert (cell->GetShapeType() == eShapeTypeAmorph);
      
      auto c = new AmorphCellContext(map, textureRect, cell);
      auto aabb = cell->GetShape()->GetAABB();
      assert(aabb.size.x == 1);
      assert(aabb.size.y == 1);
      assert(aabb.origin != source);
      aabb.origin = source; // modify no play animation with source as aabb.origin
      
      int numberOfPixels = 0;
      cell->Shape([&](GreatPixel* pd, bool& stop)
                  {
                    numberOfPixels += 1;
                    assert(numberOfPixels == 1);
                    c->AddSprite(aabb, source);
                  });
      assert(cell->userData == nullptr);
      cell->userData = c;
      c->ToggleAnimation();
      
      aabb = cell->GetShape()->GetAABB(); // get original aabb.
      
      komorki::Morphing morph;
      Morph m;
      m.dir = Morph::Outside;
      m.pos = cell->parent->GetPos();
      m.delta = source - m.pos;
      morph.vec.push_back(m);
      
      static_cast<AmorphCellContext*>(c)->MoveAmorphCells(source,morph, aabb, animationDuration*0.9);
      
      return c;

    }
    
    //********************************************************************************************
    ObjectContext* PartialMapsManager::CreateObjectContext(CellDescriptor* cell,
                                                           const PartialMapPtr& map)
    {
      if (cell->userData != nullptr)
      {
        ObjectContext* context = static_cast<ObjectContext*>(cell->userData);
        context->BecomeOwner(map);
        return context;
      }
      
      auto groupId = cell->m_genom.m_groupId;
      auto textureRect = SharedUIData::getInstance()->m_textureMap[groupId];
      
      if (cell->GetShapeType() == eShapeTypeAmorph)
      {
        auto c = new AmorphCellContext(map, textureRect, cell);
        auto aabb = cell->GetShape()->GetAABB();
        
        cell->Shape([&](GreatPixel* pd, bool& stop)
                    {
                      c->AddSprite(aabb, pd->GetPos());
                    });
        assert(cell->userData == nullptr);
        cell->userData = c;
        c->ToggleAnimation();
        return c;
      }
      else if (cell->GetShapeType() == eShapeTypeRect
               || cell->GetShapeType() == eShapeTypeSinglePixel
               || cell->GetShapeType() == eShapeTypeFixed)
      {
        auto c = new CellContext(map,
                                 textureRect,
                                 cell->parent->GetPos(),
                                 cell->GetShape()->GetAABB(),
                                 cell);
        assert(cell->userData == nullptr);
        cell->userData = c;
        c->ToggleAnimation();
        return c;
      }
      else
      {
        assert(0);
      }
      
      return nullptr;
    }
    
    void PartialMapsManager::RemoveMap(const PartialMapPtr& map)
    {
      for (int i = map->m_a1; i < map->m_a2; ++i)
      {
        for (int j = map->m_b1; j < map->m_b2; ++j)
        {
          auto pd = m_provider->GetDescriptor(i, j);
          assert(pd);
          if (pd->m_cellDescriptor && pd->m_cellDescriptor->parent == pd)
          {
            ObjectContext* context = static_cast<ObjectContext*>(pd->m_cellDescriptor->userData);
            if (context)
            {
              LOG_W("RemoveMap %p %s from pos [%d,%d]", pd->m_cellDescriptor, context->Description().c_str(), i, j);
              context->Destory(map);
              pd->m_cellDescriptor->userData = nullptr;
            }
          }
        }
      }
    }

    //********************************************************************************************
    void PartialMapsManager::DeleteFromMap(CellDescriptor* cd,
                                           ObjectContext* context,
                                           const PartialMapPtr& map)
    {
      if (cd->userData == nullptr)
        return;
      
      context->Destory(map);
      cd->userData = nullptr;
    }
    
    //********************************************************************************************
    void PartialMapsManager::Move(CellDescriptor* cd,
                                  const Vec2& source,
                                  const Vec2& dest,
                                  const Morphing& morphing,
                                  const PartialMapPtr& map,
                                  int steps,
                                  float animationDuration)
    {
      auto context = static_cast<ObjectContext*>(cd->userData);
      assert(context);
      
      if (context->m_owner != map)
      {
        context->BecomeOwner(map);
      }
      
      if (context->GetType() == ContextType::ManyPixels)
      {
        auto aabb = cd->GetShape()->GetAABB();
        static_cast<AmorphCellContext*>(context)->MoveAmorphCells(source, morphing, aabb, animationDuration*0.9*(steps + 1));
        return;
      }
      
      if (context->GetType() == ContextType::SinglePixel)
      {
        static_cast<CellContext*>(context)->Move(source, dest, animationDuration*0.9*(steps + 1));
        return;
      }
    }
    
    //********************************************************************************************
    void PartialMapsManager::Attack(ObjectContext* context,
                const Vec2& pos,
                const Vec2& offset,
                float animationDuration)
    {
      context->Attack(pos, offset, animationDuration);
    }
    
  }
}
