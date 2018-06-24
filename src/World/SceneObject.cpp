#include "core.hpp"
#include "SceneObject.hpp"

uint g_sceneObjectId;

SceneObject::SceneObject(){
    m_id = ++g_sceneObjectId;
    if(m_id == 0) m_id = ++g_sceneObjectId; // ! 0 is invalid value
}
