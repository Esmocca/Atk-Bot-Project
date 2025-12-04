# 🤖 ATK Bot – Humanoid Project HABW

ATK Bot adalah proyek **humanoid robot** berbasis **Raspberry Pi Pico W** yang mengontrol beberapa servo menggunakan **PCA9685**, dilengkapi **OLED display** dan **LCD I2C** untuk menampilkan status serta mekanisme *action-trigger* berbasis button.

Project ini dibuat sebagai bagian dari **HABW (Humanoid ATK Bot Warrior)** — robot kecil yang mampu melakukan pose, animasi gerak, dan menampilkan informasi status secara real-time.

---

## 🚀 Fitur Utama

### 🦾 **Servo Control (7 DOF)**
Menggunakan modul **PCA9685** untuk mengontrol 7 buah servo:
- Punggung  
- Tangan kanan  
- Tangan kiri  
- Siku kanan  
- Pinggang  
- Kaki kanan  
- Kaki kiri  

### 📟 **Display System**
- **OLED kanan & kiri** → status koneksi klien + font RPG  
- **LCD 16x2 I2C** → *scoreboard* berupa bar yang berkurang ketika sinyal `"Griffith defeated!"` atau `"Stellar defeated!"` diterima

### 🔘 **Button Trigger**
- Tombol untuk menjalankan animasi servo  
- Mode aksi bisa dikembangkan (attack, defend, idle, victory, dsb.)

### 🛜 **Network Integration**
- Raspberry Pi Pico W dapat menerima sinyal dari server melalui API/WebSocket  
- Display dan servo bergerak mengikuti status server

---

## 🧰 Hardware yang Digunakan
- Raspberry Pi Pico W  
- PCA9685 Servo Driver  
- 7× SG90 / MG996R Servo  
- OLED 128x64 (I2C)  
- LCD 16x2 (I2C)  
- Push Button (aktif LOW)  
- Power module servo (5–6V)

---

## 🧪 Software & Library
- **Arduino IDE (C++)**  
- Library:
  - `Adafruit_PWMServoDriver` (PCA9685)
  - `Wire.h`
  - `LiquidCrystal_I2C`
  - Library OLED (U8g2 / Adafruit SSD1306 — sesuaikan proyek)
  - `WiFi.h` (untuk Pico W)
 
---

## 🧠 Cara Kerja Sistem
1. Robot boot → masuk mode standby  
2. OLED menampilkan status *connecting*  
3. Setelah terhubung, kedua OLED menampilkan status klien  
4. LCD menjadi scoreboard pertandingan  
5. Saat tombol ditekan → robot bergerak sesuai animasi servo  
6. Server mengirim sinyal `"Griffith defeated!"` atau `"Stellar defeated!"`  
7. Scoreboard bar berkurang → jika habis → tampilkan pemenang   

---
