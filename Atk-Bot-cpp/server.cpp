#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_SDA 16
#define LCD_SCL 17


const char* ssid = "------";
const char* password = "----------";
int port = 50---;
WiFiServer server(port);

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte fullBar[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

struct ClientData {
  WiFiClient client;
  String name;
  int lifePoints = 3;
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
bool gameStarted = false;

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    server.begin();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP: ");
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print("Port: ");
    lcd.print(port);
  }
}

void displayGameStart() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Game Start!");
  delay(3000);
  gameStarted = true;
}

void displayScoreboard() {
  static int lastP1LP = -1, lastP2LP = -1;
  DNode<ClientData>* current = clientList.getHead();
  if (!current || !current->next) return;

  if (current->data.lifePoints == lastP1LP && current->next->data.lifePoints == lastP2LP) {
    return; 
  }
  
  lastP1LP = current->data.lifePoints;
  lastP2LP = current->next->data.lifePoints;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P1");
  lcd.setCursor(14, 0);
  lcd.print("P2");

  lcd.setCursor(0, 1);
  for (int i = 0; i < current->data.lifePoints; i++) {
    lcd.write(byte(0)); 
  }
  lcd.setCursor(13, 1);
  for (int i = 0; i < current->next->data.lifePoints; i++) {
    lcd.write(byte(0)); 
  }
}

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
    displayGameStart();
  }
  
  if (gameStarted) {
    displayScoreboard();
  }

  bool gameOver = false;
  
  DNode<ClientData>* current = clientList.getHead();
  while (current != nullptr) {
    WiFiClient &client = current->data.client;
    if (client && client.available()) {
      String data = client.readStringUntil('\n');
      data.trim();
      Serial.println("Received: " + data);

      if (data.endsWith("defeated!")) {
        current->data.lifePoints--;
        
        if (current->data.lifePoints == 0) {
          gameOver = true;
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.print("Game Over");
          //lcd.print(current->next ? current->next->data.name : "?");
          delay(5000);
          lcd.clear();
          gameStarted = false;
          break;
        }
      }

      DNode<ClientData>* receiver =  clientList.getHead();
      while (receiver != nullptr){
        if (receiver != current  && receiver->data.client.connected()) {
          receiver ->data.client.println(data);
        }
        receiver = receiver->next;
      }
      displayScoreboard();
    }

    if (!client.connected()) {
      DNode<ClientData>* toDelete = current;
      current = current->next;
      clientList.removeNode(toDelete);
    } else {
      current = current->next;
    }
  }
  
  if (clientList.count() == 2) {
    DNode<ClientData>* p1 = clientList.getHead();
    DNode<ClientData>* p2 = p1->next;
    if (p1->data.lifePoints == 0 && p2->data.lifePoints == 0) {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("DRAW!");
      delay(5000);
      lcd.clear();
      gameStarted = false;
    }
  }
}


void setup() {
  Serial.begin(115200);
  Wire.setSDA(LCD_SDA);
  Wire.setSCL(LCD_SCL);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, fullBar); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi Connecting...");
  delay(2000);
  connectToWiFi();
}

void loop() {
  connectToWiFi();
  mainServer();
}
