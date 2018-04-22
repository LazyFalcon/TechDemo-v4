#include "Utils.hpp"

#include "CameraController.hpp"
#include "Constants.hpp"
#include "Window.hpp" // TODO: fix camera initialization
#include "Logging.hpp" // TODO: fix camera initialization
#include <glm/gtx/projection.hpp>
#include <glm/gtx/matrix_query.hpp>

CameraController* CameraController::activeCamera {nullptr};
std::list<CameraController*> CameraController::listOf;

CameraController::CameraController(){
    listOf.push_back(this);
    if(not activeCamera) activeCamera = this;
}
CameraController::CameraController(const std::string &smode){
    listOf.push_back(this);
    if(not activeCamera) activeCamera = this;
}
CameraController::~CameraController(){
    listOf.remove(this);
}

void CameraController::focus(){
    activeCamera = this;
}
bool CameraController::hasFocus() const {
    return this == activeCamera;
}

Camera& CameraController::getActiveCamera(){
    return activeCamera->camera;
}

PinnedCamController::PinnedCamController(){
    camera.target.positionSmooth = 1.f;
    camera.target.basisSmooth = 0.3f;
    // camera.target.basisSmooth = 0.5f;
    camera.distanceToOrigin = 0.f;
    camera.offset.y = 0;
    camera.postOffset.z = 0;
}
void PinnedCamController::update(float dt){
    if(not hasFocus()) return; // FIX: powinno to stąd wylecieć!
    auto mat = baseTransform;
    glm::vec4 position = mat*offsetPosition;
    camera.setPosition(position);
    camera.origin = position;
    camera.rotate({0,0});
    {
        glm::vec4 x = glm::normalize(X4 * camera.target.basis);
        glm::vec4 y = glm::normalize(Y4 * camera.target.basis);
        glm::vec4 z = glm::normalize(Z4 * camera.target.basis);

        glm::vec4 zp = glm::normalize(mat * Z4);
        glm::vec4 xp = -glm::normalize(cross(z, zp));
        glm::vec4 yp = glm::normalize(cross(z, xp));
        // xp is orthogonal to camera.at and is in plane of module
        // new camera.target matrix is xp, yp, z
        float angle = -acos(glm::dot(xp, x))*glm::sign(glm::dot(cross(x, xp), z));
        if(std::isnan(angle)){
            error("CameraController::angle is nan");
            angle = 0;
        }

        camera.euler.z = angle;

        glm::mat4 m = glm::mat4(xp,yp,z, W4);
        // camera.target.basis *= glm::angleAxis(angle, z.xyz());
        camera.target.basis = glm::toQuat(glm::affineInverse(m));
    }
    camera.calc(dt);
}

FreeCamController::FreeCamController(){
}
void FreeCamController::update(float dt){
    if(not hasFocus()) return;
    camera.calc(dt);
}

FollowingCamController::FollowingCamController(){
    camera.distanceToOrigin = 25.f;
    // camera.target.positionSmooth = 1.0f;
    camera.target.positionSmooth = 0.09f;
    camera.offset.y = 3;
    camera.postOffset.z = -1;
    camera.euler = glm::vec3(0,1,0);
}
void FollowingCamController::update(float dt){
    if(not hasFocus()) return;
    glm::vec4 position = baseTransform*W;
    camera.setPosition(position);
    camera.calc(dt);
}

TopdownCamController::TopdownCamController(){
    camera.distanceToOrigin = 45.f;
    camera.constraints.distance = glm::vec2(5,65);
    camera.target.positionSmooth = 0.09f;
    camera.offset.y = 3;
    camera.postOffset.z = -1;
}
void TopdownCamController::update(float dt){
    if(not hasFocus()) return;
    glm::vec4 position = baseTransform*W;
    camera.setPosition(position);
    camera.calc(dt);
}
