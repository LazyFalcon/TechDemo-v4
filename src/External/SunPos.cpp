// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm

#include "SunPos.hpp"
#include <math.h>

namespace sunpos
{

const double pi = 3.14159265358979323846;
const double twopi = 2*pi;
const double rad = pi/180.0;
const double earthMeanRadius = 6371.01;    // In km
const double astronomicalUnit = 149597890;    // In km

SunCoordinates calc(Time udtTime, Location udtLocation){
    // Main variables
    SunCoordinates udtSunCoordinates {};
    double elapsedJulianDays;
    double decimalHours;
    double eclipticLongitude;
    double eclipticObliquity;
    double rightAscension;
    double declination;

    // Auxiliary variables
    double dY;
    double dX;

    // Calculate difference in days between the current Julian Day
    // and JD 2451545.0, which is noon 1 January 2000 Universal Time
    {
        double julianDate;
        long int liAux1;
        long int liAux2;

        // Calculate time of the day in UT decimal hours
        decimalHours = udtTime.hours + (udtTime.minutes + udtTime.seconds / 60.0 ) / 60.0;

        // Calculate current Julian Day
        liAux1 =(udtTime.month-14)/12;
        liAux2=(1461*(udtTime.year + 4800 + liAux1))/4 + (367*(udtTime.month - 2-12*liAux1))/12- (3*((udtTime.year + 4900 + liAux1)/100))/4+udtTime.day-32075;
        julianDate=(double)(liAux2)-0.5+decimalHours/24.0;

        // Calculate difference between current Julian Day and JD 2451545.0
        elapsedJulianDays = julianDate-2451545.0;
    }

    // Calculate ecliptic coordinates (ecliptic longitude and obliquity of the
    // ecliptic in radians but without limiting the angle to be less than 2*Pi
    // (i.e., the result may be greater than 2*Pi)
    {
        double meanLongitude;
        double meanAnomaly;
        double omega;
        omega = 2.1429-0.0010394594*elapsedJulianDays;
        meanLongitude = 4.8950630+ 0.017202791698*elapsedJulianDays; // Radians
        meanAnomaly = 6.2400600+ 0.0172019699*elapsedJulianDays;
        eclipticLongitude = meanLongitude + 0.03341607*sin( meanAnomaly ) + 0.00034894*sin( 2*meanAnomaly ) - 0.0001134 - 0.0000203*sin(omega);
        eclipticObliquity = 0.4090928 - 6.2140e-9*elapsedJulianDays + 0.0000396*cos(omega);
    }

    // Calculate celestial coordinates ( right ascension and declination ) in radians
    // but without limiting the angle to be less than 2*Pi (i.e., the result may be
    // greater than 2*Pi)
    {
        double sin_EclipticLongitude;
        sin_EclipticLongitude= sin( eclipticLongitude );
        dY = cos( eclipticObliquity ) * sin_EclipticLongitude;
        dX = cos( eclipticLongitude );
        rightAscension = atan2( dY,dX );

        if( rightAscension < 0.0 )
            rightAscension = rightAscension + twopi;

        declination = asin( sin( eclipticObliquity )*sin_EclipticLongitude );
    }

    // Calculate local coordinates ( azimuth and zenith angle ) in degrees
    {
        double greenwichMeanSiderealTime;
        double localMeanSiderealTime;
        double latitudeInRadians;
        double hourAngle;
        double cos_Latitude;
        double sin_Latitude;
        double cos_HourAngle;
        double parallax;
        greenwichMeanSiderealTime = 6.6974243242 + 0.0657098283*elapsedJulianDays + decimalHours;
        localMeanSiderealTime = (greenwichMeanSiderealTime*15 + udtLocation.longitude)*rad;
        hourAngle = localMeanSiderealTime - rightAscension;
        latitudeInRadians = udtLocation.latitude*rad;
        cos_Latitude = cos( latitudeInRadians );
        sin_Latitude = sin( latitudeInRadians );
        cos_HourAngle= cos( hourAngle );
        udtSunCoordinates.zenithAngle = (acos( cos_Latitude*cos_HourAngle*cos(declination) + sin( declination )*sin_Latitude));
        dY = -sin( hourAngle );
        dX = tan( declination )*cos_Latitude - sin_Latitude*cos_HourAngle;
        udtSunCoordinates.azimuth = atan2( dY, dX );

        if ( udtSunCoordinates.azimuth < 0.0 )
            udtSunCoordinates.azimuth = udtSunCoordinates.azimuth + twopi;

        udtSunCoordinates.azimuth = udtSunCoordinates.azimuth/rad;
        // Parallax Correction
        parallax=(earthMeanRadius/astronomicalUnit) *sin(udtSunCoordinates.zenithAngle);
        udtSunCoordinates.zenithAngle=(udtSunCoordinates.zenithAngle + parallax)/rad;
    }
    return udtSunCoordinates;
}

}
