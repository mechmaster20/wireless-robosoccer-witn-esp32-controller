# Wireless Robosoccer Platform Firmware

A robust, low-latency firmware implementation designed for a high-mobility wireless robosoccer robot. The project focuses on real-time signal decoding, rapid acceleration response, and high-torque differential steering.

## Tech Stack & Hardware
- **Microcontroller:** ESP32 
- **Communication Protocol:** [e.g., Bluetooth Serial / ESP-NOW / RF 2.4GHz]
- **Language:** C/C++
- **Motor Control:** High-current H-Bridge Driver [L298N] driving 2 high-RPM DC motors.

## Technical Highlights
- **Low-Latency Input Parsing:** Implements non-blocking polling loops to decode incoming wireless character packets with zero processing overhead.
- **Differential Vector Steering:** Maps joystick/button inputs into coordinated dual-channel motor vectors for rapid 360° rotation and precise drift corrections.
- **Hardware Protection Rules:** Software-enforced dead-zones and direction-switch delays to prevent catastrophic H-bridge short circuits during rapid forward-to-reverse transitions.

## 📂 Repository Structure
- `/src`: Contains the primary firmware files for the receiver/robot controller.
- `/remote_src`: Transmitter/Remote controller code.
