#include "PlaneSimulator.hpp"
#include "PlanePosition.hpp"
#include "NMEAGenerator.hpp"
#include <cmath>

constexpr double PI = 3.14159265358979323846;

PlaneSimulator::PlaneSimulator(
    const std::string& id,
    double radiusMeters,
    double altitudeMeters,
    double speedMetersPerSec,
    double angleRadians,
    double centerLat,
    double centerLon
)
    : id(id),
    radiusMeters(radiusMeters),
    altitudeMeters(altitudeMeters),
    speedMetersPerSec(speedMetersPerSec),
    angleRadians(angleRadians),
    centerLat(centerLat),
    centerLon(centerLon)
{
    latDegreeMeters = 111000.0;
    lonDegreeMeters = latDegreeMeters * std::cos(centerLat * PI / 180.0);
}

PlanePosition PlaneSimulator::calculatePosition(double angleRad) {
    double x = radiusMeters * std::cos(angleRad); // East
    double y = radiusMeters * std::sin(angleRad); // North

    double deltaLat = y / latDegreeMeters;
    double deltaLon = x / lonDegreeMeters;

    double newLat = centerLat + deltaLat;
    double newLon = centerLon + deltaLon;

    constexpr double deltaAngle = 0.0001;

    double x2 = radiusMeters * std::cos(angleRad + deltaAngle);
    double y2 = radiusMeters * std::sin(angleRad + deltaAngle);

    double deltaX = x2 - x;
    double deltaY = y2 - y;

    double headingRad = std::atan2(deltaX, deltaY);
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


const std::string& PlaneSimulator::getID() const {
    return id;
}