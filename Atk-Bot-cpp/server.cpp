#include <WiFi.h>
#include <Wire.h>
// #include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
// #define LCD_SDA 16
// #define LCD_SCL 17
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

const char* ssid = "Alamak";
const char* password = "ndaktaukoktanyasaya";
int port = 50003;
WiFiServer server(port);

// Button pins
const int hpbuffPins = 11;
const int atkbuffPins = 12;
const int deffbuffPins = 13;
const int strbuffPins = 14;
const int engybuffPins = 15;

// LiquidCrystal_I2C lcd(0x27, 16, 2);

// byte fullBar[8] = {
//   0b11111,
//   0b11111,
//   0b11111,
//   0b11111,
//   0b11111,
//   0b11111,
//   0b11111,
//   0b11111
// };

struct ClientData {
  WiFiClient client;
  String name;
  // int lifePoints = 3;
};

template <class T>
class DNode {
public:
  T data;
  DNode<T>* next;
  DNode<T>* prev;
  DNode(const T& d, DNode<T>* p = nullptr, DNode<T>* n = nullptr)
    : data(d), prev(p), next(n) {}
};

template <class T>
class DoublyLinkedList {
private:
  DNode<T>* head;
  DNode<T>* tail;
public:
  DoublyLinkedList() : head(nullptr), tail(nullptr) {}
  ~DoublyLinkedList() {
    while (head) {
      DNode<T>* tmp = head;
      head = head->next;
      delete tmp;
    }
  }
  void insertBack(const T& data) {
    DNode<T>* newNode = new DNode<T>(data, tail, nullptr);
    if (tail) {
      tail->next = newNode;
    } else {
      head = newNode;
    }
    tail = newNode;
  }
  void removeNode(DNode<T>* node) {
    if (!node) return;
    if (node->prev)
      node->prev->next = node->next;
    else
      head = node->next;
    if (node->next)
      node->next->prev = node->prev;
    else
      tail = node->prev;
    delete node;
  }
  DNode<T>* getHead() {
    return head;
  }
  int count() {
    int c = 0;
    DNode<T>* temp = head;
    while (temp) {
      c++;
      temp = temp->next;
    }
    return c;
  }
};

DoublyLinkedList<ClientData> clientList;
// bool gameStarted = false;
bool serverReady = false;

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    server.begin();
    
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("IP: ");
    // lcd.print(WiFi.localIP());
    // lcd.setCursor(0, 1);
    // lcd.print("Port: ");
    // lcd.print(port);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.print("Port: ");
    display.println(port);
    display.display();
  }
}

// void displayGameStart() {
//   lcd.clear();
//   lcd.setCursor(3, 0);
//   lcd.print("Game Start!");
//   delay(3000);
//   gameStarted = true;
// }

void displayServerReady() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 20);
  display.print("Server");
  display.setCursor(25, 40);
  display.print("Ready!");
  display.display();
  serverReady = true;
}

// void displayScoreboard() {
//   static int lastP1LP = -1, lastP2LP = -1;
//   DNode<ClientData>* current = clientList.getHead();
//   if (!current || !current->next) return;

//   if (current->data.lifePoints == lastP1LP && current->next->data.lifePoints == lastP2LP) {
//     return; 
//   }
  
//   lastP1LP = current->data.lifePoints;
//   lastP2LP = current->next->data.lifePoints;

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("P1");
//   lcd.setCursor(14, 0);
//   lcd.print("P2");

//   lcd.setCursor(0, 1);
//   for (int i = 0; i < current->data.lifePoints; i++) {
//     lcd.write(byte(0)); 
//   }
//   lcd.setCursor(13, 1);
//   for (int i = 0; i < current->next->data.lifePoints; i++) {
//     lcd.write(byte(0)); 
//   }
// }

void mainServer() {
  WiFiClient newClient = server.accept();
  if (newClient) {
    String clientName = "Player_" + String(random(1, 99)); 
    ClientData cd;
    cd.client = newClient;
    cd.name = clientName;
    clientList.insertBack(cd);
  }
  
  if (clientList.count() == 2 && !gameStarted) {
    // displayGameStart();
    displayServerReady();
  }
  
  // if (gameStarted) {
  //   displayScoreboard();
  // }

  // bool gameOver = false;
  
  DNode<ClientData>* current = clientList.getHead();
  while (current != nullptr) {
    WiFiClient &client = current->data.client;
    
    if (client && client.available()) {
      String data = client.readStringUntil('\n');
      data.trim();
      Serial.println("Received: " + data);

      // if (data.endsWith("defeated!")) {
      //   current->data.lifePoints--;
        
      //   if (current->data.lifePoints == 0) {
      //     gameOver = true;
      //     lcd.clear();
      //     lcd.setCursor(3, 0);
      //     lcd.print("Game Over");
      //     //lcd.print(current->next ? current->next->data.name : "?");
      //     delay(5000);
      //     lcd.clear();
      //     gameStarted = false;
      //     break;
      //   }
      // }

      DNode<ClientData>* receiver =  clientList.getHead();
      while (receiver != nullptr){
        if (receiver != current  && receiver->data.client.connected()) {
          receiver ->data.client.println(data);
        }
        receiver = receiver->next;
      }
      // displayScoreboard();
    }

    if (!client.connected()) {
      DNode<ClientData>* toDelete = current;
      current = current->next;
      clientList.removeNode(toDelete);
    } else {
      current = current->next;
    }
  }
  
  // if (clientList.count() == 2) {
  //   DNode<ClientData>* p1 = clientList.getHead();
  //   DNode<ClientData>* p2 = p1->next;
  //   if (p1->data.lifePoints == 0 && p2->data.lifePoints == 0) {
  //     lcd.clear();
  //     lcd.setCursor(3, 0);
  //     lcd.print("DRAW!");
  //     delay(5000);
  //     lcd.clear();
  //     gameStarted = false;
  //   }
  // }
}


void setup() {
  Serial.begin(115200);

  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed to start"));
    while (true);
  }
  // lcd.init();
  // lcd.backlight();
  // lcd.createChar(0, fullBar); 
  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("Wi-Fi Connecting...");
  // delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Connecting WiFi...");
  display.display();
  delay(1000);
  
  connectToWiFi();
}

void loop() {
  connectToWiFi();
  mainServer();
}
