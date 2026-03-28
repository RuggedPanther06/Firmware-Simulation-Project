#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

enum class SystemState {
    NOMINAL,
    COOL,
    CRITICAL_COLD
};

class TelemetryProcessor {
private:
    static constexpr double SYS_VOLTAGE = 5.0;
    static constexpr int RESOLUTION = 4095;
    static constexpr int WINDOW_SIZE = 5;
    static constexpr int CRITICAL_THRESHOLD = 1800;
    static constexpr int WARNING_THRESHOLD = 1200;

    std::vector<int> readings;
    int current_index;
    int current_size;
    double running_sum;
    SystemState current_state;

    void updateState(double filtered_value) {
        if (filtered_value > CRITICAL_THRESHOLD) {
            current_state = SystemState::CRITICAL_COLD;
            std::cout << "Filtered ADC: " << static_cast<int>(filtered_value) 
                      << " -> STATUS: CRITICAL COLD! System temperature is dangerously low.\n";
        } else if (filtered_value > WARNING_THRESHOLD) {
            current_state = SystemState::COOL;
            std::cout << "Filtered ADC: " << static_cast<int>(filtered_value) 
                      << " -> STATUS: COOL. System is operating at a low temperature.\n";
        } else {
            current_state = SystemState::NOMINAL;
            std::cout << "Filtered ADC: " << static_cast<int>(filtered_value) 
                      << " -> STATUS: NOMINAL. System temperature is normal.\n";
        }
    }

public:
    TelemetryProcessor() 
        : readings(WINDOW_SIZE, 0), current_index(0), current_size(0), 
          running_sum(0.0), current_state(SystemState::NOMINAL) {}

    void processReading(double voltage) {
        int adc_value = static_cast<int>((voltage / SYS_VOLTAGE) * RESOLUTION);

        if (current_size == WINDOW_SIZE) {
            running_sum -= readings[current_index];
        } else {
            current_size++;
        }

        readings[current_index] = adc_value;
        running_sum += adc_value;
        current_index = (current_index + 1) % WINDOW_SIZE;

        double filtered_value = running_sum / current_size;
        updateState(filtered_value);
    }
};

int main() {
    std::cout << "Phase 2: C++ Firmware Model\n";
    std::cout << "---------------------------\n";

    std::ifstream inputFile("voltage_divider.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file\n";
        return 1;
    }

    TelemetryProcessor processor;
    std::string line;
    double time, voltage;

    while (std::getline(inputFile, line)) {
        if (line.find("Step") == std::string::npos && 
            line.find("time") == std::string::npos && 
            !line.empty()) {
            
            std::stringstream ss(line);
            if (ss >> time >> voltage) {
                processor.processReading(voltage);
            }
        }
    }

    inputFile.close();
    std::cout << "\nProject complete.\n";

    return 0;
}