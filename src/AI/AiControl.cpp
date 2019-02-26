#include "core.hpp"
#include "input.hpp"
#include "input-dispatcher.hpp"
#include "AiControl.hpp"
#include "AiCommand.hpp"
#include "Logging.hpp"
#include "PointerInfo.hpp"

AiControlViaInput::AiControlViaInput(InputDispatcher& inputDispatcher, PointerInfo& pointerInfo):
    m_input(inputDispatcher.createNew("Ai")),
    m_pointerInfo(pointerInfo)
{

    m_input->action("RMB").off([this](){
        log("Move order", m_pointerInfo.worldPosition);
        MoveCommand payload {m_pointerInfo.worldPosition+glm::vec4(0,0,2,0)};
        AiCommand command {MoveTo};
        command.payload = &payload;
        m_putCommandHere(command);
    });


    m_input->activate();
}

void AiControlViaInput::update(){}
