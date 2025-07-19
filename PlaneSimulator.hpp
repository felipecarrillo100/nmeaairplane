#ifndef PLANESIMULATOR_HPP
#define PLANESIMULATOR_HPP

#include <string>
#include "PlanePosition.hpp"

class PlaneSimulator {
public:
    struct NMEAMessages {
        std::string gprmc;
        std::string gpgga;
    };

    PlaneSimulator(
        const std::string& id = "plane1",
        double radiusMeters = 10000.0,
        double altitudeMeters = 12000 * 0.3048,
        double speedMetersPerSec = 72.0,
        double angleRadians = 0.0,
        double centerLat = 40.7831,
        double centerLon = -73.9712
    );

    NMEAMessages getNextNMEAMessages();
    const std::string& getID() const;

private:
    struct PlanePosition calculatePosition(double angleRad);

    std::string id;
    double radiusMeters;
    double altitudeMeters;
    double speedMetersPerSec;
    double angleRadians;
    double centerLat;
    double centerLon;
    double latDegreeMeters;
    double lonDegreeMeters;
};

#endif // PLANESIMULATOR_HPP
