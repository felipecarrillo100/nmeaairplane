# nmeaairplane 

A simple C++ simulator that generates airplane position tracks flying in a circle above Manhattan, NYC.  
It produces NMEA 0183 sentences every second with realistic position, altitude, speed, and heading data.  
Ideal for testing GPS and NMEA-based receivers.

---

## Features

- Simulates a (count) planes Cessna 172 flying a 10 km radius circular path at 12,000 feet altitude.
- Outputs valid NMEA 0183 GGA and RMC sentences every second.
- Uses realistic speed, heading, and position calculations.
- Sends generated NMEA messages via MQTT.
---

### Application Command Line  Options:

| Option         | Description                     | Default                   |
|----------------|---------------------------------|---------------------------|
| `-b, --broker` | MQTT broker URL                | `tcp://localhost:1883`    |
| `-u, --username` | MQTT username                 | `admin`                   |
| `-p, --password` | MQTT password                 | `admin`                   |
| `-t, --topic`  | Base MQTT topic                | `producers/cessna/data`   |
| `-c, --count`  | Number of planes to simulate   | `1`                       |

### Use

```bash
./nmeaairplane --broker tcp://mybroker:1883 --username user --password pass --topic producers/planes/nmea --count 5
````
---

## ⚠️ MQTT/STOMP Compatibility

Catalog Explorer communicates via the **STOMP** protocol and does **not** natively support MQTT.
To bridge MQTT messages into STOMP (so they appear in Catalog Explorer), your broker must support both protocols **and translate between them**.

### ✅ Recommended: Use ActiveMQ

If you use **ActiveMQ**, you can enable both MQTT and STOMP connectors. ActiveMQ will automatically and transparently map MQTT topics to STOMP destinations.

#### Example Mapping:
- MQTT topic: `producers/cessna/data` is mapped to
- STOMP topic: `/topic/producers.cessna.data`

This bridging lets MQTT publishers and STOMP consumers (like Catalog Explorer) work together seamlessly.

---

## Requirements to compile

- Windows 10 or later (or Linux)
- Visual Studio 2022 (with C++ development workload) or GCC/Clang on Linux
- [vcpkg](https://github.com/microsoft/vcpkg) package manager
- CMake (version 3.10 or later)

---

## Dependencies

The project uses the Eclipse Paho MQTT C++ client library, installed via vcpkg:

```bash
vcpkg install paho-mqttpp3:x64-windows  # Windows
vcpkg install paho-mqttpp3               # Linux
```

For CLI parsing we use cli11
```bash
vcpkg install cli11
```

---

## Build Instructions

1. Clone the repository:

```bash
git clone https://github.com/felipecarrillo100/nmeaairplane.git
cd nmeaairplane
```

2. Install dependencies with vcpkg (if not done already):
> **Note:** Make sure [vcpkg](https://github.com/microsoft/vcpkg#quick-start) is installed on your system before proceeding. You can install `vcpkg` in any folder of your choice, for instance `C:\cpptools\vcpkg`

```bash
# Windows
vcpkg install paho-mqttpp3:x64-windows
vcpkg install cli11


# Linux
./vcpkg install paho-mqttpp3
./vcpkg install cli11

```

3. Create and enter a build directory:

```bash
mkdir build
cd build
```

4. Configure the project with CMake, making sure to provide the path to your vcpkg toolchain file:

> **Note:** Verify where `vcpkg` is installed in your system and adjust the path accordingly

```bash
# Windows example
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
```

```bash
# Linux example
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux
```

5. Build the project:

```bash
cmake --build . --config Release
```
> **Note:** When successful, you will have now a folder called `Release` and your executable will be located inside together with any dependencies (i.e. dll files).

---

## Run Instructions (Windows & Linux)

### Windows

Run the executable with optional command line arguments:

```bash
nmeaairplane.exe [broker_url] [username] [password] [base_topic]
```

- **broker_url** (default: `tcp://localhost:1883`)
- **username** (default: `admin`)
- **password** (default: `admin`)
- **base_topic** (default: `producers/cessna/data`)

Example:

```bash
nmeaairplane.exe tcp://localhost:1883 admin admin producers/cessna/data
```

---

### Linux

1. **Install dependencies**

Make sure you have the following installed:

- C++ compiler (e.g., `g++` or `clang++`)
- CMake (version 3.10 or later)
- OpenSSL development libraries
- vcpkg

For example, on Debian/Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev
```

2. **Install vcpkg**

If you haven't installed vcpkg yet, clone and bootstrap it:

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
```

3. **Install dependencies with vcpkg**

```bash
./vcpkg install paho-mqttpp3
```

4. **Build the project**

From the project root:

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux
cmake --build . --config Release
```

5. **Run the executable**

```bash
./nmeaairplane [broker_url] [username] [password] [base_topic]
```

Example:

```bash
./nmeaairplane tcp://localhost:1883 admin admin producers/plane/nmea
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
