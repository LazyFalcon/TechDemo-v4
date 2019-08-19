#include "core.hpp"
#include "camera-factory.hpp"
#include "camera-controller.hpp"
#include "Window.hpp"

namespace camera
{

Factory::Factory(Window &window) : windowSize(window.size){}

}
