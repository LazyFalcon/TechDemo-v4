#pragma once

/*
* Ponieważ pointer jest jeden, jego stan musi być sharowany pomiędzy użytkownikami

*/
class Pointer
{
    glm::vec2 screenPosition;
    enum Mode {
        InCenter, Free, Menu
    };

};