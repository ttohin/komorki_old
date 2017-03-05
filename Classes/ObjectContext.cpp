//
//  PixelMapContext.cpp
//  prsv
//
//  Created by Anton Simakov on 09.07.16.
//
//

#include "ObjectContext.h"
#include "Logging.h"
#include "PartialMap.h"


using namespace komorki::graphic;

#define SMALL_ANIMATION_TAG 99

namespace komorki
{
namespace graphic
{
  ObjectContext::ObjectContext(PartialMapPtr _owner)
  {
    m_owner = _owner;
  }
  
  ObjectContext::~ObjectContext()
  {
    LOG_W("%s, %s", __FUNCTION__, Description().c_str());
  }
  
  std::string ObjectContext::Description() const
  {
    std::stringstream ss;
    ss <<
    "[" <<
    static_cast<const void*>(this) <<
    " ownwer: " <<
    m_owner->Description() <<
    " cell: " <<
    static_cast<const void*>(m_cell) <<
    "]";
    return ss.str();
  }
  
  void ObjectContext::ForceDestory(PartialMapPtr _owner)
  {
    LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner.get());
    delete this;
  }
  
  Vec2 ObjectContext::GetPosInOwnerBase(Vec2ConstRef pos) const
  {
    return pos - Vec2(m_owner->m_a1, m_owner->m_b1);
  }

}
}

