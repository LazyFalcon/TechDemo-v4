#pragma once

// used with assume that
class Layout
{
public:
    glm::ivec4 yeld(const glm::ivec4& req){ // obsługa requestowanowanego rozmiaru, ale co jeśli pozycja była też nadpisana? jakiś min/max?
        glm::ivec4 out(req);
        if(m_axis == 0 and m_direction == 1){ // przesunąć x, zmienić szerokość
            out = m_free;
        }
        else if(m_axis == 0 and m_direction == -1){ // zmoieniamy tylko szerokość
            out.x = m_free.x + m_free.z - req.z;
            out.y = m_free.y;
        }
        else if(m_axis == 1 and m_direction == 1){
            out.x = m_free.x;
            out.y = m_free.y + m_free.w - req.w;
        }
        else if(m_axis == 1 and m_direction == -1){
            out.x = m_free.x;
            out.y = m_free.y;
        }
        return out;
    }

    void feedback(const glm::ivec4& v){ // zmniejszenie wolnego obczaru o podany v, z uwzględnieniem kierunku układania
        if(m_axis == 0 and m_direction == 1){ // przesunąć x, zmienić szerokość
            i32 x = v.x + v.z;
            m_free.z -= x - m_free.x;
            m_free.x = x;
        }
        else if(m_axis == 0 and m_direction == -1){ // zmieniamy tylko szerokość
            m_free.z = v.x - m_free.x;
        }
        else if(m_axis == 1 and m_direction == 1){
            m_free.w = v.y - m_free.y;
        }
        else if(m_axis == 1 and m_direction == -1){
            i32 y = v.y + v.w;
            m_free.w -= y - m_free.y;
            m_free.y = y;
        }
    }

    void move(float f){
        if(m_axis == 0) move(f*m_bounds.x);
        else if(m_axis == 1) move(f*m_bounds.y);
    }

    void move(i32 i){
        if(m_axis == 0 and m_direction == 1){ // down
            m_free.z -= i;
        }
        else if(m_axis == 0 and m_direction == -1){ // up
            m_free.z -= i;
            m_free.x += i;
        }
        else if(m_axis == 1 and m_direction == 1){ // right
            m_free.w = i;
        }
        else if(m_axis == 1 and m_direction == -1){ // left
            m_free.w -= i;
            m_free.y += i;
        }
    }

    void setBounds(glm::vec4){
    } // wołane przez Panel
    void compile(){
    } // wołane przez Panel

    Layout& vertical(){
        m_axis = 1;
        return *this;
    } // pionowy układ
    Layout& horizontal(){
        m_axis = 0;
        return *this;
    } // poziomy układ
    Layout& forward(){
        m_direction = 1;
        return *this;
    } // w dół lub w prawo, zgodne z kierunkiem czytania
    Layout& backward(){
        m_direction = -1;
        return *this;
    } // w górę lub w lewo()

    // align, domyślnie środek

    // zgłaszamy że tyle i tyle obiektów o takich wymiarach będzie do wyrysowania
    Layout& prepare(glm::vec4 evenSize, i32 count){
        return *this;
    } // ileśtam obiektów tego samego wymiaru, do rozłożenia zgodnie z algortmem, można stackować :D
    Layout& prepare(const std::vector<glm::vec4>& requestedSizes){
        return *this;
    } // pamiętać o uwzględnieniu
private:
    std::vector<glm::vec4> m_generatedLayout;
    int m_used;

    int m_axis;
    int m_direction;

    glm::vec4 m_bounds;
    glm::vec4 m_free;
};
