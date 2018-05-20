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

        sub.item().w(itemSize[1]).h(itemSize[1])().text("\\/");
        sub.item().w(itemSize[0]-itemSize[1]).h(itemSize[1])().text(m_selected.first)
           .action([this]{m_expanded = !m_expanded;})
           .actionOutside([this]{m_expanded = false;});
        if(m_expanded){
            Panel panel(parentPanel.getUi());
            float height = m_values.size() * itemSize[1];
            panel.x(sub.getSize().x).y(sub.getSize().y-height)
                 .width(itemSize[0]).height(height)();
            panel.color(0);
            panel.layout().spacing(0).padding({}).toDown();
            for(auto&it : m_values)
                panel.item().w(itemSize[0]).h(itemSize[1])().text(it.first);
        }

    }

};
