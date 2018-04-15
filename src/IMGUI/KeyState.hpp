﻿#pragma once
class KeyState
{
public:
    static bool lClick;
    static bool mClick;
    static bool rClick;

    static bool lClicked;
    static bool mClicked;
    static bool rClicked;

    static bool mouseReset;

    static glm::vec2 mousePosition;
    static glm::vec2 mouseTranslation;
    static glm::vec2 mouseTranslationNormalized;

    static void reset();
};
