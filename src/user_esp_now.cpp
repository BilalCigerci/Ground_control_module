#include "user_esp_now.h"

static uint8_t broadcastAddress[] = {0xE8, 0x31, 0xCD, 0xAD, 0x33, 0xCC};  //receiver E8:31:CD:AD:33:CC
static char data_received[100];
static esp_now_peer_info_t peerInfo;

// esp-now init
void user_esp_now_init(void)
{
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);
  esp_now_register_recv_cb(OnDataRecv);
}

void user_esp_now_send(const uint8_t *data, size_t len)
{
  esp_now_send(broadcastAddress, data, len);
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char success[25];
  if (status ==0){
    strcpy(success, "Delivery Success :)");
  }
  else{
    strcpy(success, "Delivery Fail :(");
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  memcpy(data_received, incomingData, len);
}
