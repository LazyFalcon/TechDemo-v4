#include "ui.hpp"

template<typename T>
class DropDownList
{
private:
    using ValuePair = std::pair<std::string, T>;
    std::vector<ValuePair> m_values;
    ValuePair m_selected;
    bool m_expanded{false};
public:
    DropDownList(std::vector<ValuePair> values, ValuePair selected) : m_values(std::move(values)), m_selected(selected){}
    void execute(Panel& parentPanel, glm::vec2 itemSize){
        Panel sub(parentPanel);
        sub.width(itemSize[1]).height(itemSize[1])();
        sub.layout().spacing(0).padding({}).toRight();

        bool shouldBeExpandedInNextFrame = m_expanded;

        sub.item().w(itemSize[1]).h(itemSize[1])().formatting(Text::Centered).textColor(0x000000ff).symbol(u"\xe011");
        sub.item().w(itemSize[0]-itemSize[1]).h(itemSize[1])().formatting(Text::Centered).textColor(0x000000ff).text(m_selected.first)
           .action([this, &shouldBeExpandedInNextFrame]{shouldBeExpandedInNextFrame = !m_expanded;});
        if(m_expanded){
            Panel panel(parentPanel.getUi());
            float height = m_values.size() * itemSize[1];
            panel.x(sub.getSize().x).y(sub.getSize().y-height)
                 .width(itemSize[0]).height(height)();
            panel.color(0);
            panel.actionOutside([this, &shouldBeExpandedInNextFrame]{shouldBeExpandedInNextFrame = false;});
            panel.layout().spacing(0).padding({}).toDown();

            for(auto&it : m_values)
                panel.item().w(itemSize[0]).h(itemSize[1])().formatting(Text::Centered).text(it.first);
        }

        m_expanded = shouldBeExpandedInNextFrame;
    }

};

void checkbox(bool& value, const std::string& text, Panel& parentPanel, glm::vec2 size){
    Panel panel(parentPanel);
    panel.width(size[0]).height(size[1]).color(0)().layout().spacing(0).padding({}).toRight();

    panel.item().w(size[1]).h(size[1])().formatting(Text::Centered).textColor(0x000000ff).symbol(value? u"\x2b1b" : u"\x2b1c" ).switchBool(value);
    panel.item().w(size[0]-size[1]).h(size[1])().formatting(Text::Centered).textColor(0x000000ff).text(text).switchBool(value);
}
