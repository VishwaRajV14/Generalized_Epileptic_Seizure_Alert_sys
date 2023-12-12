
 
 //Senders in our project are basically the slave esp, they going to read the sensor data and send to the master slave
 //Receivers in our project is The Master esp present in the control room

#include <esp_now.h>
#include <WiFi.h>
#include"DHT.h"

DHT temp_sensor(5,DHT11);
uint8_t broadcastAddress[] = {0x40, 0x91, 0x51, 0xBF, 0xB9, 0xA0};
//40:91:51:BF:B9:A0

typedef struct struct_message 
{
  String model_name;
  float temp;
  float humid;
} struct_message;

// An array can be created for multiple slaves and can be initiales with different names to differentiate
struct_message Watch;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//bpm
/*int x = 0;
int LastTime = 0;
bool BPMTiming = false;
bool BeatComplete = false;
int BPM = 0;    
#define UpperThreshold 518
#define LowerThreshold 490  */  

/////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
 
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  temp_sensor.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  float temp=temp_sensor.readTemperature();
  float humid=temp_sensor.readHumidity();

 /* int value = analogRead(13);
  if (value > UpperThreshold) {
    if (BeatComplete) {
      BPM = millis() - LastTime;
      BPM = int(60 / (float(BPM) / 1000));
      BPMTiming = false;
      BeatComplete = false;
    }
    if (BPMTiming == false) {
      LastTime = millis();
      BPMTiming = true;
    }
  }
  if ((value < LowerThreshold) & (BPMTiming))
    BeatComplete = true;*/
  


  Watch.model_name="Rescuer_watch_1";
  Watch.temp = temp;
  Watch.humid=humid;
  //Watch.bpm=BPM;
  
  // Send message via ESP-NOW
  esp_err_t  result = esp_now_send(broadcastAddress, (uint8_t *) &Watch, sizeof(Watch));
   
  if (result == ESP_OK)
   Serial.println("Temp and humidity sent");
  else 
    Serial.println("Error sending the data");

   Serial.println(Watch.temp);
   Serial.println(Watch.humid);
  // Serial.println(Watch.bpm); 
  
  delay(2000);
}
