#pragma once
#include "GameState.hpp"
#include "PointerInfo.hpp"

class AI;
class Context;
class FreeCamController;
class GBufferSampler;
class Imgui;
class Input;
class InputDispatcher;
class PhysicalWorld;
class Player;
class Scene;
class Vehicle;
class Window;

class Playground : public GameState
{
private:
    std::shared_ptr<Input> m_input;
    std::shared_ptr<Player> m_player;
    std::vector<std::shared_ptr<Vehicle>> m_vehicles;
    std::unique_ptr<PhysicalWorld> m_physics;
    Window& m_window;
    std::unique_ptr<Scene> m_scene;
    glm::vec2 m_mousePos;
    glm::vec4 m_mouseWorldPos;
    glm::vec2 m_mouseTranslation;
    glm::vec2 m_mouseTranslationNormalized;
    bool m_cameraRotate {};
    PointerInfo m_pointerInfo;

    // std::shared_ptr<FreeCamController> m_defaultCamera; // TODO: later convert into list of cameras
    int m_selectedCamera {0};
    bool m_useFreecam {false};

    std::unique_ptr<GBufferSampler> m_mouseSampler;

public:
    Playground(Imgui&, InputDispatcher&, Window&);
    ~Playground();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    void renderProcedure(GraphicEngine&);

    PhysicalWorld& getPhysics(){
        return *m_physics;
    }

    Scene& loadScene(const std::string&);
    void spawnPlayer(const std::string&, const glm::mat4&);
    void spawnBot(const std::string&, const glm::mat4&, Context&);
    void spawnHostileBot(const std::string&, const glm::mat4&);
};
