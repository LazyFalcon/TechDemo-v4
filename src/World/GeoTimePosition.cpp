#include "core.hpp"
#include "Sun.hpp"

#include "GeoTimePosition.hpp"
#include "Colors.hpp"
#include "Yaml.hpp"

GeoTimePosition::GeoTimePosition(const Yaml &sett)
{
    m_date = boost::posix_time::time_from_string(sett["DateTime"].string());
    latitude = sett["Latitude"].number();
    longitude = sett["Longitude"].number();
    tweak = sett["Tweak"].boolean();

    log("Game date and time:", to_simple_string(m_date));

    year = date().year();
    month = date().month();
    day = date().day();

    hours = time().hours();
    minutes = time().minutes();
    seconds = time().seconds();

    totalDaySeconds = seconds + minutes*60 + hours*60*60;
}
GeoTimePosition::~GeoTimePosition(){
}
