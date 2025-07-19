#include "PlaneSimulator.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <mqtt/client.h>  // Paho MQTT C++ client
#include <CLI/CLI.hpp>    // Include CLI11 header

constexpr double PI = 3.14159265358979323846;

int main(int argc, char* argv[]) {
    std::string brokerUrl = "tcp://localhost:1883";
    std::string username = "admin";
    std::string password = "admin";
    std::string baseTopic = "producers/cessna/data";
    int counter = 1; // number of planes to simulate

    CLI::App app{"NMEA Airplane Simulator"};

    app.add_option("-b,--broker", brokerUrl, "MQTT broker URL")->default_val(brokerUrl);
    app.add_option("-u,--username", username, "MQTT username")->default_val(username);
    app.add_option("-p,--password", password, "MQTT password")->default_val(password);
    app.add_option("-t,--topic", baseTopic, "Base MQTT topic")->default_val(baseTopic);
    app.add_option("-c,--count", counter, "Number of planes to simulate")->default_val("1");

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    const std::string clientId = "plane-simulator-client";

    mqtt::client client(brokerUrl, clientId);

    mqtt::connect_options connOpts;
    connOpts.set_user_name(username);
    connOpts.set_password(password);

    try {
        std::cout << "Connecting to the MQTT broker at " << brokerUrl << "...\n";
        client.connect(connOpts);
        std::cout << "Connected.\n";

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

                // Conditionally prepend id only if more than one plane
                std::string prefixId = (counter == 1) ? "" : "[" + id + "] ";
                std::string messageRMC = prefixId + msgs.gprmc;
                auto pubmsg1 = mqtt::make_message(topic, messageRMC);
                pubmsg1->set_qos(1);
                client.publish(pubmsg1);

                std::string messageGGA = prefixId + msgs.gpgga;
                auto pubmsg2 = mqtt::make_message(topic, messageGGA);
                pubmsg2->set_qos(1);
                client.publish(pubmsg2);

                std::cout << "Published (" << id << "):\n" << messageRMC << "\n" << messageGGA << "\n\n";
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
