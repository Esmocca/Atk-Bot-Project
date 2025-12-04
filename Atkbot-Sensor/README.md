# 🛑 ATKBOT SENSOR (Archived)

## ⚠️ Project Status: **Deprecated / No Longer Developed**
ATKBOT Sensor adalah versi lama dari sistem humanoid robot ATK yang menggunakan **sensor berbasis LDR**, tombol fisik, dan animasi servo untuk melakukan aksi dasar seperti **attack, block, heal**, dan **skill**.  
Proyek ini telah **dihentikan pengembangannya**, digantikan oleh versi Wi-Fi dan humanoid robot terbaru dengan kontrol yang lebih modern.

Repositori ini dipertahankan sebagai arsip dan referensi.

---

# 🤖 Description

**ATKBOT Sensor V2** memiliki fitur utama:
- Sistem **attack combo** dengan variasi 3 animasi
- **Skill system** lengkap dengan:
  - Skill button  
  - Skill cooldown  
  - Critical rate (%)  
  - Critical damage multiplier (crit DMG %)  
- **Blocking system** berbasis `millis()`
- **Healing system** menggunakan button
- **HP system** dengan pengurangan HP berdasarkan input LDR (light detection)
- **Servo animation engine** yang mengontrol bagian tubuh robot:
  - Punggung  
  - Tangan kanan  
  - Tangan kiri  
  - Pinggang  

---

# 📝 Program Overview

File ini berisi logika utama robot humanoid berbasis Arduino Nano menggunakan sensor fisik dan servo.  
Beberapa fitur utamanya:

### ✔️ **Character Stats**
- `hp = 30`
- `atk = 300`
- `deff = 800` (threshold LDR damage)
- `critRate = 0.3` (30% critical)
- `critDMG = 1.0` (100% skill damage)

### ✔️ **Attack System**
- 3 jenis animasi serangan  
- LED menyala saat serangan aktif  
- Animasi dihitung menggunakan waktu (non-delay)  
- Damage dihitung melalui deteksi cahaya (LDR)

### ✔️ **Skill System**
- Skill memiliki:
  - cooldown 5 detik  
  - animasi durasi 300ms  
  - kemungkinan critical  
  - IR LED untuk skill output  
- Animasi servo dinamis

### ✔️ **Defense (Block) System**
- Menonaktifkan LDR selama mode blocking  
- Animasi block 1 detik  
- Menggunakan `millis()` agar non-blocking

### ✔️ **Heal System**
- Setiap button tekan menambah HP sebesar 1

---

# 🔧 Hardware Requirements

### **Microcontroller**
- **Arduino Nano**

### **Sensors & Inputs**
- LDR (Light Dependent Resistor)  
- Button:
  - Attack  
  - Skill  
  - Heal  
  - Block  
  - Run  

### **Actuators**
- 6 Servo:
  - Punggung  
  - Bahu kanan  
  - Tangan kiri  
  - Pinggang  
  - Kaki kanan  
  - Kaki kiri  

### **Other Components**
- LED indikator serangan  
- Resistor (10kΩ & 220Ω)  
- Jumper wires  
- Breadboard / acrylic frame

---

