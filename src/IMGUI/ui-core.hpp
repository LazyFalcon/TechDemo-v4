#pragma once
#include "common.hpp"
#include <stdexcept>

using iBox = glm::ivec4;

class InvaliUiOperation : public std::runtime_error
{
    // std::string whatMsg;
public:
    InvaliUiOperation(const std::string& whatMsg) : std::runtime_error(whatMsg){}
    // const char* what() override
    // {
    //     return whatMsg.c_str();
    // }
};

struct ImguiRenderElement
{
    iBox element;
    u32 color;
    float depth;
};
