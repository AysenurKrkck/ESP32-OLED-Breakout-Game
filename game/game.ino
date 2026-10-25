#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Preferences.h>

// TR: 1.3 inç OLED Sürücüsü (SH1106 I2C)
// EN: 1.3-inch OLED Driver (SH1106 I2C)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// TR: Pin Tanımlamaları
// EN: Pin Definitions
#define JOY_X_PIN   34  // TR: Analog Giriş (GPIO 34) | EN: Analog Input (GPIO 34)
#define JOY_SW_PIN  25  // TR: Dijital Giriş (Buton) | EN: Digital Input (Button)
#define BUZZER_PIN  26  // TR: PWM Ses Çıkışı | EN: PWM Audio Output

// TR: Joystick Yönü Tersse 'true' Yap
// EN: Set to 'true' if Joystick direction is reversed
const bool REVERSE_X = false; 

// TR: ESP32 Kalıcı Bellek (NVS) Nesnesi
// EN: ESP32 Non-Volatile Storage (NVS) Object
Preferences preferences;

// TR: Kalibrasyon ve Ölü Bölge Değişkenleri (10-bit: 0 - 1023)
// EN: Calibration and Deadzone Variables (10-bit: 0 - 1023)
int joyCenter = 512;
const int DEADZONE = 60; 

// TR: Tuğla Düzeni ve Boyut Parametreleri
// EN: Brick Layout and Dimension Parameters
const int BRICK_ROWS = 3;
const int BRICK_COLS = 8;
const int BRICK_WIDTH = 13;
const int BRICK_HEIGHT = 4;
const int BRICK_GAP = 2;
const int BRICK_OFFSET_X = 5;
const int BRICK_OFFSET_Y = 12;

// TR: Tuğla Durum Matrisi (true: var, false: kırıldı)
// EN: Brick State Matrix (true: active, false: destroyed)
bool bricks[BRICK_ROWS][BRICK_COLS];

// TR: Raket Boyut ve Pozisyon Ayarları
// EN: Paddle Size and Position Settings
int paddleW = 22;
const int paddleH = 3;
const int paddleY = 59;
float paddleX = 53;

// TR: Top Fiziği ve Hız Ayarları
// EN: Ball Physics and Speed Settings
float ballX, ballY;
float ballSpeedX, ballSpeedY;
const float BASE_SPEED = 2.2;
bool ballAttached = true; // TR: Top rakete yapışık mı? | EN: Is ball attached to paddle?

// TR: Oyun Durum Değişkenleri
// EN: Game State Variables
int lives = 3;
int score = 0;
int highScore = 0;
bool gameOver = false;
bool gameWon = false;

// TR: Pasif Buzzer Ton Çalma Fonksiyonu
// EN: Safe Passive Buzzer Tone Player Function
void playToneSafe(int freq, int dur) {
  tone(BUZZER_PIN, freq, dur);
}

// TR: Yüksek Skoru Hafızadan Yükleme
// EN: Load High Score from NVS Memory
void loadHighScore() {
  preferences.begin("breakout", true);
  highScore = preferences.getInt("highscore", 0);
  preferences.end();
}

// TR: Yüksek Skoru Hafızaya Kaydetme
// EN: Save High Score to NVS Memory
void saveHighScore(int s) {
  preferences.begin("breakout", false);
  preferences.putInt("highscore", s);
  preferences.end();
}

// TR: Bütün Tuğlaları Sıfırlayıp Yeniden Oluşturma
// EN: Reset and Re-enable All Bricks
void resetBricks() {
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      bricks[r][c] = true;
    }
  }
}

// TR: Topu Raket Üzerine Hizalayarak Sıfırlama
// EN: Reset Ball Position onto Paddle
void resetBall() {
  ballAttached = true;
  ballX = paddleX + (paddleW / 2);
  ballY = paddleY - 3;
  ballSpeedX = 0;
  ballSpeedY = -BASE_SPEED;
}

// TR: Yeni Oyun Başlatma (Skor, Can ve Oyun Alanı Sıfırlama)
// EN: Start New Game (Reset Score, Lives, and Field)
void startNewGame() {
  lives = 3;
  score = 0;
  paddleX = (128 - paddleW) / 2;
  resetBricks();
  resetBall();
  gameOver = false;
  gameWon = false;
}

// TR: Joystick Merkez Noktası Otomatik Kalibrasyonu
// EN: Automatic Joystick Center Point Calibration
void calibrateJoystick() {
  long sum = 0;
  for (int i = 0; i < 40; i++) {
    sum += analogRead(JOY_X_PIN);
    delay(10);
  }
  joyCenter = sum / 40; // TR: 10-bit ADC ile ~512 merkeze hizalar | EN: Calibrates ~512 center with 10-bit ADC
}

void setup() {
  // TR: Pin Modları ve İç Direnç Ayarları
  // EN: Pin Modes and Internal Pull-Up Configuration
  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // TR: ESP32 ADC Çözünürlüğünü 10-bit (0-1023) Moduna Ayarlama
  // EN: Set ESP32 ADC Resolution to 10-bit (0-1023) Mode
  analogReadResolution(10);

  // TR: I2C İletişimi ve OLED Ekran Başlatma
  // EN: Initialize I2C Communication and OLED Display
  Wire.begin(21, 22);
  u8g2.begin();

  // TR: Kalibrasyon Ekranını Çizdirme
  // EN: Render Calibration Screen
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(20, 32, "Calibrating Joy...");
  } while (u8g2.nextPage());

  calibrateJoystick();
  loadHighScore();
  startNewGame();
}

// TR: Oyun Fiziği ve Çarpışma Hesaplamaları Güncelleme
// EN: Update Game Physics and Collision Calculations
void updatePhysics() {
  // 1. TR: Stabilize Edilmiş Joystick Okuması | EN: Stabilized Joystick Reading
  int joyVal = analogRead(JOY_X_PIN);
  int diff = joyVal - joyCenter;

  if (abs(diff) > DEADZONE) {
    float moveSpeed = 0;
    if (diff > 0) {
      moveSpeed = (float)diff / (1023 - joyCenter) * 3.0;
    } else {
      moveSpeed = (float)diff / joyCenter * 3.0;
    }

    if (REVERSE_X) {
      paddleX -= moveSpeed;
    } else {
      paddleX += moveSpeed;
    }
  }

  // TR: Raket Ekran Sınır Kontrolleri
  // EN: Paddle Screen Edge Constraints
  if (paddleX < 1) paddleX = 1;
  if (paddleX + paddleW > 127) paddleX = 127 - paddleW;

  // 2. TR: Top Raketteyken Buton Fırlatma Kontrolü
  // EN: Launch Ball Control via Joystick Button
  if (ballAttached) {
    ballX = paddleX + (paddleW / 2);
    ballY = paddleY - 3;

    if (digitalRead(JOY_SW_PIN) == LOW) {
      ballAttached = false;
      ballSpeedX = random(-15, 16) / 10.0;
      ballSpeedY = -BASE_SPEED;
      playToneSafe(1200, 30);
      delay(120);
    }
    return;
  }

  // 3. TR: Top Hareketi ve Çarpmalar | EN: Ball Movement & Bounce Physics
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // TR: Yan Duvar Çarpışmaları | EN: Side Wall Collisions
  if (ballX <= 1) {
    ballX = 1;
    ballSpeedX = -ballSpeedX;
    playToneSafe(600, 15);
  } else if (ballX >= 126) {
    ballX = 126;
    ballSpeedX = -ballSpeedX;
    playToneSafe(600, 15);
  }

  // TR: Tavan Çarpışması | EN: Top Ceiling Collision
  if (ballY <= 10) {
    ballY = 10;
    ballSpeedY = -ballSpeedY;
    playToneSafe(600, 15);
  }

  // TR: Raket Çarpışma Kontrolü ve Açılı Yansıma
  // EN: Paddle Collision Detection and Angle Bounce
  if (ballY >= paddleY - 2 && ballY <= paddleY + paddleH) {
    if (ballX >= paddleX - 1 && ballX <= paddleX + paddleW + 1) {
      float hitOffset = (ballX - (paddleX + paddleW / 2.0)) / (paddleW / 2.0);
      ballSpeedX = hitOffset * 2.6;
      ballSpeedY = -abs(ballSpeedY);
      ballY = paddleY - 3;
      playToneSafe(950, 20);
    }
  }

  // TR: Tuğla Çarpışma Kontrolü ve Skor Artışı
  // EN: Brick Collision Detection and Score Handling
  bool allCleared = true;
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      if (bricks[r][c]) {
        allCleared = false;
        int bx = BRICK_OFFSET_X + c * (BRICK_WIDTH + BRICK_GAP);
        int by = BRICK_OFFSET_Y + r * (BRICK_HEIGHT + BRICK_GAP);

        if (ballX >= bx && ballX <= bx + BRICK_WIDTH &&
            ballY >= by && ballY <= by + BRICK_HEIGHT) {
          bricks[r][c] = false;
          ballSpeedY = -ballSpeedY;
          score += (3 - r) * 10;
          playToneSafe(1300 + (r * 250), 20);

          if (score > highScore) {
            highScore = score;
            saveHighScore(highScore);
          }
          break;
        }
      }
    }
  }

  // TR: Tüm Tuğlalar Temizlendi mi? (Zafer Durumu)
  // EN: Are All Bricks Cleared? (Win Condition)
  if (allCleared) {
    gameWon = true;
    playToneSafe(2000, 250);
  }

  // TR: Can Kaybı ve Oyun Bitti Kontrolü
  // EN: Life Loss and Game Over Check
  if (ballY > 64) {
    lives--;
    playToneSafe(250, 120);
    if (lives <= 0) {
      gameOver = true;
    } else {
      resetBall();
    }
  }
}

// TR: Ekran Grafiklerini Çizdirme Fonksiyonu
// EN: Render Game Graphics on Display
void drawGame() {
  u8g2.setDrawColor(1);

  // TR: Üst Panel (Skor, Rekor, Can Göstergeleri)
  // EN: Top Panel (Score, High Score, Lives Indicators)
  u8g2.drawHLine(0, 9, 128);
  u8g2.setFont(u8g2_font_5x7_tr);
  
  u8g2.setCursor(2, 7);
  u8g2.print("SCORE:");
  u8g2.print(score);

  u8g2.setCursor(62, 7);
  u8g2.print("HI:");
  u8g2.print(highScore);

  u8g2.setCursor(102, 7);
  u8g2.print("C:");
  for (int i = 0; i < lives; i++) {
    u8g2.drawBox(114 + (i * 4), 2, 2, 5);
  }

  // TR: Tuğlaları Ekran Üzerine Çizdirme
  // EN: Draw Bricks on Screen
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      if (bricks[r][c]) {
        int bx = BRICK_OFFSET_X + c * (BRICK_WIDTH + BRICK_GAP);
        int by = BRICK_OFFSET_Y + r * (BRICK_HEIGHT + BRICK_GAP);
        u8g2.drawFrame(bx, by, BRICK_WIDTH, BRICK_HEIGHT);
      }
    }
  }

  // TR: Raket ve Topu Çizdirme
  // EN: Render Paddle and Ball
  u8g2.drawRBox((int)paddleX, paddleY, paddleW, paddleH, 1);
  u8g2.drawBox((int)ballX, (int)ballY, 2, 2);

  // TR: Başlangıç/Oyun Sonu Metinleri
  // EN: Start / Game Over Text Overlays
  if (ballAttached && !gameOver && !gameWon) {
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(32, 45, "PRESS SW TO START");
  }

  if (gameOver) {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(30, 36, "GAME OVER");
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(24, 48, "Press SW to replay");
  } else if (gameWon) {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(34, 36, "YOU WIN!");
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(24, 48, "Press SW to replay");
  }
}

void loop() {
  // TR: Oyun Bitti veya Kazanıldı Durumunda Yeniden Başlatma Kontrolü
  // EN: Check Replay Input on Game Over or Game Won State
  if (gameOver || gameWon) {
    if (digitalRead(JOY_SW_PIN) == LOW) {
      startNewGame();
    }
  } else {
    updatePhysics();
  }

  // TR: U8g2 Sayfalama (Page Buffering) Döngüsü
  // EN: U8g2 Page Buffering Loop
  u8g2.firstPage();
  do {
    drawGame();
  } while (u8g2.nextPage());
}
