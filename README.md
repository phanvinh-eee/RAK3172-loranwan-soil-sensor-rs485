# RAK3172-loranwan-soil-sensor-rs485
This project develops a low-power IoT soil monitoring node based on the RAK3172 LoRaWAN module and STM32 microcontroller.

## 📘 Project Overview

This project develops a **low-power IoT soil monitoring node** based on the **RAK3172 LoRaWAN module** and **STM32 microcontroller**.  
The device collects soil and environmental data — including **pH**, **NPK (Nitrogen–Phosphorus–Potassium)**, and **EC (Electrical Conductivity)** — via **RS485 Modbus sensors**, then transmits the readings over **LoRaWAN** to a central gateway for remote monitoring and irrigation management.

Designed for **smart agriculture**, the node is battery-powered and optimized for **ultra-low power operation**, suitable for farms and remote areas without stable network connectivity.

---

## ⚙️ Hardware Components

| Component | Description |
|------------|-------------|
| **MCU** | STM32 (ARM Cortex-M4), firmware generated with Keil C and HAL drivers |
| **LoRaWAN Module** | RAK3172 (Semtech SX1262), compliant with AS923-2 |
| **Sensors (RS485 Modbus)** | - Address 1: pH sensor  <br> - Address 2: NPK sensor  <br> - Address 3: EC, temperature, humidity sensor |
| **Communication Bus** | RS485 (Modbus RTU protocol) |
| **Power Supply** | Li-ion / LiFePO₄ rechargeable battery, optional solar charging |
| **Enclosure** | IP65 waterproof housing for outdoor use |

---

## 🔋 Operating Principle

1. MCU wakes up from low-power sleep at defined intervals.  
2. Sequentially reads data from RS485 Modbus sensors: pH, NPK, temperature, humidity, and EC.  
3. Packages sensor values into a LoRaWAN payload.  
4. Transmits data uplink to LoRaWAN gateway.  
5. Returns to deep sleep mode to conserve power.  

---

## 🛰️ System Features

- Long-range wireless data transmission via **LoRaWAN**
- **Low power consumption** with deep sleep between measurements
- Modular RS485 interface for multi-sensor expansion
- Configurable **uplink interval** and LoRaWAN parameters
- Designed for **outdoor agricultural environments**
- Optional **solar charging** system for off-grid deployment

---

## 📡 Applications

- Smart irrigation and fertilization control  
- Precision agriculture and soil analysis  
- Remote farm environmental monitoring  
- Research and crop data logging systems  

---

## 🧩 Firmware Overview

- Developed with **Keil C + HAL drivers**
- **Modbus RTU** communication over UART (RS485 transceiver)
- **LoRaWAN stack** based on RAK3172 STM32CubeMX
- Periodic wake-up using RTC timer interrupt
- Configurable payload format and uplink frequency

---

## 📁 Directory Structure

