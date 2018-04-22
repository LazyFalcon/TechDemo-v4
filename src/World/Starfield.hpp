#pragma once

/**
 *  Ok, mamy niebo, dzielimy go na kilka fragment�w by si� leiej renderowa�o
 *
 *  Widoczne fragmenty obszaru wyci�gamy z frustuma
 *
 *  Przy wyszukiwaniu musimy pami�ta� �e obraca si� obserwator,
 *  na razie bez optymalizacji, chcę narysowac gwiazdy
 *
 */

union Color
{
    struct {u8 r; u8 g; u8 b; u8 a;};
    u32 rgba;
};

struct Star
{
    glm::vec4 position;
    Color color;
    float brightness;
};
// moze lepiej starfield?
class Starfield
{
public:
    Starfield(){}
    void regenerate();
    void update();
    VAO upload(std::vector<Star> &stars);

    VAO starsOnGpu;
    u32 starCount {};
};
