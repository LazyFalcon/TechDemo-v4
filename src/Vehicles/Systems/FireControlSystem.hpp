#pragma once

class FireControlSystem
{
private:
    glm::vec4 m_pointInWS;
    glm::vec4 m_recalculatedPointInWS;


    glm::vec4 recalculate(glm::vec4 pointInWS){
        return pointInWS;
    }
public:
    void updateTarget(glm::vec4 pointInWS){
        m_pointInWS = pointInWS;
        m_recalculatedPointInWS = recalculate(m_pointInWS);
    }

    const glm::vec4& getTarget() const {
        return m_recalculatedPointInWS;
    }

};
