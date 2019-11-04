#include "core.hpp"
#include "AiControl.hpp"
#include "AiCommand.hpp"
#include "Logger.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"

AiControlViaInput::AiControlViaInput(InputDispatcher& inputDispatcher) : m_input(inputDispatcher.createNew("Ai")) {
    m_input->action("RMB").off([this]() {
        // MoveCommand payload {m_pointerInfo.worldPosition+glm::vec4(0,0,2,0)};
        // AiCommand command {MoveTo};
        // command.payload = &payload;
        // m_putCommandHere(command);
    });

    m_input->activate();
}

void AiControlViaInput::update() {}
