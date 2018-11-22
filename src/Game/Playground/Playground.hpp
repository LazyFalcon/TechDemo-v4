#pragma once
#include "GameState.hpp"

class FreeCamController;
class GBufferSampler;
class Imgui;
class Input;
class InputDispatcher;
class Player;
class PhysicalWorld;
class Scene;
class Window;

class Playground : public GameState
{
private:
    std::shared_ptr<Input> m_input;
    std::shared_ptr<Player> m_player;
    std::unique_ptr<PhysicalWorld> m_physics;
    std::unique_ptr<Scene> m_scene;
    Window& m_window;
    glm::vec2 m_mousePos;
    glm::vec4 m_mouseWorldPos;
    glm::vec2 m_mouseTranslation;
    glm::vec2 m_mouseTranslationNormalized;
    bool m_cameraRotate {};

    std::shared_ptr<FreeCamController> m_defaultCamera; // TODO: later convert into list of cameras
    bool m_useFreecam {true};

    std::unique_ptr<GBufferSampler> m_mouseSampler;

public:
    Playground(Imgui&, InputDispatcher&, Window&);
    ~Playground();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    void renderProcedure(GraphicEngine&);

    void loadScene(const std::string&);
    void spawnPlayer(const std::string&, glm::vec4);
};
