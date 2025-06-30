#include "PlaneSimulator.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Include Paho MQTT C++ headers
#include <mqtt/client.h>

int main(int argc, char* argv[]) {
    std::string brokerUrl = "tcp://localhost:1883";
    std::string username = "admin";
    std::string password = "admin";
    std::string baseTopic = "producers/cessna/data";

    if (argc >= 2) brokerUrl = argv[1];
    if (argc >= 3) username = argv[2];
    if (argc >= 4) password = argv[3];
    if (argc >= 5) baseTopic = argv[4];

    const std::string clientId = "plane-simulator-client";

    mqtt::client client(brokerUrl, clientId);

    mqtt::connect_options connOpts;
    connOpts.set_user_name(username);
    connOpts.set_password(password);

    try {
        std::cout << "Connecting to the MQTT broker at " << brokerUrl << "...\n";
        client.connect(connOpts);
        std::cout << "Connected.\n";

        PlaneSimulator simulator;

        while (true) {
            PlaneSimulator::NMEAMessages msgs = simulator.getNextNMEAMessages();

            // Publish GPRMC sentence
            mqtt::message_ptr pubmsg1 = mqtt::make_message(baseTopic, msgs.gprmc);
            pubmsg1->set_qos(1);
            client.publish(pubmsg1);

            // Publish GPGGA sentence
            mqtt::message_ptr pubmsg2 = mqtt::make_message(baseTopic, msgs.gpgga);
            pubmsg2->set_qos(1);
            client.publish(pubmsg2);

            std::cout << "Published:\n" << msgs.gprmc << "\n" << msgs.gpgga << "\n\n";

            std::this_thread::sleep_for(std::chrono::milliseconds (1000));
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
