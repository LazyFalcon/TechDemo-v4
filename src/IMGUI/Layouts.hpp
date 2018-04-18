#pragma once

class Layout
{
public:
    virtual ~Layout() = default;
    virtual glm::vec4 yeld() = 0;
    virtual void setBounds(glm::vec4) = 0;
    virtual void compile() = 0;
};

// used with assume that
class BasicLayout : public Layout
{
public:
    glm::vec4 yeld() override {
        if(m_used>=m_generatedLayout.size()) return {};
        return m_generatedLayout[m_used++];
    }

    void setBounds(glm::vec4) override {
    } // wołane przez Panel
    void compile() override {
    } // wołane przez Panel

    BasicLayout& vertical(){
        return *this;
    } // pionowy układ
    BasicLayout& horizontal(){
        return *this;
    } // poziomy układ
    BasicLayout& forward(){
        return *this;
    } // w dół lub w prawo, zgodne z kierunkiem czytania
    BasicLayout& backward(){
        return *this;
    } // w górę lub w lewo()

    // zgłaszamy że tyle i tyle obiektów o takich wymiarach będzie do wyrysowania
    BasicLayout& prepare(glm::vec4 evenSize, i32 count){
        return *this;
    } // ileśtam obiektów tego samego wymiaru, do rozłożenia zgodnie z algortmem, można stackować :D
    BasicLayout& prepare(const std::vector<glm::vec4>& requestedSizes){
        return *this;
    } // pamiętać o uwzględnieniu
private:
    std::vector<glm::vec4> m_generatedLayout;
    int m_used;
};
