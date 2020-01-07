#include "core.hpp"
#include "base-of-game-object.hpp"
#include "GraphicComponent.hpp"
#include "Logger.hpp"
#include "visuals-prepared-scene.hpp"


namespace utils
{
struct Id
{
    BaseOfGameObject* pointee {nullptr};
};

struct IdContainer
{
    std::vector<int> freeIndices;
    std::vector<int> releasedIndices;
    std::vector<Id> ids;

    IdContainer() {
        int capacity = 50000;
        ids.resize(capacity);
        freeIndices.reserve(capacity);
        for(int i = capacity - 1; i > 0; --i) { freeIndices.push_back(i); }
    }
};

IdContainer idContainer;

void releaseIndex(int i) {
    idContainer.releasedIndices.push_back(i);
    if(idContainer.freeIndices.empty())
        idContainer.freeIndices.swap(idContainer.releasedIndices);
}
int acquireIndex() {
    int i = idContainer.freeIndices.back();
    if(not idContainer.freeIndices.empty())
        idContainer.freeIndices.pop_back();

    return i;
}
void updatePointer(int i, BaseOfGameObject* ptr) {
    idContainer.ids[i].pointee = ptr;
}
BaseOfGameObject* deref(int i) {
    return (BaseOfGameObject*)idContainer.ids[i].pointee;
}
// TODO: object 0 szhould be dummy.
BaseOfGameObject* deref(const btRigidBody* rgBody) {
    return (BaseOfGameObject*)idContainer.ids[rgBody->getUserIndex()].pointee;
}
void setRef(int i, btRigidBody* rgBody) {
    return rgBody->setUserIndex(i);
}
}

void BaseOfGameObject::addItselfToShadowCastingList(model::Collection& command) {
    if(command.id == lastRenderCommand)
        return;
    lastRenderCommand = command.id;
    addToShadowCastingList(command);
}
void BaseOfGameObject::addItselfToSceneVisibleList(visuals::PreparedScene& visibleList) {
    if(lastFrame == frame())
        return;
    lastFrame = frame();
    addToSceneVisibleList(visibleList);
}
