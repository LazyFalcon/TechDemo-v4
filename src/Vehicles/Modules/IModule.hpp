#pragma once
#include "Joint.hpp"
#include "Utils.hpp"
#include "Vehicle.hpp"

enum class CameraFilters {
    Clear, InfraRed, Broken, Normal, BlackWhite, NightWision
};

class AiControl;
class IModule;
class Input;
class Joint;
class LightSource;

// matrixContainer contains transformations in world space
class ModuleVisualUpdater
{
private:
    std::vector<glm::mat4>* m_matrixContainer {nullptr};
    uint m_boneIndex;
public:
    ModuleVisualUpdater() = default;
    ModuleVisualUpdater(std::vector<glm::mat4>& referenceContainer, uint boneIndex) : m_matrixContainer(&referenceContainer), m_boneIndex(boneIndex){}

    virtual ~ModuleVisualUpdater() = default;
    virtual void setTransform(const btTransform &tr){
        (*m_matrixContainer)[m_boneIndex] = convert(tr);
    }
    virtual void setTransform(const btTransform &tr, u32 i){
         (*m_matrixContainer)[i] = convert(tr);
    }
    virtual void setTransform(const glm::mat4 &tr){
        (*m_matrixContainer)[m_boneIndex] = tr;
    }
    virtual void setTransform(const glm::mat4 &tr, u32 i){
        (*m_matrixContainer)[i] = tr;
    }
    virtual const glm::mat4& getTransform() const {
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
private:
    btCompoundShape* m_compound {nullptr};
    glm::mat4 m_transformRelativeToBase;
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
        m_transformRelativeToBase = tr;
    }
    virtual const glm::mat4& getTransform() const {
        return m_transformRelativeToBase;
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
public:
    IModule(const std::string& name, Vehicle &vehicle, IModule* parent) :
        name(name),
        vehicle(vehicle),
        parent(parent),
        moduleVisualUpdater(std::make_unique<NullModuleVisualUpdater>()),
        moduleCompoundUpdater(std::make_unique<NullModuleCompoundUpdater>()){}
    virtual ~IModule() = default;

    virtual void update(float dt) = 0;
    void transform(const glm::mat4& tr){
        moduleVisualUpdater->setTransform(getParentTransform() * (localTransform * tr)); // here goes full world transform for rendering
        moduleCompoundUpdater->setTransform(parent ? parent->getLocalTransform() * (localTransform * tr) : glm::mat4(1)); // and here local transform for bullets compound
    }
    const glm::mat4& getTransform() const { // full world transform
        return moduleVisualUpdater->getTransform();
    }
    const glm::mat4& getLocalTransform() const { // full transform in vehicle space
        return moduleCompoundUpdater->getTransform();
    }
    const glm::mat4& getParentTransform() const { // full world transform of parent module
        if(not parent) return identityMatrix;
        return parent->getTransform();
    }
    const glm::mat4& getParentLocalTransform() const { // full world transform of parent module
        if(not parent) return identityMatrix;
        return parent->getLocalTransform();
    }
    const glm::mat4& getBaseTransform() const { // full world transform of
        return vehicle.glTrans;
    }
    const glm::mat4& getInvBaseTransform() const {
        return vehicle.invTrans;
    }

    std::string name;
    Vehicle &vehicle;
    IModule *parent {nullptr};
    glm::mat4 localTransform; // from parent module to origin, apply before local transormations!
    std::unique_ptr<ModuleVisualUpdater> moduleVisualUpdater;
    std::unique_ptr<ModuleCompoundUpdater> moduleCompoundUpdater;
};

template<typename CC, typename = std::enable_if_t<std::is_base_of<CameraController, CC>::value>>
class ModuleFollower : public CC
{
private:
    IModule& m_module {nullptr};
public:
    ModuleFollower(IModule& module) : m_module(module){}

    template<typename... Args>
    ModuleFollower(IModule *module, const glm::mat4& cameraRelativeMatrix, Args&... args) : CC(m_module->getTransform(), cameraRelativeMatrix, args...), m_module(module){}

    void update(float dt) override {
        CC::update(m_module->getTransform(), dt);
    }
};
