# PlaneSimulator

A simple C++ simulator that generates airplane position tracks flying in a circle above Manhattan, NYC.  
It produces NMEA 0183 sentences every second with realistic position, altitude, speed, and heading data.  
Ideal for testing GPS and NMEA-based systems.

---

## Features

- Simulates a Cessna 172 flying a 5 km radius circular path at 12,000 feet altitude.
- Outputs valid NMEA 0183 GGA and RMC sentences every second.
- Uses realistic speed, heading, and position calculations.
- Sends generated NMEA messages via MQTT.

---

## Requirements

- Windows 10 or later
- Visual Studio 2022 (with C++ development workload)
- [vcpkg](https://github.com/microsoft/vcpkg) package manager
- CMake (version 3.10 or later)
- MQTT broker (e.g., Mosquitto) running locally or remotely

---

## Dependencies

The project uses the Eclipse Paho MQTT C++ client library, installed via vcpkg:

```bash
vcpkg install paho-mqttpp3:x64-windows
```

---

## Build Instructions

1. Clone the repository:

```bash
git clone https://github.com/yourusername/PlaneSimulator.git
cd PlaneSimulator
```

2. Install dependencies with vcpkg (if not done already):

```bash
vcpkg install paho-mqttpp3:x64-windows
```

3. Create and enter a build directory:

```bash
mkdir build
cd build
```

4. Configure the project with CMake, making sure to provide the path to your vcpkg toolchain file:

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
```

5. Build the project:

```bash
cmake --build . --config Release
```

---

## Run Instructions

Run the executable with optional command line arguments:

```bash
mqtt_hello.exe [broker_url] [username] [password] [base_topic]
```

- **broker_url** (default: `tcp://localhost:1883`)
- **username** (default: `admin`)
- **password** (default: `admin`)
- **base_topic** (default: `producers/hello/data`)

Example:

```bash
mqtt_hello.exe tcp://localhost:1883 admin admin producers/plane/nmea
```

---

## Project Structure

- `main.cpp` - Program entry point, handles MQTT client connection and runs the simulation loop.
- `PlaneSimulator.hpp` / `PlaneSimulator.cpp` - The simulator module generating plane positions and NMEA messages.
- `CMakeLists.txt` - CMake build configuration.

---

## Notes

- The simulation runs indefinitely, publishing one NMEA message per second.
- The plane flies in a perfect circle using flat-earth assumption (valid for 5 km radius).
- Altitude is fixed at 12,000 feet (converted to meters in output).
- MQTT broker must be accessible for messages to be published.

---

## License

[MIT License](LICENSE)

---

## Contact

For questions or feedback, please open an issue or contact [your-email@example.com].
