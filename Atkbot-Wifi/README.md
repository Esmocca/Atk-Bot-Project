# 📡 ATK BOT WI-FI (Deprecated)

## ⚠️ Project Status: No Longer Developed
Proyek **ATK BOT WI-FI** versi ini **sudah dihentikan pengembangannya**.  
Versi ini awalnya menggunakan **dua mikrokontroler**:
- Raspberry Pi Pico W → Wi-Fi & komunikasi server  
- Arduino Nano → Motion control humanoid robot  

Namun kini seluruh sistem telah diperbarui dan **digantikan oleh versi baru** yang:
- **Tidak lagi menggunakan Arduino Nano**
- **Menggunakan Raspberry Pi Pico W saja sebagai kontrol utama**
- Memiliki sistem gerakan + komunikasi yang digabung dalam satu file utama

---

## 📝 Project Description (Old Version)

ATK BOT WI-FI adalah sistem humanoid robot **multiplayer** yang dapat:
- Terhubung ke server melalui Wi-Fi  
- Mengirim status robot secara real-time  
- Menjalankan animasi gerakan melalui Arduino Nano  
- Memulai game apabila kedua robot terkoneksi

Versi ini **hanya arsip** dan tidak digunakan lagi.

---

## 🛠️ Tools & Parts (Old Version)

### 1. Microcontrollers
- **Raspberry Pi Pico W**  
  → Untuk Wi-Fi, client communication, status robot  
- **Arduino Nano** *(deprecated)*  
  → Kontrol servo & motion robot

### 2. I2C parts
- **PCA9685** – Driver servo  
- **OLED LCD** – UI status koneksi & mode robot  

### 3. Small Components
- LDR  
- LED Indicator  
- Buttons  
- Wires  
- Resistor 10kΩ (pull-up/down)  
- Resistor 220Ω (LED)  
- Pin header  

---

---

# 🔄 New Version

### NEW SYSTEM:
- Hanya **1** microcontroller → **Raspberry Pi Pico W**
- Semua logic Wi-Fi + controller + motion → digabung ke satu program
- Menggunakan `atk_bot.cpp` sebagai source utama
- Arduino Nano **dihapus sepenuhnya**

### Alasan Perubahan:
- Menyederhanakan wiring  
- Mengurangi latency Wi-Fi → Arduino  
- Mengurangi konsumsi daya  
- Memudahkan debugging & update firmware  
- Pico W lebih powerful untuk handle multitasking  

---

# 📁 For the New Version
Lihat file untuk update terbaru: atk-bot cpp

