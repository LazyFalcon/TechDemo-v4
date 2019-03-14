#pragma once

class IModule;
class IWeapon;


/*
 * fire events should be responsible for playing proper animation, sound, instantiate particle and animate module
*/
class MountedWeapon
{
private:
    std::shared_ptr<IWeapon> m_weapon;
    IModule& m_moduleAttachedTo;
    glm::vec4 m_pointOfAttachment;
    glm::vec4 m_directionOfFire;
    bool m_active;
    std::function<void(void)> m_onFireEvent;
public:
    void fire();
    void update(float ms);
    void on();
    void off();
};
