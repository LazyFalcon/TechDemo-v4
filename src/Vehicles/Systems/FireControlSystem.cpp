#include "core.hpp"
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include "FireControlSystem.hpp"

glm::vec4 FireControlSystem::recalculate(const glm::vec4& pointInWS){
    return m_pointInWS;
}
void FireControlSystem::updateTarget(const glm::vec4& pointInWS){
    m_pointInWS = pointInWS;
    for(auto& x : m_recalculatedTargetsForGuns) x = recalculate(pointInWS);
}

const glm::vec4& FireControlSystem::getTarget(int i) const {
    return m_recalculatedTargetsForGuns[i];
}
void FireControlSystem::allocateNewLink(){
    ++m_currentLinkId;
    m_recalculatedTargetsForGuns.emplace_back(0,0,0,1);
    m_isNeededToCreateNewLink = false;
}

// always return main target point
int FireControlSystem::idForTurret(){
    return 0;
}
// if needed create new kinematic link
int FireControlSystem::idForGunServo(){
    if(m_isNeededToCreateNewLink) allocateNewLink();

    return m_currentLinkId;
}
int FireControlSystem::idForGun(u64 parentHash){
    if(m_gunParentHash != parentHash) allocateNewLink();
    m_isNeededToCreateNewLink = true;

    return m_currentLinkId;
}
