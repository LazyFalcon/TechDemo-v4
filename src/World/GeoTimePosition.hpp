#pragma once
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
// http://www.boost.org/doc/libs/1_55_0/doc/html/date_time/posix_time.html#time_duration_header
using namespace boost::gregorian;
using namespace boost::posix_time;

class Yaml;

struct GeoTimePosition
{
    GeoTimePosition(const Yaml& sett);
    ~GeoTimePosition();

    GeoTimePosition& operator=(const GeoTimePosition& geo) {
        m_date = geo.m_date;
        return *this;
    }

    float longitude {0}; // horizontal value, vertical lines, [0 : 2*pi]
    float latitude {0};  // vertical value, horizontal lines, [-pi/2 : pi/2]

    boost::posix_time::ptime m_date;

    u32 modifier {3};
    const u32 modifierList[9] {0, 1, 10, 50, 200, 500, 1000, 3000, 10000};
    u32 year;
    u32 month;
    u32 day;
    u32 hours;
    u32 minutes;
    u32 seconds;
    double totalDaySeconds;

    bool tweak;

    void update(u64 dtMs) {
        m_date = m_date + boost::posix_time::millisec(dtMs * modifierList[modifier]);
    }
    auto date() {
        return m_date.date();
    }
    auto time() {
        return m_date.time_of_day();
    }
};
