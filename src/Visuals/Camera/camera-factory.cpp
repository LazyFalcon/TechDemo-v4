#include "core.hpp"
#include "camera-factory.hpp"
#include "Window.hpp"
#include "camera-controller.hpp"

namespace camera
{
Factory::Factory(Window& window) : windowSize(window.size) {}

}
