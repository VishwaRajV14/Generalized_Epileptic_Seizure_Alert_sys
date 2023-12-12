#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClientESP32Ex.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "smart-wearables-d10b9-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "k0qfDcqJLrxzJDzfI1ArAOJPKvNI2LpZc8p8Sbg4"

FirebaseData fbdo;
FirebaseJson json;

#define ssid "SSN"
#define password "Ssn1!Som2@Sase3#"

//object for esp now data transfer
typedef struct struct_message 
{
  String model_name;
  float temp;
  float humid;
} struct_message;
struct_message Watch;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  memcpy(&Watch, incomingData, sizeof(Watch));
  Serial.println("Data read");
  Serial.print("Model_name: ");
  Serial.println("Rescuer_Watch_1");
  Serial.print("Temperature: ");
  Serial.println(Watch.temp);
  Serial.println("Humidity: ");
  Serial.println(Watch.humid);
  Serial.println();
}

///////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////

void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());

   
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Now connecting to Firebase");
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}
 
void loop() 
{
  /*
   * This code acts the master esp so after receiving the data from all the watches , we 
   * need to classify them properly and then save it to cloud 
   * 
   * we need to note that many watches will be sending the data to the master
   * So should come up with some queue type algo to maintain the data properly
   */
  Firebase.setFloat(fbdo,"/TEMPERATURE",Watch.temp);
  Firebase.setFloat(fbdo,"/HUMIDITY",Watch.humid);
}
