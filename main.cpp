#include "PlaneSimulator.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <mqtt/client.h>  // Paho MQTT C++ client

constexpr double PI = 3.14159265358979323846;

int main(int argc, char* argv[]) {
    std::string brokerUrl = "tcp://localhost:1883";
    std::string username = "admin";
    std::string password = "admin";
    std::string baseTopic = "producers/cessna/data";
    int counter = 1; // number of planes to simulate

    if (argc >= 2) brokerUrl = argv[1];
    if (argc >= 3) username = argv[2];
    if (argc >= 4) password = argv[3];
    if (argc >= 5) baseTopic = argv[4];
    if (argc >= 6) counter = std::stoi(argv[5]);

    const std::string clientId = "plane-simulator-client";

    mqtt::client client(brokerUrl, clientId);

    mqtt::connect_options connOpts;
    connOpts.set_user_name(username);
    connOpts.set_password(password);

    try {
        std::cout << "Connecting to the MQTT broker at " << brokerUrl << "...\n";
        client.connect(connOpts);
        std::cout << "Connected.\n";

        // Create simulators for each plane
        std::vector<PlaneSimulator> simulators;
        simulators.reserve(counter);

        for (int i = 0; i < counter; ++i) {
            double angleRad = (2.0 * PI / counter) * i;
            simulators.emplace_back("plane" + std::to_string(i + 1), 10000.0, 12000 * 0.3048, 72.0, angleRad);
        }

        while (true) {
            for (int i = 0; i < counter; ++i) {
                auto& simulator = simulators[i];
                PlaneSimulator::NMEAMessages msgs = simulator.getNextNMEAMessages();

                std::string topic = baseTopic + "/plane" + std::to_string(i + 1);
                std::string id = simulator.getID();

                // Publish RMC sentence
                std::string messageRMC = "[" + id + "] " + msgs.gprmc;
                auto pubmsg1 = mqtt::make_message(topic, messageRMC);
                pubmsg1->set_qos(1);
                client.publish(pubmsg1);

                // Publish GGA sentence
                std::string messageGGA = "[" + id + "] " + msgs.gpgga;
                auto pubmsg2 = mqtt::make_message(topic, messageGGA);
                pubmsg2->set_qos(1);
                client.publish(pubmsg2);

                std::cout << "Published (" << "plane" + std::to_string(i + 1) << "):\n"
                    << messageRMC << "\n" << messageGGA << "\n\n";
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        client.disconnect();
        std::cout << "Disconnected.\n";
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "MQTT Exception: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
