#include "core_pch.hpp"
#include <glm/glm.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include <glm/gtx/matrix_query.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/optimum_pow.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/wrap.hpp>

#ifdef USE_BULLET
#include <btBulletDynamicsCommon.h>
#endif

#include <boost/core/noncopyable.hpp>

#include "Types.hpp"
#include "Constants.hpp"
#include "DefaultResourcePaths.hpp"
#include "TimeEvents.hpp"

using namespace std::literals;

namespace fs = boost::filesystem;


uint frame(); // * returns frame number, should be defined elsewhere
