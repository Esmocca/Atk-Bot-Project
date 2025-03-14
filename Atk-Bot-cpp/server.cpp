#include <WiFi.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Konfigurasi Wi-Fi
const char* ssid = "Alamak";
const char* password = "ndaktaukoktanyasaya";

// Konfigurasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Konfigurasi Server
WiFiServer server(50003);
WiFiClient clients[5]; // Maksimal 5 klien
int numClients = 0;
bool communicationStarted = false;

String clientNames[2];
int wins[2] = {0, 0};
int roundCount = 1;

void displayOnOLED(const String &text) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    
    String lines[6];
    int lineIndex = 0;
    int start = 0;
    
    // Split text into lines
    for(int i = 0; i < text.length() && lineIndex < 6; i++) {
        if(text[i] == '\n' || (i - start) >= 16) {
            lines[lineIndex++] = text.substring(start, i);
            start = i + 1;
        }
    }
    if(lineIndex < 6 && start < text.length()) {
        lines[lineIndex] = text.substring(start);
    }
    
    // Display lines
    for(int i = 0; i < 6; i++) {
        display.setCursor(0, i * 10);
        display.print(lines[i]);
    }
    display.display();
}

void customPrint(const String &text) {
    Serial.println(text);
    displayOnOLED(text);
}

void setup() {
    Serial.begin(115200);

    //Inisialisasi OLED
    Wire.setSDA(20);
    Wire.setSCL(21);
    Wire.begin();
    
    // Inisialisasi OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("OLED failed"));
        while(1);
    }
    display.setTextSize(1);
    
    // Koneksi Wi-Fi
    customPrint("Connecting to\nWi-Fi...");
    WiFi.begin(ssid, password);
    
    while(WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    
    customPrint("Connected to\nWi-Fi!\nIP: " + WiFi.localIP().toString());
    
    // Mulai server
    server.begin();
    customPrint("Server ready\nPort 50003");
}

void loop() {
   // Terima koneksi baru
    if(numClients < 2) {
        WiFiClient newClient = server.available();
        if(newClient) {
            String clientName = newClient.readStringUntil('\n');
            clientName.trim();
            if(clientName.length() > 0) {
                clients[numClients] = newClient;
                clientNames[numClients] = clientName;
                numClients++;
                customPrint("Player" + String(numClients) + " connected\n" + clientName);
                if(numClients == 2) {
                    communicationStarted = true;
                    customPrint("Game Start!\nRound" + String(roundCount));
                }
            } else {
                newClient.stop();
            }
        }
    }
    
    // Baca data dari klien
    for(int i = 0; i < numClients; i++) {
        if(clients[i] && clients[i].available()) {
            String data = clients[i].readStringUntil('\n');
            data.trim();
            customPrint("Received:\n" + data);
            
            if(data.endsWith("defeated!")) {
                String winner = clientNames[(i + 1) % 2];
                wins[(i + 1) % 2]++;
                customPrint(winner + " Win!\nRound" + String(roundCount) + " over");
                roundCount++;
                if(wins[(i + 1) % 2] == 3) {
                    customPrint(winner + " is the\nfinal winner!");
                    // Reset game
                    numClients = 0;
                    communicationStarted = false;
                    roundCount = 1;
                    wins[0] = wins[1] = 0;
                    for(int j = 0; j < 2; j++) {
                        clients[j].stop();
                    }
                } else {
                    customPrint("Next Round\nRound" + String(roundCount));
                }
            }
            
            // Kirim ke klien lain
            for(int j = 0; j < numClients; j++) {
                if(j != i && clients[j].connected()) {
                    clients[j].println(data);
                }
            }
        }
    }
    
    // Cek koneksi klien
    for(int i = 0; i < numClients; i++) {
        if(!clients[i].connected()) {
            clients[i].stop();
            // Geser array
            for(int j = i; j < numClients-1; j++) {
                clients[j] = clients[j+1];
                clientNames[j] = clientNames[j+1];
            }
            numClients--;
            customPrint("Player" + String(i+1) + "\ndisconnected");
        }
    }
}
    
