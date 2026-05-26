/*
  NABHYAN Transmitter (Tx) - FINAL FLIGHT READY
  
  Updates:
  1. KILL SWITCH: Button S6 (Pin 18). Press to Kill, Release to Fly.
  2. SILENT BOOT: Pin 15 (Buzzer) forced LOW to stop noise.
*/
#include <esp_now.h>
#include <WiFi.h>

// 1. CONFIRM YOUR RECEIVER MAC ADDRESS
uint8_t broadcastAddress[] = {0x1C, 0xC3, 0xAB, 0xF9, 0x1E, 0x84};
typedef struct struct_message {
  int lr;
  int ud;
} struct_message;

struct_message myData={1930,1874};
esp_now_peer_info_t peerInfo;

// --- PIN DEFINITIONS ---
const int left_right = 36; 
const int up_down = 34; 
const int PIN_BUZZER = 15;
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  // Optional debug
}

void setup() {
  Serial.begin(115200);
  
  // 1. SILENCE THE BUZZER (First priority!)
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Ensure Buzzer stays OFF (Just in case)
  digitalWrite(PIN_BUZZER, LOW);

  // Read Analog Controls
  myData.lr   = analogRead(left_right);
  myData.ud   = analogRead(up_down);
  

  // Send Message
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  delay(20); 
}