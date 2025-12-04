# Project Overview

This project is a collection of Arduino sketches for the M5Stack device. The sketches are organized into directories based on the topic they cover, such as UART, timers, and FreeRTOS. The project is intended to be used as a learning resource for programming the M5Stack and understanding various hardware peripherals and software concepts.

## Directory Structure

*   `Apuntes/`: Contains notes related to the topics covered in the exercises.
*   `Ejercicios/`: Contains the Arduino sketches for the exercises.
    *   `Ejercicios UART/`: Exercises for UART communication.
    *   `Ejercicios clase_Timers/`: Exercises for using timers.
    *   `Ejercicios 5-8/Ejercicios_Clase2_FreeRTOS/`: Exercises for FreeRTOS.
*   `Examenes/`: Contains exams and related materials.
*   `Practica1/`: Contains a practical exercise.
*   `Teoria/`: Contains theory documents.

# Building and Running

These are Arduino sketches and are intended to be compiled and uploaded to an M5Stack device using the Arduino IDE or a compatible tool like PlatformIO.

**To run these sketches:**

1.  **Install the Arduino IDE:** If you don't have it, download and install it from the [official Arduino website](https://www.arduino.cc/en/software).
2.  **Install M5Stack Board Support:** Follow the M5Stack instructions to add the M5Stack board to the Arduino IDE. This can usually be done through the "Boards Manager".
3.  **Install M5Stack Library:** Install the M5Stack library from the "Library Manager" in the Arduino IDE.
4.  **Open a Sketch:** Open one of the `.ino` files (e.g., `Ejercicios/Ejercicios UART/Ejerc1/Ejerc1.ino`) in the Arduino IDE.
5.  **Select Board and Port:** In the "Tools" menu, select your M5Stack board and the correct COM port.
6.  **Compile and Upload:** Click the "Upload" button to compile the sketch and upload it to your M5Stack device.
7.  **Monitor Output:** Open the "Serial Monitor" to interact with the device and see the output.

# Development Conventions

*   The code is written in C/C++ for the Arduino framework.
*   The M5Stack library is used for interacting with the device hardware (e.g., LCD screen).
*   The exercises are structured to be completed in a sequential manner.
