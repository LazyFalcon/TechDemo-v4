#pragma once
#include "DecalsAndMarkers.hpp"
#include "VehicleEquipment.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

enum class CameraFilters {
    Clear, InfraRed, Broken, Normal, BlackWhite, NightWision
};

class AiControl;
class IModule;
class Input;
class Joint;
class LightSource;
class VehicleEquipment;

// matrixContainer contains transformations in world space
class ModuleVisualUpdater
{
    std::vector<glm::mat4>* m_matrixContainer {nullptr};
    uint m_boneIndex;
public:
    ModuleVisualUpdater() = default;
    ModuleVisualUpdater(std::vector<glm::mat4>& referenceContainer, uint boneIndex) : m_matrixContainer(&referenceContainer), m_boneIndex(boneIndex){}

    virtual ~ModuleVisualUpdater() = default;
    virtual void setTransform(const btTransform &tr){
        console.clog(__PRETTY_FUNCTION__, m_boneIndex);
        (*m_matrixContainer)[m_boneIndex] = convert(tr);
    }
    virtual void setTransform(const btTransform &tr, u32 i){
        console.clog(__PRETTY_FUNCTION__, m_boneIndex);
         (*m_matrixContainer)[i] = convert(tr);
    }
    virtual void setTransform(const glm::mat4 &tr){
        console.clog(__PRETTY_FUNCTION__, m_boneIndex);
        (*m_matrixContainer)[m_boneIndex] = tr;
    }
    virtual void setTransform(const glm::mat4 &tr, u32 i){
        console.clog(__PRETTY_FUNCTION__, m_boneIndex);
        (*m_matrixContainer)[i] = tr;
    }
    virtual const glm::mat4& getTransform() const {
        console.clog(__PRETTY_FUNCTION__, m_boneIndex);
        return (*m_matrixContainer)[m_boneIndex];
    }
};

class NullModuleVisualUpdater : public ModuleVisualUpdater
{
public:
    virtual void setTransform(const btTransform &tr) override {}
    virtual void setTransform(const btTransform &tr, u32 i) override {}
    virtual void setTransform(const glm::mat4 &tr) override {}
    virtual void setTransform(const glm::mat4 &tr, u32 i) override {}
    virtual const glm::mat4& getTransform() const override {
        return identityMatrix;
    }
};

class ModuleCompoundUpdater
{
    btCompoundShape* m_compound {nullptr};
    glm::mat4 m_currentTransform;
    uint m_childIndex;
public:
    ModuleCompoundUpdater() = default;
    ModuleCompoundUpdater(btCompoundShape* compound, uint childIndex) : m_compound(compound), m_childIndex(childIndex){}
    virtual ~ModuleCompoundUpdater() = default;
    // sets transform in vehicle space
    virtual void setTransform(const btTransform& tr){
        m_compound->updateChildTransform(m_childIndex, tr, false);
    }
    virtual void setTransform(const glm::mat4& tr){
        m_compound->updateChildTransform(m_childIndex, convert(tr), false);
        m_currentTransform = tr;
    }
    virtual const glm::mat4& getTransform() const {
        return m_currentTransform;
    }
};

class NullModuleCompoundUpdater : public ModuleCompoundUpdater
{
public:
    void setTransform(const btTransform& tr) override {}
    void setTransform(const glm::mat4& tr) override {}
    const glm::mat4& getTransform() const override {
        return identityMatrix;
    }
};

class IModule
{
protected:
    VehicleEquipment &eq;
public:
    std::string name;

    virtual void update(float dt) = 0;
    virtual void init(){}
    virtual ~IModule() = default;
};

class LogicTypeModule : public IModule
{
public:
    using IModule::IModule;
};

class PlainModule
{
public:
    std::shared_ptr<Joint> joint;

    IModule *parent {nullptr};

    std::unique_ptr<ModuleVisualUpdater> moduleVisualUpdater;
    std::unique_ptr<ModuleCompoundUpdater> moduleCompoundUpdater;

    glm::mat4 worldTransform; // ?  maybe remove this and use transform stored in bones?

    IModule(VehicleEquipment &eq, ModuleType type) : type(type), eq(eq), moduleVisualUpdater(std::make_unique<NullModuleVisualUpdater>()), moduleCompoundUpdater(std::make_unique<NullModuleCompoundUpdater>()){}

    // ustawia jednocześnie transformację dla kości, dla potomków(również wzgledm świata) i tr compound mesha
    // transformacja jest względem rodzica
    void transform(const glm::mat4& tr){
        console.clog(__PRETTY_FUNCTION__, name);
        worldTransform = getParentTransform() * tr;
        moduleVisualUpdater->setTransform(worldTransform); /// tu wrzucamy pełną trnsformację
        moduleCompoundUpdater->setTransform(parent ? parent->getLocalTransform() * tr : glm::mat4()); /// a tu względem rodzica, no nic, trzeba dodać dodatkowy wektor
    }
    const glm::mat4& getGlmTransform() const {
        return moduleVisualUpdater->getTransform();
    }
    const glm::mat4& getTransform() const {
        return worldTransform;
    }
    const glm::mat4& getLocalTransform() const {// nie używane
        return moduleCompoundUpdater->getTransform();
    }
    const glm::mat4& getParentTransform() const {
        if(not parent) return identityMatrix;
        return parent->getTransform();
    }
    const glm::mat4& getBaseTransform() const {
        return eq.glTrans;
    }
    const glm::mat4& getInvBaseTransform() const {
        return eq.invTrans;
    }

    void updateCommon(float dt){} // TODO: called outside fixed step loop, once per frame i.e. light updates
    virtual void provideControlInterfaceForKeyboard(Input& input){}
    virtual void provideControlInterfaceForXPad(Input& input){}
    virtual void provideControlInterfaceForAI(AiControl& input){}

};

template<typename CC, typename = std::enable_if_t<std::is_base_of<CameraController, CC>::value>>
class ModuleFollower : public CC
{
private:
    IModule* m_module {nullptr};
    glm::vec3 m_position;
public:
    ModuleFollower(IModule *module, glm::vec3 pos) : m_module(module), m_position(pos){}
    template<typename... Args>
    ModuleFollower(IModule *module, glm::vec3 pos, Args&... args) : CC(args...), m_module(module), m_position(pos){}

    void update(float dt) override {
        // CC::updateBaseTransform(m_module->getGlmTransform());
        CC::update(m_module->getGlmTransform()*glm::translate(m_position), dt);
    }
};
