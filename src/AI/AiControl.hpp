#pragma once

class AiCommand;
class InputDispatcher;
class Input;

using commandCallback = std::function<void(AiCommand&)>;

class AiControl
{
public:
    virtual ~AiControl() = default;
    virtual void update() = 0;
    virtual void newCommandCallback(commandCallback&&) = 0;
};

class AiControlViaInput : public AiControl
{
public:
    AiControlViaInput(InputDispatcher& inputdispatcher);

    void update() override;
    void newCommandCallback(commandCallback&& callback) override {
        m_putCommandHere = callback;
    }

private:
    std::shared_ptr<Input> m_input;
    commandCallback m_putCommandHere;
};

class AiSelfControl : public AiControl
{
public:
    AiSelfControl() {}

    void update() override {}
    void newCommandCallback(commandCallback&& callback) override {
        m_putCommandHere = callback;
    }

private:
    commandCallback m_putCommandHere;
};
