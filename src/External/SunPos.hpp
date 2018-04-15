// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm
// Dez: adjusted to my standards with full respect to devs
#pragma once

namespace sunpos
{

struct Time
{
    int year;
    int month;
    int day;
    double hours;
    double minutes;
    double seconds;
};

struct Location
{
    double longitude; // E-W, W is negative
    double latitude; // N-S,
};

struct SunCoordinates
{
    double zenithAngle;
    double azimuth;
};

SunCoordinates calc(Time udtTime, Location udtLocation);

}
