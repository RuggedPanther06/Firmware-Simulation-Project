#include <iostream> // For printing to the console (cout)
#include <fstream>  // For reading from files (ifstream)
#include <string>   // For using the string data type
#include <vector>   // For using the vector data structure
#include <sstream>
using namespace std;


const double sys_voltage = 5.0;
const int resolution = 4095;
int adc_value;
const int WINDOW_SIZE=5;
vector<int> readings;
const int CRITICAL_THRESHOLD = 1800; //Threshold for critical cold
const int WARNING_THRESHOLD = 1200;  //Threshold for cold


 

// The main function is the entry point of our program
 int main() {
    
    cout << "Phase 2: C++ Firmware Model" << std::endl;
    cout << "---------------------------" << std::endl;

 // defining filename 
 string filename = "voltage_divider.txt";
 ifstream inputFile(filename);

 if(!inputFile.is_open()) {
    cerr<<"Error: Could not open file"<<endl;
    return 1;
 }

 //parsing file
string line; 
double time,Voltage;
 while (getline(inputFile,line)){
    
    if (line.find("Step") == string::npos && line.find("time") == string::npos) {
        double sum = 0;
            
            stringstream ss(line);
            ss >> time >> Voltage;
            //calculating ADC value

            adc_value= (Voltage/sys_voltage)*resolution; 

            readings.push_back(adc_value);
            if (readings.size()>WINDOW_SIZE){
                readings.erase(readings.begin());
            }

            for(int value : readings){
                sum += value;
            }    
            //finding filtered value, trying to account for noise by taking average over multiple readings
            double filtered_value;
            if(!readings.empty()){
                filtered_value=(sum)/(readings.size());
            }

            //Status of the Thermistor System

            cout << "Filtered ADC: " << static_cast<int>(filtered_value) << " -> STATUS: ";
            if (filtered_value > CRITICAL_THRESHOLD) {
                cout << "CRITICAL COLD! System temperature is dangerously low." << endl;
            }
            else if (filtered_value > WARNING_THRESHOLD) {
                cout << "COOL. System is operating at a low temperature." << endl;
            }
            else {
                cout << "NOMINAL. System temperature is normal." << endl;
            }

    }
 }
 inputFile.close();
     cout << "\nProject complete." << endl;

 return 0;
}