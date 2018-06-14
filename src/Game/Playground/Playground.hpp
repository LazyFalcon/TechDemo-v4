#pragma once
#include "GameState.hpp"
#include "Scene.hpp"

class FreeCamController;
class GBufferSampler;
class Imgui;
class Input;
class InputDispatcher;
class Player;
class Window;

class Playground : public GameState
{
private:
    Scene m_scene;
    std::shared_ptr<Input> m_input;
    std::shared_ptr<Player> m_player;
    std::unique_ptr<PhysicalWorld> m_physical;
    Window& m_window;
    glm::vec2 m_mousePos;
    glm::vec4 m_mouseWorldPos;
    glm::vec2 m_mouseTranslation;
    glm::vec2 m_mouseTranslationNormalized;
    bool m_cameraRotate {};

    std::shared_ptr<FreeCamController> m_defaultCamera; // TODO: later convert into list of cameras

    std::unique_ptr<GBufferSampler> m_mouseSampler;

public:
    Playground(Imgui&, InputDispatcher&, Window&);
    ~Playground();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    Scene& getScene();
    void renderProcedure(GraphicEngine&);

    void spawnPlayer(const std::string&, glm::vec4);
};
