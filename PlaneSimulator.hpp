#ifndef PLANESIMULATOR_HPP
#define PLANESIMULATOR_HPP

#include <string>
#include "PlanePosition.hpp"

class PlaneSimulator {
public:
    PlaneSimulator();

    struct NMEAMessages {
        std::string gprmc;
        std::string gpgga;
    };

    NMEAMessages getNextNMEAMessages();

private:
    PlanePosition calculatePosition(double angleRad);

    std::string formatLatitude(double lat);
    std::string formatLongitude(double lon);
    std::string formatNMEATime();
    std::string formatNMEADate();
    std::string calculateChecksum(const std::string& sentence);
    std::string generateGPRMC(const PlanePosition& pos);
    std::string generateGPGGA(const PlanePosition& pos);

private:
    const double PI = 3.14159265358979323846;

    double radiusMeters;
    double altitudeMeters;
    double speedMetersPerSec;
    double centerLat;
    double centerLon;

    double angleRadians;

    double latDegreeMeters;  // meters per degree latitude
    double lonDegreeMeters;  // meters per degree longitude (varies by latitude)
};

#endif // PLANESIMULATOR_HPP
