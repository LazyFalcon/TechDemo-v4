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

    m_input->action("LMB").off([this](){
        log("Move order", m_pointerInfo.worldPosition);
        MoveCommand payload {m_pointerInfo.worldPosition};
        AiCommand command {MoveTo};
        command.payload = &payload;
        m_putCommandHere(command);
    });


    m_input->activate();
}

void AiControlViaInput::update(){}
