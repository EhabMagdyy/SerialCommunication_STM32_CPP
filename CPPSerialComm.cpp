/*****************************************************************************************************************************
**********************************    Author  : Ehab Magdy Abdullah                      *************************************
**********************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  *************************************
**********************************    Youtube : https://www.youtube.com/@EhabMagdyy      *************************************
******************************************************************************************************************************/

#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Configure the serial port settings
bool configureSerialPort(HANDLE hSerial, DCB& dcbSerialParams)
{
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        std::cerr << "Error getting serial port state" << std::endl;
        return false;
    }

    dcbSerialParams.BaudRate = CBR_115200; // Set baud rate
    dcbSerialParams.ByteSize = 8;        // Data bit size
    dcbSerialParams.StopBits = ONESTOPBIT; // Stop bit size
    dcbSerialParams.Parity = NOPARITY;   // No parity

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error setting serial port state" << std::endl;
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error setting serial port timeouts" << std::endl;
        return false;
    }

    return true;
}

int main()
{
    HANDLE hSerial;
    std::string portName = "COM4";      // Change this to your serial port name
    hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(hSerial == INVALID_HANDLE_VALUE) 
    {
        std::cerr << "Error opening serial port" << std::endl;
        return 1;
    }

    // Configure Serial port settings
    DCB dcbSerialParams = {0};
    if(!configureSerialPort(hSerial, dcbSerialParams)) 
    {
        CloseHandle(hSerial);
        return 1;
    }

    std::cout << "Communication Starts." << std::endl;

    unsigned char counter = 0;

    /* Sending & Receiving data with STM32 - 3 times then end communication */
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Sending data
        std::string dataToSend = "Hello STM32, How are you?";
        DWORD bytesWritten;
        if(!WriteFile(hSerial, dataToSend.c_str(), dataToSend.size(), &bytesWritten, NULL))
        {
            std::cerr << "Error writing to serial port" << std::endl;
        } 
        else
        {
            std::cout << "Sent: " << dataToSend << std::endl;
        }

        // Receiving data [in this example we receive 2 times in a row]
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            char buffer[256];
            DWORD bytesRead;
            if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL))
            {
                std::cerr << "Error reading from serial port" << std::endl;
                break;
            } 
            else
            {
                buffer[bytesRead] = '\0';
                // checks if we received 'e' ->  ends receving from STM32
                if(bytesRead == 1 && buffer[0] == 'e'){
                    std::cout << "Receving ends" << std::endl;
                    break;
                }
                // print received data
                else
                    std::cout << "Received: " << buffer << std::endl;
            }
        }
        // ends communication after three loops
        if(counter++ >= 2)
        {
            std::cout << "Communication Ends." << std::endl;
            break;
        }
    }

    // close serial
    CloseHandle(hSerial);

    return 0;
}