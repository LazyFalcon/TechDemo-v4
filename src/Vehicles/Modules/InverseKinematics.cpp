#include "core.hpp"
#include "Servomechanism.hpp"
#include "Yaml.hpp"

// std::optional<Servomechanism::ValueTarget> Servomechanism::retrieveAxis(const Yaml& params, int idx) const {
//     std::optional<Servomechanism::ValueTarget> out;
//     if(not params["Axis"][idx].boolean()) return out;

//     out.emplace(params["Value"][idx].number(), 0.02f);

//     if(params["Min"][idx].number() == params["Max"][idx].number())
//         out->limits.emplace(params["Min"][idx].number() == params["Max"][idx].number());

//     return out;
// }
