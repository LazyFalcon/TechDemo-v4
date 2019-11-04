#pragma once

class FireControlSystem
{
private:
    glm::vec4 m_pointInWS;
    // todo: in futre store all kinematic link parameters
    std::vector<glm::vec4> m_recalculatedTargetsForGuns;

    bool m_isNeededToCreateNewLink {true};
    int m_currentLinkId {0};
    u64 m_gunParentHash {0};

    glm::vec4 recalculate(const glm::vec4& pointInWS);
    void allocateNewLink();

public:
    FireControlSystem() : m_recalculatedTargetsForGuns(1, {0, 0, 0, 1}) {}
    void updateTarget(const glm::vec4& pointInWS);
    const glm::vec4& getTarget(int i) const;

    int idForTurret();
    int idForGunServo();
    int idForGun(u64 parentHash);

    void recalculateForTargetMovement(const glm::vec4& position, const glm::vec4& velocity);
};
