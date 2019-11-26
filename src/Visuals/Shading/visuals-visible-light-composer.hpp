#pragma once

class VisibleLightComposer
{
private:
    ShadowPool m_shadowPool;

public:
    void processVisibleShadows() {
        // znaleźć nowe
        // posortować po odległości/priorytecie(tak dajemy światłom priorytet, jak bardzo wązne są ich cienie)
        // przeliczyć zapotrzebowanie na tekstury i sprawdzić ilu światłom da się przydzielić
        // przydzielić i przeliczyć macierze projekcji dla świateł

        // na razie bez keszowania(potrzeban jakaś flaga że światło zostało zmodyfikowane): znaleźć światłom obiekty których dotykają, przy pomocy ghost obiektów i bulleta
        // renderujemy każde światło osobno
    }
};
