// Hab-w weapon stance position
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PCA9585
#define SERVO_MIN 150  // Posisi minimum servo
#define SERVO_MAX 600  // Posisi maksimum servo
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);


// Definisi kanal untuk masing-masing servo
#define SERVO_PUNGGUNG 0
#define SERVO_BAHUKANAN 1
#define SERVO_TANGANKIRI 2
#define SERVO_SIKUKANAN 3
#define SERVO_PINGGANG 4
#define SERVO_KAKIKANAN 5
#define SERVO_KAKIKIRI 6
#define SERVO_LUTUTKANAN 7
#define SERVO_LUTUTKIRI 8
#define SERVO_TELAPAKKANAN 9
#define SERVO_TELAPAKKIRI 10
  
// Konfigurasi Wi-Fi
const char* ssid = "Alamak";
const char* password = "ndaktaukoktanyasaya";

// Konfigurasi Server
const char* serverIP = "10.233.";
const int serverPort = 50003;

// Konfigurasi Klien
const String clientName = "Esmocca"; // Ganti untuk client kedua

// Status robot
int hp = 400;
int atk = 30;
int defense = 30; // Defense dalam persen (misalnya 20 berarti mengurangi damage sebesar 20%)
int strength = 10;   // Strength dalam persen (misalnya 10 berarti menambah 10% damage dari atk)
int energy = 150;
const int energyMax = 150;
const int energyRegen = 10;
const unsigned long energyInterval = 10000;
bool robotAlive = true;
int hpstats = hp;

// Button pins
const int resetPin = 13;
const int atkPin = 14;
const int blockPin = 15;

// Block button
int block_press_count = 0;
int block_state = 0;
int prev_block_state = 0;
unsigned long lastBlockTime = 0, blockInterval = 3000; // Block timing
bool isBlocking = false;

// Attack button
int button_state = 0;
unsigned long lastAtkTime = 0, atkInterval = 500;  // Attack timing
bool isAttacking = false;

// Timing Variables
unsigned long lastRegen = 0;
unsigned long lastAttack = 0;
bool atkReleased = true;

// Variabel untuk animasi
int hitAnimationIndex = 0;
int defeatAnimationStep = 0;
unsigned long defeatMotionStart = 0;
bool isHitAnimating = false;
unsigned long hitAnimationStart = 0;
bool blockingAtkMotionActive = false;
unsigned long blockingAtkMotionStart = 0;

WiFiClient client;

void setup() {
    Serial.begin(115200);

    // Inisialisasi OLED
    Wire1.setSDA(18);  // Menggunakan pin 18 untuk SDA
    Wire1.setSCL(19);  // Menggunakan pin 19 untuk SCL
    Wire1.begin();

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
      Serial.println("OLED Gagal");
      while(1);
  }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    Wire.setSDA(20);
    Wire.setSCL(21);
    Wire.begin();
    pwm.begin();
    pwm.setPWMFreq(50); // Frekuensi untuk servo (50 Hz)

    // Inisialisasi Pin
    pinMode(atkPin, INPUT_PULLUP);
    pinMode(blockPin, INPUT_PULLUP);
    pinMode(resetPin, INPUT_PULLUP);

    setIdlePosition();
    connectToWiFi();
    connectToServer();
}

void setServoAngle(uint8_t servo, int angle) {
    int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
    pwm.setPWM(servo, 0, pulse);
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  //display.startscrollright(0x00, 0x0F); // Animasi scroll
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.println(clientName);
  display.println("HP: " + String(hp));
  display.println("ATK: " + String(atk));
  display.println("DEF: " + String(defense) + "%");
  display.println("STR: " + String(strength) + "%");
  display.println("ENGY: " + String(energy));
  display.display();
}

void displayPrint(const String &text) {
  display.setTextColor(WHITE);
  display.setTextSize(1);
  Serial.println(text);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(text);
  display.display();
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  displayPrint("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  displayPrint("WiFi Connected!\nIP: " + WiFi.localIP().toString());
  delay(2000);
}

void connectToServer() {
  displayPrint("Connecting to server...");
  while (!client.connect(serverIP, serverPort)) {
    Serial.println("Connection failed, retrying...");
    delay(5000);
  }
  client.println(clientName);
  displayPrint("Server Connected!");
}

void handleServer() {
  if (!client.connected()) connectToServer();
  
  while (client.available()) {
    String data = client.readStringUntil('\n');
    data.trim();
    Serial.println("Received: " + data);
    
    if (data.startsWith("Atk")) {
      int incomingDamage = data.substring(4).toInt();
      //handleDamage(data.substring(4).toInt());
      handleDamage(incomingDamage);
    }
  }
}

void setIdlePosition() {
  setServoAngle(SERVO_BAHUKANAN, 80);//-kanan +kiri
  setServoAngle(SERVO_TANGANKIRI, 130);// - semakin kecil semakin kedepan
  setServoAngle(SERVO_PUNGGUNG, 75);//-semakin kecil semakin naik
  setServoAngle(SERVO_SIKUKANAN, 40);//- semakin kecil semakin kebelakang
  setServoAngle(SERVO_PINGGANG, 40);//-semakin besar semakin ke kiri
  setServoAngle(SERVO_KAKIKANAN, 70);// -semakin besar semakin kedepan
  setServoAngle(SERVO_KAKIKIRI, 25);// -semakin kecil semakin kedepan
  setServoAngle(SERVO_LUTUTKANAN , 10);// -semakin kecil semakin lurus
  setServoAngle(SERVO_LUTUTKIRI , 130);// -semakin besar semakin lurus
  setServoAngle(SERVO_TELAPAKKANAN , 65);// -semakin besar semakin maju
  setServoAngle(SERVO_TELAPAKKIRI , 80);// -semakin kecil semakin maju

}

bool debounceButton(int pin) {
  static unsigned long lastDebounceTime = 0;
  static int lastButtonState = LOW;
  int currentState = digitalRead(pin);

  if (currentState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 50) {  // Debounce 50ms
    if (currentState != button_state) {
      button_state = currentState;
    }
  }

  lastButtonState = currentState;
  return button_state == HIGH;
}

void handleEnergy() {
  if (millis() - lastRegen >= energyInterval) {
    energy = min(energy + energyRegen, energyMax);
    lastRegen = millis();
  }
}

void handleDamage(int damage) {
  if (!robotAlive) return;

  // Jika sedang blok dan energi masih tersedia, serangan diblok
  if (isBlocking && energy > 0) {
    Serial.println("Attack Blocked!");
    energy = max(energy - 5, 0);
    handleBlockAtkmotion();
    return;
  } else {
    isBlocking = false;
  }

  int TotalDamage = damage - (damage * defense / 100);
  
  hp = max(hp - TotalDamage, 0);
  if (hp <= 0) {
    handleDefeat();
  } else {
    hitAnimationIndex = (hitAnimationIndex + 1) % 3;
    isHitAnimating = true;
    hitAnimationStart = millis();
  }
}

  void handleHitMotion()
  {
    if(!isHitAnimating) return;

    unsigned long currentTime = millis();
    switch(hitAnimationIndex) {
      case 0:
      if(currentTime - hitAnimationStart > 200) {
      setServoAngle(SERVO_PINGGANG, 80);
      setServoAngle(SERVO_TANGANKIRI, 120);
      Serial.println("Hit motion 1");
      delay(50);
      }
      if(currentTime - hitAnimationStart > 300) {
        setIdlePosition();
        isHitAnimating = false;
      }
      break;

      case 1:
      if(currentTime - hitAnimationStart > 200) {
      setServoAngle(SERVO_PINGGANG, 20);
      setServoAngle(SERVO_TANGANKIRI, 120);
      Serial.println("Hit motion 2");
      delay(50);
      }
      if(currentTime - hitAnimationStart > 300) {
        setIdlePosition();
        isHitAnimating = false;
      }
      break;

      case 2:
      if(currentTime - hitAnimationStart > 200) {
      setServoAngle(SERVO_PINGGANG, 20);
      setServoAngle(SERVO_KAKIKANAN, 180);
      Serial.println("Hit motion 3");
      delay(50);
      }
      if(currentTime - hitAnimationStart > 300) {
        setIdlePosition();
        isHitAnimating = false;
      }
      break;
    }
  }

void handleBlockAtkmotion() {

  if(!blockingAtkMotionActive){
  blockingAtkMotionActive = true;
  blockingAtkMotionStart = millis();
  }
  unsigned long currentTime = millis();
    if (currentTime - blockingAtkMotionStart > 200){
      setServoAngle(SERVO_TANGANKIRI, 70); 
      setServoAngle(SERVO_PINGGANG, 10);
      Serial.println("BlockingAtkMotion");
      delay(50);
    } 
    if (currentTime - blockingAtkMotionStart > 400){
      setIdlePosition();
      blockingAtkMotionActive = false;
    }
}

  void handleDefeat() {
  robotAlive = false;
  client.print(clientName + " defeated!\n"); // kirim pesan ke server
  defeatAnimationStep = 0;
  defeatMotionStart = millis();
  displayPrint("DEFEATED! Hold reset");
  displayPrint("5s to reboot");
  //client.stop();
}

void handleDefeatMotion() {
  switch(defeatAnimationStep) {
    case 0:
      setServoAngle(SERVO_PINGGANG, 20);
      setServoAngle(SERVO_TANGANKIRI, 140);
      setServoAngle(SERVO_SIKUKANAN, 80);
      if(millis() - defeatMotionStart > 1000) {
        defeatAnimationStep = 1;
        defeatMotionStart = millis();
      }
      break;
  }
}

void checkReset() {
    if (!robotAlive) {
        static unsigned long resetStart = 0;
        if (digitalRead(resetPin) == LOW) {
            if (resetStart == 0) {
                resetStart = millis();
                displayPrint("Resetting...");
            } else if (millis() - resetStart > 5000) {
                hp = hpstats;
                energy = energyMax;
                robotAlive = true;
                defeatAnimationStep = 0;
                //connectToServer();
                setIdlePosition();
                displayPrint("Reboot Complete!");
            }
        } else {
            resetStart = 0;
        }
    }
}

void sendAttack() {
  // Tambahkan bonus damage berdasarkan strength
  int totalAtk = atk + (atk * strength / 100);
  String msg = "Atk " + String(totalAtk) + "\n";
  client.print(msg);
  Serial.println("Attack Sent: " + msg);
}

void BlockState() //Blocking menggunakan millis
{
  static unsigned long lastEnergyDrain = 0;
  block_state = digitalRead(blockPin);
  
  if (block_state ==  LOW && !isBlocking && energy > 0) {
    isBlocking = true;
    energy = max(energy - 1, 0); // Energi berkurang 1 saat block
    lastBlockTime = millis();
    Serial.println("Blocking...");
  }

  if (isBlocking && energy > 0) {
    if (millis() - lastEnergyDrain > 1000) {
      lastEnergyDrain = millis();
    }
    unsigned long currentTime = millis();
    isBlocking = true;
    if (currentTime - lastBlockTime < 400) {
      setServoAngle(SERVO_TANGANKIRI,90);
      setServoAngle(SERVO_PINGGANG, 20);
      // setServoAngle(SERVO_BAHUKANAN, 100);//-kanan +kiri
      // setServoAngle(SERVO_PUNGGUNG, 110);//+turun -naik
      delay(50);
    } else {
      isBlocking = false;
    }
  }
}

void setIdleAnimation() 
{
  // Static variable untuk mencatat waktu idle mulai
  static unsigned long idleStartTime = 0;

  // Jika ada tombol yang ditekan atau motion lain aktif, reset timer dan keluar
  if (digitalRead(atkPin) == LOW || digitalRead(blockPin) == LOW 
      || isAttacking || isHitAnimating || blockingAtkMotionActive) {
    idleStartTime = 0;  // Reset waktu tunggu
    return;
  }

  // Jika kedua tombol tidak ditekan dan tidak ada motion aktif,
  // catat waktu mulai jika belum dicatat.
  if (idleStartTime == 0) {
    idleStartTime = millis();
  }

  // Tunggu hingga 1 detik telah berlalu sejak tombol dilepas
  if (millis() - idleStartTime >= 2000) {
    // Idle animation dijalankan
    static unsigned long lastUpdateTime = 0;
    const unsigned long updateInterval = 5; // Update setiap 5 ms
    static int offset = 0;    // Offset derajat dari posisi dasar
    static int step = 1;      // Langkah per update

    if (millis() - lastUpdateTime >= updateInterval) {
      lastUpdateTime = millis();
      offset += step;
      // Balik arah jika offset mencapai batas ±5 derajat
      if (offset >= 5 || offset <= -5) {
        step = -step;
      }
      // Terapkan pergeseran offset ke beberapa servo
      setServoAngle(SERVO_BAHUKANAN, 80 + offset);   // Contoh: Bahu kanan
      setServoAngle(SERVO_PUNGGUNG, 75 + offset);      // Punggung
      setServoAngle(SERVO_SIKUKANAN, 60 + offset);      // Siku kanan
      setServoAngle(SERVO_PINGGANG, 40 + offset);       // Pinggang
      // setServoAngle(SERVO_KAKIKANAN, 70 + offset);      // Kaki kanan
      // setServoAngle(SERVO_KAKIKIRI, 30 + offset);       // Kaki kiri
    }
  }
}

void handleAtkState() {

  if (isBlocking) {
    Serial.println("Cannot attack while blocking.");
    return;  // Langsung keluar dari fungsi jika blocking aktif
  }
  
  static int animationIndex = 0;  // Indeks untuk memilih animasi
  button_state = debounceButton(atkPin);

  if (button_state == LOW && !isAttacking) {
    if (!isBlocking && digitalRead(atkPin) == LOW && atkReleased) {
      isAttacking = true;
      if (millis() - lastAttack > 1000 && energy >= 10) {
      sendAttack();
      energy = max(energy - 5, 0);
      lastAtkTime = millis();
      Serial.println("Attack started.");
      atkReleased = false;
      }
    }
  }
  if (isAttacking) {
    unsigned long currentTime = millis();

    switch (animationIndex) {
      case 0:  // Animasi serangan 1
        if (currentTime - lastAtkTime < 200) {
          setServoAngle(SERVO_PINGGANG, 120);
          setServoAngle(SERVO_PUNGGUNG, 50);
          setServoAngle(SERVO_BAHUKANAN, 150);
          // setServoAngle(SERVO_LUTUTKIRI , 120);// -semakin besar semakin lurus
          setServoAngle(SERVO_SIKUKANAN, 80);//- semakin kecil semakin kebelakang
          // setServoAngle(SERVO_KAKIKIRI, 20);// -semakin kecil semakin kedepan
          delay(50);
        } else if (currentTime - lastAtkTime < 500) {
          setServoAngle(SERVO_PINGGANG, 20);
          setServoAngle(SERVO_PUNGGUNG, 50);
          setServoAngle(SERVO_BAHUKANAN, 40);
          setServoAngle(SERVO_SIKUKANAN, 90);;//-kanan +kiri
          delay(50);
        } else {
          isAttacking = false;
          animationIndex = (animationIndex + 1) % 3;  // Pindah ke animasi berikutnya
          setIdlePosition();
          Serial.println("Attack 1 ended.");
          atkReleased = true;
        }
        break;

      case 1:  // Animasi serangan 2
        if (currentTime - lastAtkTime < 200) {
          // setServoAngle(SERVO_BAHUKANAN, 180);
          setServoAngle(SERVO_PINGGANG, 20);
          setServoAngle(SERVO_PUNGGUNG, 50);
          setServoAngle(SERVO_BAHUKANAN, 90);
          setServoAngle(SERVO_SIKUKANAN, 90);//- semakin kecil semakin kebelakang
          // setServoAngle(SERVO_KAKIKIRI, 20);// -semakin kecil semakin kedepan
          delay(50);
        } else if (currentTime - lastAtkTime < 500) {
          setServoAngle(SERVO_PUNGGUNG, 50);
          setServoAngle(SERVO_PINGGANG, 120);
          setServoAngle(SERVO_BAHUKANAN, 140);
          // setServoAngle(SERVO_LUTUTKIRI , 120);// -semakin besar semakin lurus
          setServoAngle(SERVO_SIKUKANAN, 90);;//-kanan +kiri
          delay(50);
        }
         else {
          isAttacking = false;
          animationIndex = (animationIndex + 1) % 3;  // Pindah ke animasi berikutnya
          setIdlePosition();
          Serial.println("Attack 2 ended.");
          atkReleased = true;
        }
        break;

      case 2:  // Animasi serangan 3 -+
        if (currentTime - lastAtkTime < 200) {

         setServoAngle(SERVO_PINGGANG, 60);
         setServoAngle(SERVO_SIKUKANAN, 90);;//-kanan +kiri
         delay(50);
        } else if (currentTime - lastAtkTime < 400) {
          setServoAngle(SERVO_PINGGANG, 120);
          setServoAngle(SERVO_PUNGGUNG, 40);
          setServoAngle(SERVO_BAHUKANAN, 100);
          // setServoAngle(SERVO_LUTUTKIRI , 120);// -semakin besar semakin lurus
          setServoAngle(SERVO_SIKUKANAN, 90);;//-kanan +kiri
          // setServoAngle(SERVO_KAKIKIRI, 20);// -semakin kecil semakin kedepan
          delay(50);
        }
          else {
          isAttacking = false;
          animationIndex = (animationIndex + 1) % 3;  // Pindah ke animasi berikutnya
          setIdlePosition();
          Serial.println("Attack 3 ended.");
          atkReleased = true;
        }
        break;
    }
  }
  else {
    // Servo dalam posisi idle saat tidak menyerang
    setIdlePosition();
    setIdleAnimation();
  }
}


void loop() {
  if (!robotAlive) {
    handleDefeatMotion();
    checkReset();
    handleServer();
    updateDisplay();
    //checkReset();
    return;
  }
  handleEnergy();

  if (isHitAnimating) {
    handleHitMotion();
  }

  handleServer();
  updateDisplay();
  handleAtkState();
  BlockState();
}
