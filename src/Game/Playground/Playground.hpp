#pragma once
#include "GameState.hpp"
#include "PointerInfo.hpp"

class AI;
class FreeCamController;
class GBufferSampler;
class Imgui;
class Input;
class InputDispatcher;
class Player;
class PhysicalWorld;
class Scene;
class VehicleEquipment;
class Window;

class Playground : public GameState
{
private:
    std::shared_ptr<Input> m_input;
    std::shared_ptr<Player> m_player;
    std::vector<std::shared_ptr<VehicleEquipment>> m_vehicles;
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
    bool m_useFreecam {true};

    std::unique_ptr<GBufferSampler> m_mouseSampler;

public:
    Playground(Imgui&, InputDispatcher&, Window&);
    ~Playground();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    void renderProcedure(GraphicEngine&);

    Scene& loadScene(const std::string&);
    void spawnPlayer(const std::string&, const glm::mat4&);
    void spawnBot(const std::string&, const glm::mat4&);
    void spawnHostileBot(const std::string&, const glm::mat4&);
};
