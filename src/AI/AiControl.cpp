#include "core.hpp"
#include "input.hpp"
#include "input-dispatcher.hpp"
#include "AiControl.hpp"
#include "AiCommand.hpp"

AiControlViaInput::AiControlViaInput(InputDispatcher& inputdispatcher):
    m_input(inputDispatcher.createNew("Ai"))
{

    m_input.action("LMB").off([this](){
        log("Move order");
        AiCommand command {MoveTo};
        m_putCommandHere(command);
    });


    m_input->activate();
}

void AiControlViaInput::update(){}
