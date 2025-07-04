#include "NMEAGenerator.hpp"
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cmath>
#include <ctime>

namespace {

    constexpr double PI = 3.14159265358979323846;

    std::string formatLatitude(double lat) {
        char hemi = lat >= 0 ? 'N' : 'S';
        lat = std::abs(lat);
        int degrees = static_cast<int>(lat);
        double minutes = (lat - degrees) * 60.0;

        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(2) << degrees
           << std::fixed << std::setprecision(4) << std::setw(7) << minutes
           << "," << hemi;
        return ss.str();
    }

    std::string formatLongitude(double lon) {
        char hemi = lon >= 0 ? 'E' : 'W';
        lon = std::abs(lon);
        int degrees = static_cast<int>(lon);
        double minutes = (lon - degrees) * 60.0;

        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(3) << degrees
           << std::fixed << std::setprecision(4) << std::setw(7) << minutes
           << "," << hemi;
        return ss.str();
    }

    std::string formatNMEATime() {
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

    std::string formatNMEADate() {
        auto now = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        struct tm utc_tm;
        gmtime_s(&utc_tm, &tt);

        char buffer[7];
        strftime(buffer, sizeof(buffer), "%d%m%y", &utc_tm);
        return std::string(buffer);
    }

    std::string calculateChecksum(const std::string& sentence) {
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

} // anonymous namespace

namespace NMEA {

    std::string generateGPRMC(const PlanePosition& pos) {
        std::ostringstream ss;
        ss << "$GPRMC,";
        ss << formatNMEATime() << ",";
        ss << "A,";
        ss << formatLatitude(pos.latitude) << ",";
        ss << formatLongitude(pos.longitude) << ",";
        ss << std::fixed << std::setprecision(1) << pos.speed * 1.94384 << ","; // to knots
        ss << std::fixed << std::setprecision(1) << pos.heading << ",";
        ss << formatNMEADate() << ",";
        ss << ",";
        ss << "A";

        std::string sentence = ss.str();
        sentence += calculateChecksum(sentence);
        return sentence;
    }

    std::string generateGPGGA(const PlanePosition& pos) {
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

} // namespace NMEA
