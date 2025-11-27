# Project Overview

This project contains a series of exercises for learning and practicing UART (Universal Asynchronous Receiver-Transmitter) communication on an M5Stack device. The exercises guide the user through different aspects of UART, from basic communication to handling multiple serial ports.

## Directory Structure

*   `Intrucciones.txt`: Contains instructions for the exercises.
*   `UART.pdf`: Likely a supplementary document explaining UART concepts.
*   `Ejerc1/Ejerc1.ino`: A basic example of UART communication.
*   `Ejerc2_Alumnos/Ejerc2_Alumnos.ino`: A template for a UART exercise for students.
*   `Ejerc_3/Ejerc_3.ino`: A more advanced example using multiple UART ports.

# Building and Running

These are Arduino sketches and are intended to be compiled and uploaded to an M5Stack device using the Arduino IDE or a compatible tool like PlatformIO.

**To run these sketches:**

1.  **Install the Arduino IDE:** If you don't have it, download and install it from the [official Arduino website](https://www.arduino.cc/en/software).
2.  **Install M5Stack Board Support:** Follow the M5Stack instructions to add the M5Stack board to the Arduino IDE. This can usually be done through the "Boards Manager".
3.  **Install M5Stack Library:** Install the M5Stack library from the "Library Manager" in the Arduino IDE.
4.  **Open a Sketch:** Open one of the `.ino` files (e.g., `Ejerc1/Ejerc1.ino`) in the Arduino IDE.
5.  **Select Board and Port:** In the "Tools" menu, select your M5Stack board and the correct COM port.
6.  **Compile and Upload:** Click the "Upload" button to compile the sketch and upload it to your M5Stack device.
7.  **Monitor Output:** Open the "Serial Monitor" to interact with the device and see the output.

# Development Conventions

*   The code is written in C/C++ for the Arduino framework.
*   The M5Stack library is used for interacting with the device hardware (e.g., LCD screen).
*   The exercises seem to be structured to be completed in order, starting from `Ejerc1`.
