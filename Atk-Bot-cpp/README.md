# ATK BOT C++

## Description
ATK BOT C++ adalah versi terbaru dari proyek robot humanoid multiplayer yang sepenuhnya menggunakan **Raspberry Pi Pico W** sebagai pusat kontrol.  
Seluruh sistem — mulai dari komunikasi Wi-Fi, pengendalian servo, hingga mekanik robot — kini ditulis menggunakan **C++** tanpa Arduino.

Proyek ini merupakan lanjutan dari versi lama **ATK BOT WI-FI**, yang kini sudah tidak dikembangkan lagi.

---

## Features
- 🔗 **Full Wi-Fi Communication** menggunakan Raspberry Pi Pico W  
- 🤖 **Control up to 7 servos** menggunakan PCA9685 (back, arms, waist, legs)  
- 🎮 **Multiplayer-ready** dengan sistem server & client  
- ⚙️ Ditulis sepenuhnya dalam **C++ for Pico SDK**  
- 📟 Dukungan tampilan **OLED** dan **LCD 16x2 I2C**  
- 🟦 Desain modular & mudah dikembangkan  

---

## Hardware Used
### Microcontroller
- **Raspberry Pi Pico W** (main controller + Wi-Fi)

### I2C Modules
- **PCA9685** — pengontrol servo  
- **OLED Display**  
- **LCD 16x2 I2C** (opsional: scoreboard, status bar)

### Additional Components
- Buttons  
- LEDs  
- Wires & headers  
- Resistor 10k / 220Ω  

---

---

## Status
🟩 **Active Development**

Versi ini adalah versi utama yang akan terus dikembangkan. Semua update robot, komunikasi, animasi gerakan, dan fitur baru akan difokuskan di repository ini.

---

## Notes
Jika sebelumnya menggunakan **ATK BOT WI-FI**, disarankan untuk beralih ke repository ini karena:
- Lebih stabil  
- Performa lebih cepat  
- Arsitektur lebih sederhana (tanpa Arduino Nano)  

---

Jika ingin ditambahkan contoh cara compile dengan Pico SDK, wiring diagram, atau flowchart robot—tinggal bilang saja!
