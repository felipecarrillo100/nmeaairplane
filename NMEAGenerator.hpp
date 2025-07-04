#ifndef NMEA_GENERATOR_HPP
#define NMEA_GENERATOR_HPP

#include <string>
#include "PlanePosition.hpp"


namespace NMEA {

    std::string generateGPRMC(const PlanePosition& pos);
    std::string generateGPGGA(const PlanePosition& pos);

} // namespace NMEA

#endif // NMEA_GENERATOR_HPP
