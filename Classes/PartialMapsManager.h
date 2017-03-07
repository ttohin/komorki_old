//
//  PartialMapsManager.h
//  Komorki
//
//  Created 04.03.17.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <list>
#include "Common.h"
#include "WorldUpdateResult.h"
#include "Genom.h"

namespace komorki
{
  class IPixelWorld;
  
  namespace graphic
  {
    class ObjectContext;
    class PartialMap;

    using PartialMapPtr = std::shared_ptr<PartialMap>;
    using Maps = std::unordered_map<Vec2, PartialMapPtr, Vec2Hasher>;
    
    class PartialMapsManager
    {
    public:
      struct CreateMapArg
      {
        Rect rect;
        cocos2d::Vec2 graphicPos;
      };
      
      using CreateMapArgs = std::list<CreateMapArg>;
      using RemoveMapArgs = std::list<Vec2>;
      
      void Update(const CreateMapArgs& createMapArgs,
                  const RemoveMapArgs& mapsToRemove,
                  const WorldUpdateResult& worldUpdate,
                  float animationDuration);
      
      const Maps& GetMaps() const;
      void EnableAnimation(bool enableAnimations, bool enableFancyAnimations);
      
    private:
      using GraphicContextList = std::list<ObjectContext*>;
      
      PartialMapPtr GetMap(Vec2ConstRef pos);
      PartialMapPtr CreateMap(const CreateMapArg& args);
      ObjectContext* CreateObjectContext(CellDescriptor* cell,
                                         const PartialMapPtr& map);
      void RemoveMap(const PartialMapPtr& map);

      void DeleteFromMap(CellDescriptor* cd, ObjectContext* context, const PartialMapPtr& map);
      void Move(CellDescriptor* cd,
                const Vec2& source,
                const Vec2& dest,
                const Morphing& morphing,
                const PartialMapPtr& map,
                int steps,
                float animationDuration);
      void Attack(ObjectContext* context,
                  const Vec2& pos,
                  const Vec2& offset,
                  float animationDuration);
      ObjectContext* BornNewAmorphCell(CellDescriptor* cell,
                                       Vec2ConstRef source,
                                       const PartialMapPtr& map,
                                       float animationDuration);
      
      Maps m_map;


      // TODO: move the fields to constructor make them private
    public:
      cocos2d::Node* m_mainNode;
      cocos2d::Node* m_lightNode;
      std::shared_ptr<IPixelWorld> m_provider;
      Rect m_visibleArea; // visible area
      bool m_enableAnimations = false;
      bool m_enableFancyAnimaitons = false;
    };
  }
}

