#include "PlaneSimulator.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <ctime>

PlaneSimulator::PlaneSimulator()
    : radiusMeters(10000.0),
      altitudeMeters(12000 * 0.3048), // feet to meters
      speedMetersPerSec(72.0),        // ~140 knots
      centerLat(40.7831),
      centerLon(-73.9712),
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

std::string PlaneSimulator::formatLatitude(double lat) {
    char hemi = lat >= 0 ? 'N' : 'S';
    lat = std::abs(lat);
    int degrees = static_cast<int>(lat);
    double minutes = (lat - degrees) * 60.0;

    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(2) << degrees
       << std::fixed << std::setprecision(4) << minutes << "," << hemi;
    return ss.str();
}

std::string PlaneSimulator::formatLongitude(double lon) {
    char hemi = lon >= 0 ? 'E' : 'W';
    lon = std::abs(lon);
    int degrees = static_cast<int>(lon);
    double minutes = (lon - degrees) * 60.0;

    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(3) << degrees
       << std::fixed << std::setprecision(4) << minutes << "," << hemi;
    return ss.str();
}

std::string PlaneSimulator::formatNMEATime() {
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    struct tm utc_tm;
    gmtime_s(&utc_tm, &tt);

    char buffer[11];
    strftime(buffer, sizeof(buffer), "%H%M%S", &utc_tm);

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream ss;
    ss << buffer << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string PlaneSimulator::formatNMEADate() {
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    struct tm utc_tm;
    gmtime_s(&utc_tm, &tt);

    char buffer[7];
    strftime(buffer, sizeof(buffer), "%d%m%y", &utc_tm);

    return std::string(buffer);
}

std::string PlaneSimulator::calculateChecksum(const std::string& sentence) {
    unsigned char checksum = 0;
    for (size_t i = 1; i < sentence.length(); ++i) {
        char c = sentence[i];
        if (c == '*') break;
        checksum ^= static_cast<unsigned char>(c);
    }
    std::ostringstream ss;
    ss << "*" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)checksum;
    return ss.str();
}

std::string PlaneSimulator::generateGPRMC(const PlanePosition& pos) {
    std::ostringstream ss;
    ss << "$GPRMC,";
    ss << formatNMEATime() << ",";
    ss << "A,";
    ss << formatLatitude(pos.latitude) << ",";
    ss << formatLongitude(pos.longitude) << ",";
    ss << std::fixed << std::setprecision(1) << pos.speed * 1.94384 << ","; // knots
    ss << std::fixed << std::setprecision(1) << pos.heading << ",";
    ss << formatNMEADate() << ",";
    ss << ",";
    ss << "A";

    std::string sentence = ss.str();
    sentence += calculateChecksum(sentence);
    return sentence;
}

std::string PlaneSimulator::generateGPGGA(const PlanePosition& pos) {
    std::ostringstream ss;
    ss << "$GPGGA,";
    ss << formatNMEATime() << ",";
    ss << formatLatitude(pos.latitude) << ",";
    ss << formatLongitude(pos.longitude) << ",";
    ss << "1,";
    ss << "08,";
    ss << "0.9,";
    ss << std::fixed << std::setprecision(1) << pos.altitude << ",M,";
    ss << "0.0,M,,";
    std::string sentence = ss.str();
    sentence += calculateChecksum(sentence);
    return sentence;
}

PlaneSimulator::NMEAMessages PlaneSimulator::getNextNMEAMessages() {
    PlanePosition pos = calculatePosition(angleRadians);

    NMEAMessages msgs;
    msgs.gprmc = generateGPRMC(pos);
    msgs.gpgga = generateGPGGA(pos);

    double angularSpeed = speedMetersPerSec / radiusMeters;
    angleRadians = std::fmod(angleRadians + angularSpeed, 2 * PI);

    return msgs;
}
