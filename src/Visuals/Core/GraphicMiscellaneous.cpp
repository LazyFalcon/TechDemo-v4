#include "core.hpp"
#include "GraphicMiscellaneous.hpp"
#include "Context.hpp"

BlurParameters::BlurParameters(Context& c) : blurPolygon(), stages({2, 4, 8}) {}
