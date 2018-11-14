#include "core.hpp"
#include "BaseGameObject.hpp"

GameObjectIdProvider BaseGameObject::m_idProvider;

BaseGameObject* deref(int idx){
    return BaseGameObject::m_idProvider.get(idx).pointerTo;
}
