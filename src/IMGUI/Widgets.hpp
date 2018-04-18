struct Button
{};

struct EditBox
{};

struct TextBox : public Button
{};

class Text
{
public:
    Text(const std::string& text) : m_text(text){}
    Text& font();
    Text& size();
    // czy dało by się jakaś magią wyciągnąć rozmiar przycisku/

    iBox renderedBox;
private:
    const std::string& m_text;
};
