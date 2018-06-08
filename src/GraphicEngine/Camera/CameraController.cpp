#include "Utils.hpp"

#include "CameraController.hpp"
#include "Constants.hpp"
#include "Window.hpp" // TODO: fix camera initialization
#include "Logging.hpp"
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
    return *activeCamera;
}


void CopyOnlyPosition::update(const glm::mat4& parentTransform, float dt){
    if(not hasFocus()) return;

    Camera::setTargetPivot(parentTransform[3]);
    Camera::evaluate(dt);

}

// * X axis of camera must be placed in plane XY of parent
// * With unchanged Z axis
void CopyPlane::update(const glm::mat4& parentTransform, float dt){
    if(not hasFocus()) return;

    Camera::setTargetPivot(parentTransform[3]); // * the best is to have this only copying without smoothing

    glm::vec3 x = Camera::x().xyz();
    glm::vec3 z = Camera::z().xyz();

    glm::vec3 zp = parentTransform[2].xyz();

    //* find angle between x and plane
    glm::vec3 xInPlane = glm::normalize(glm::cross(zp, z));

    Camera::target.basis *= glm::rotation(x, xInPlane);

    Camera::evaluate(dt);
}







PinnedCamController::PinnedCamController(){
    // Camera::target.positionSmooth = 1.f;
    // Camera::target.basisSmooth = 0.3f;
    // // Camera::target.basisSmooth = 0.5f;
    // Camera::offset.z = 0.f;
    // Camera::offset.y = 0;
    // Camera::postOffset.z = 0;
}
void PinnedCamController::update(float dt){
    // if(not hasFocus()) return; // FIX: powinno to stąd wylecieć!
    // auto mat = baseTransform;
    // glm::vec4 position = mat*offsetPosition;
    // Camera::setPosition(position);
    // Camera::rotationCenter = position;
    // Camera::rotate({0,0});
    // {
    //     glm::vec4 x = glm::normalize(X4 * Camera::target.basis);
    //     glm::vec4 y = glm::normalize(Y4 * Camera::target.basis);
    //     glm::vec4 z = glm::normalize(Z4 * Camera::target.basis);

    //     //* uh, what a shit. it calculates new matrix with camera Z, plane normal as camera Y(little moded, to be othogonal)
    //     glm::vec4 zp = glm::normalize(mat * Z4);
    //     glm::vec4 xp = -glm::normalize(cross(z, zp));
    //     glm::vec4 yp = glm::normalize(cross(z, xp));
    //     // xp is orthogonal to Camera::at and is in plane of module
    //     // new Camera::target matrix is xp, yp, z
    //     float angle = -acos(glm::dot(xp, x))*glm::sign(glm::dot(cross(x, xp), z));
    //     if(std::isnan(angle)){
    //         error("CameraController::angle is nan");
    //         angle = 0;
    //     }

    //     Camera::euler.z = angle;

    //     glm::mat4 m = glm::mat4(xp,yp,z, W4);
    //     // Camera::target.basis *= glm::angleAxis(angle, z.xyz());
    //     Camera::target.basis = glm::toQuat(glm::affineInverse(m));
    // }
    // Camera::evaluate(dt);
}

FreeCamController::FreeCamController(){
}
void FreeCamController::update(float dt){
    if(not hasFocus()) return;
    Camera::evaluate(dt);
}

FollowingCamController::FollowingCamController(){
    Camera::offset.z = 25.f;
    // Camera::target.positionSmooth = 1.0f;
    Camera::target.positionSmooth = 0.09f;
    Camera::offset.y = 3;
    Camera::postOffset.z = -1;
    Camera::euler = glm::vec3(0,1,0);
}
void FollowingCamController::update(float dt){
    if(not hasFocus()) return;
    glm::vec4 position = baseTransform*W;
    Camera::setPosition(position);
    Camera::evaluate(dt);
}

TopdownCamController::TopdownCamController(){
    Camera::offset.z = 45.f;
    Camera::constraints.distance = glm::vec2(5,65);
    Camera::target.positionSmooth = 0.09f;
    Camera::offset.y = 3;
    Camera::postOffset.z = -1;
}
void TopdownCamController::update(float dt){
    if(not hasFocus()) return;
    glm::vec4 position = baseTransform*W;
    Camera::setPosition(position);
    Camera::evaluate(dt);
}
