# 🕹️ ESP32 OLED Breakout Game (Tuğla Kırma Oyunu)

[TR] ESP32 microdenetleyici, 1.3 inç SH1106 I2C OLED ekran, analog joystick ve pasif buzzer kullanılarak geliştirilmiş retro Breakout (Tuğla Kırma) oyunu.
[EN] A retro Breakout arcade game built with ESP32 microcontroller, 1.3-inch SH1106 I2C OLED display, analog joystick, and passive buzzer.

---

## 🇹🇷 Türkçe 

### 📌 Proje Hakkında
Bu proje, ESP32 üzerinde çalışan ve 1.3 inçlik SH1106 I2C OLED ekranda görüntülenen tam fonksiyonel bir **Breakout (Tuğla Kırma)** oyunudur. Analog joystick ile raket hareket ettirilir, dahili push-button (SW) ile top fırlatılır veya oyun yeniden başlatılır. Oyun yüksek skoru ESP32'nin non-volatile belleğinde (`Preferences` kütüphanesi) saklar.

#### ✨ Öne Çıkan Özellikler
- 🎮 **Otomatik Joystick Kalibrasyonu:** Başlangıçta 10-bit ADC çözünürlüğü ile hassas eksen kalibrasyonu.
- 💾 **Kalıcı Yüksek Skor (NVS):** Yüksek skor `Preferences` kütüphanesi ile belleğe kaydedilir, cihaz kapansa da silinmez.
- 🔊 **Ses Efektleri:** Raket çarpması, tuğla kırılması, atış ve oyun bitti durumları için pasif buzzer ton efektleri.
- 🖼️ **Optimize Grafik Rendering:** `U8g2lib` sayfalama (firstPage/nextPage) yöntemi ile akıcı görsel çıktı.

---

### 🔌 Devre Bağlantıları (Pinout & Wiring)

> 💡 **Not:** Joystick modülünün sadece **VRx** (X ekseni) pini kullanılmıştır. **VRy** pini boş bırakılacaktır.

| Bileşen | ESP32 Pin | Açıklama |
| :--- | :--- | :--- |
| **SH1106 OLED SDA** | `GPIO 21` | I2C Veri Hattı |
| **SH1106 OLED SCL** | `GPIO 22` | I2C Saat Hattı |
| **Joystick VRx (X Axis)** | `GPIO 34` | Analog Giriş (ADC) |
| **Joystick SW (Button)** | `GPIO 25` | Dijital Giriş (Internal Pull-Up) |
| **Buzzer (+ Signal)** | `GPIO 26` | PWM Ses Çıkışı |
| **VCC (Modüller)** | `3.3V / 5V` | Güç Kaynağı |
| **GND (Modüller)** | `GND` | Ortak Toprak |

---

### 🛠️ Gereksinimler & Kütüphaneler
Projenin derlenebilmesi için **Arduino IDE** veya **PlatformIO** ortamında aşağıdaki kütüphanelerin kurulu olması gerekir:

1. **U8g2lib** (OLED sürücü kütüphanesi)
2. **Preferences** (ESP32 yerleşik NVS depolama kütüphanesi)
3. **Wire** & **Arduino Core for ESP32**

---

### 🚀 Kurulum ve Çalıştırma
1. Devre bağlantılarını tabloya uygun şekilde tamamlayın.
2. Arduino IDE'de **ESP32 Dev Module** kartını seçin.
3. Kodu karta yükleyin.
4. Oyun başladığında ekranda `"Calibrating Joy..."` yazısı çıkacaktır. Joystick'i serbest konumda tutun.
5. `PRESS SW TO START` uyarısını gördüğünüzde joystick butonuna basarak topu fırlatın ve oynamaya başlayın!

---

## 🇬🇧 English

### 📌 About The Project
This project is a fully functional **Breakout arcade game** developed for the ESP32 microcontroller, rendered on a 1.3-inch SH1106 I2C OLED display. The player controls the paddle using an analog joystick and launches the ball or restarts the game via the integrated push-button (SW). The game saves the high score persistently using ESP32's Non-Volatile Storage (`Preferences` library).

#### ✨ Features
- 🎮 **Automatic Joystick Calibration:** Precise center-point auto-calibration on startup using 10-bit ADC resolution.
- 💾 **Persistent High Score:** Saves the record score into NVS memory so it survives power cycles.
- 🔊 **Sound Effects:** Tone feedback via passive buzzer for paddle hit, brick destruction, launch, game over, and victory.
- 🖼️ **Optimized Display Rendering:** Smooth performance driven by `U8g2lib` page buffering.

---

### 🔌 Wiring & Pinout

> 💡 **Note:** Only the **VRx** (X-axis) pin of the joystick module is used. The **VRy** pin remains unconnected.

| Component | ESP32 Pin | Description |
| :--- | :--- | :--- |
| **SH1106 OLED SDA** | `GPIO 21` | I2C Data Line |
| **SH1106 OLED SCL** | `GPIO 22` | I2C Clock Line |
| **Joystick VRx (X Axis)** | `GPIO 34` | Analog Input (ADC) |
| **Joystick SW (Button)** | `GPIO 25` | Digital Input (Internal Pull-Up) |
| **Buzzer (+ Signal)** | `GPIO 26` | PWM Sound Output |
| **VCC (Modules)** | `3.3V / 5V` | Power Supply |
| **GND (Modules)** | `GND` | Common Ground |

---

### 🛠️ Requirements & Libraries
Make sure the following dependencies are installed in your **Arduino IDE** or **PlatformIO** environment:

1. **U8g2lib** (OLED display driver)
2. **Preferences** (ESP32 built-in NVS library)
3. **Wire** & **Arduino Core for ESP32**

---

### 🚀 Setup & Usage
1. Connect the hardware components according to the pinout table.
2. Select **ESP32 Dev Module** in your Arduino IDE.
3. Upload the code to your ESP32 board.
4. Keep the joystick centered while `"Calibrating Joy..."` appears on the screen.
5. Press the joystick push-button when `PRESS SW TO START` appears to launch the ball and play!

---

## 📩 İletişim/Contact

Proje hakkında sorularınız, geri bildirimleriniz veya iş birliği önerileriniz için bana ulaşabilirsiniz.

Feel free to reach out for any questions, feedback, or collaboration opportunities.

* **Instagram:** [@TechByAysenur](https://www.instagram.com/techbyaysenur/) — DIY teknoloji ve robotik içeriklerim/ DIY tech & robotics content
* **LinkedIn:** [Ayşenur Karaküçük](www.linkedin.com/in/ayşenur-karaküçük-918ba2248) — Profesyonel ağım/ Professional network
* **GitHub:** [aysenurkarakucuk](https://github.com/AysenurKrkck) — Diğer projelerim/ My other projects

---

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).

---
