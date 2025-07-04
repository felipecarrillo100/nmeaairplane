#include "PlaneSimulator.hpp"
#include "NMEAGenerator.hpp"

#include <cmath>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

constexpr double PI = 3.14159265358979323846;

// Manhattan
constexpr double CenterLongitude = -73.9712;
constexpr double CenterLatitude = 40.7831;

// Paris
// constexpr double CenterLongitude = 2.2943067050001695;
// constexpr double CenterLatitude = 48.85822780194177;

// Sao Paolo
// constexpr double CenterLatitude = -23.544613569973116;
// constexpr double CenterLongitude = -46.66660718174367;

// Sydney
// constexpr double CenterLatitude = -33.85776741122166;
// constexpr double CenterLongitude = 151.21549760187668;

// Shanghai
// constexpr double CenterLatitude = 31.240735926475473;
// constexpr double CenterLongitude = 121.5004677369338;

PlaneSimulator::PlaneSimulator()
        : radiusMeters(10000.0),
          altitudeMeters(12000 * 0.3048), // feet to meters
          speedMetersPerSec(72.0),        // ~140 knots
          centerLat(CenterLatitude),
          centerLon(CenterLongitude),
          angleRadians(0.0)
{
    latDegreeMeters = 111000.0;
    lonDegreeMeters = latDegreeMeters * std::cos(centerLat * PI / 180.0);
}

PlanePosition PlaneSimulator::calculatePosition(double angleRad) {
    // Current position on circle (flat Earth)
    double x = radiusMeters * std::cos(angleRad); // East
    double y = radiusMeters * std::sin(angleRad); // North

    double deltaLat = y / latDegreeMeters;
    double deltaLon = x / lonDegreeMeters;

    double newLat = centerLat + deltaLat;
    double newLon = centerLon + deltaLon;

    // Calculate heading by difference with a small delta angle
    constexpr double deltaAngle = 0.0001;

    double x2 = radiusMeters * std::cos(angleRad + deltaAngle);
    double y2 = radiusMeters * std::sin(angleRad + deltaAngle);

    double deltaX = x2 - x;
    double deltaY = y2 - y;

    double headingRad = std::atan2(deltaX, deltaY);  // atan2(East, North)
    double headingDeg = headingRad * 180.0 / PI;
    if (headingDeg < 0) headingDeg += 360.0;

    return PlanePosition{ newLat, newLon, altitudeMeters, speedMetersPerSec, headingDeg };
}

PlaneSimulator::NMEAMessages PlaneSimulator::getNextNMEAMessages() {
    PlanePosition pos = calculatePosition(angleRadians);

    NMEAMessages msgs;
    msgs.gprmc = NMEA::generateGPRMC(pos);
    msgs.gpgga = NMEA::generateGPGGA(pos);

    double angularSpeed = speedMetersPerSec / radiusMeters;
    angleRadians = std::fmod(angleRadians + angularSpeed, 2 * PI);

    return msgs;
}
