class Moon
{
public:
    Moon(shared_ptr<GeoTimePosition> &geoTimePosition) : geoTimePosition(geoTimePosition), lightDirectionVector(glm::normalize(glm::vec4(20,36,-10,0))){}

    void update(Atmosphere &atm);
    glm::vec4 getVector(){
        return glm::normalize(lightDirectionVector);
    }
    glm::vec4 lightDirectionVector {};
    glm::vec4 lightColor {};
private:
    glm::vec4 polarToVector();
    void calcSunPosition_SUNAE();
    void calcSunPosition_DIN();
    glm::vec4 calcSunColor(Atmosphere &atmosphere);
    void modify();
    shared_ptr<GeoTimePosition> &geoTimePosition;
    // sun position, on sky, polar
    float azimuth {0}; // horizontal, radians
    float elevation {0}; // vertical, radians
    float height {0};

    float radialSize {0.15};
    HexColor color;
};
