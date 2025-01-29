#include <Adafruit_SleepyDog.h> //güç tasarufu modu
#include <SoftwareSerial.h>  // Yazılımsal seri port kütüphanesi
#include "Adafruit_FONA.h"   // FONA GSM/GPRS modülü için kütüphane
#include "Adafruit_MQTT.h"   // MQTT protokolü kütüphanesi
#include "Adafruit_MQTT_FONA.h" // FONA ile MQTT kullanımı için gerekli kütüphane

// GSM Shield için pin ayarları
#define FONA_RX  11
#define FONA_TX  10
#define FONA_RST 4
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX); // FONA için yazılımsal seri port tanımlama


Adafruit_FONA fona = Adafruit_FONA(FONA_RST); // FONA nesnesi oluşturma


// Hücresel bağlantı ayarları
#define FONA_APN        "internet"
#define FONA_USERNAME   ""
#define FONA_PASSWORD   ""

// MQTT Sunucu bilgileri
#define AIO_SERVER      "142.93.101.74"
#define AIO_SERVERPORT  1883

// FONA üzerinden MQTT nesnesi oluşturma
Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT);


// Yardımcı makro: Hata durumunda sistem durdurulur
#define halt(s) { Serial.println(F( s )); while(1); }
boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);


// Bağlantı başarısızlıklarını izlemek için sayaç
uint8_t txfailures = 0;
#define MAXTXFAILURES 3

// GPS modülü için ayarlar
#include <TinyGPS.h>
#define GPS_RX_PIN 0
#define GPS_TX_PIN 1

TinyGPS gps; // GPS nesnesi
const char* deviceID = "741";

// Fonksiyon prototipleri
boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);
void MQTT_connect();
static void smartdelay(unsigned long ms);

// Kurulum
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600); // Seri portu başlat
  Serial.println(F("Sistem başlatıldı."));


  // FONA bağlantısı hücreselveri
  while (!FONAconnect(F(FONA_APN), F(FONA_USERNAME), F(FONA_PASSWORD))) {
    Serial.println(F("FONA bağlantısı yeniden deneniyor..."));
  }
  Serial.println(F("Hücresel bağlantı kuruldu!"));
}

// Ana döngü
void loop() {
  MQTT_connect(); // MQTT bağlantısını kontrol et ve gerekirse bağlan
  smartdelay(1000);// GPS veri işleme ve gecikme

  // GPS'ten konum bilgilerini al
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  int alt = gps.f_altitude();
  int spd = gps.f_speed_kmph();
  int crs = gps.f_course();

String jsonData = "{\"latitude\": " + String(flat, 6) + ", \"longitude\": " + String(flon, 6) + 
                    ", \"altitude\": " + String(alt) + ", \"speed\": " + String(spd) + ", \"deviceID\": \"" + String(deviceID) + "\"" +
                    ", \"course\": " + String(crs) + "}";
  char locData2[256];
  jsonData.toCharArray(locData2, 256);

  Serial.print(F("Konum bilgisi gönderiliyor: "));
  Serial.println(locData2);

  if (!mqtt.publish("LOCATION", locData2, 1, 1)) { //qos 1 çünkü hızlı ve onay bekliyor güvenilir, retain abone olunduğunda son mesajı iletir
    Serial.println(F("Gönderim başarısız"));
  } else {
    Serial.println(F("Gönderim başarılı"));
  }

  // FONA üzerinden hücresel bağlantıyı kur
boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password) {
  if (!fona.begin(fonaSS)) {  // FONA başlangıç
    Serial.println(F("FONA modül başlatılamadı."));
    return false;
  }
  if (!fona.enableGPRS(true)) {  // GPRS etkinleştirme
    Serial.println(F("GPRS bağlantısı başarısız."));
    return false;
  }
  Serial.println(F("GPRS bağlantısı kuruldu."));
  return true;
}



// MQTT bağlantısı
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {   // Eğer zaten bağlıysa (mqtt.connected()), fonksiyondan çık
    return;
  }

  Serial.print(F("MQTT bağlantısı kuruluyor... "));
  while ((ret = mqtt.connect()) != 0) {
        // Eğer bağlantı başarısız olursa hata mesajını yazdır
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println(F("MQTT bağlantısı tekrar deneniyor..."));
        // Bağlantıyı kes ve 5 saniye beklemeden sonra tekrar dene

    mqtt.disconnect();
    delay(5000);
  }
  Serial.println(F("MQTT bağlantısı başarılı!"));
}

// GPS veri okuma
static void smartdelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    // Seri porttan veri varsa, bu veriyi GPS kütüphanesine gönder
    while (Serial.available()) {
      gps.encode(Serial.read());// GPS verisini işle
    }
  } while (millis() - start < ms); // Bekleme süresi dolana kadar döngü devam eder
}
//Ancak smartdelay, hem belirli bir süre beklerken hem de GPS’ten gelen verileri sürekli işler.
//	9600, saniyede 9600 bit veri gönderileceği anlamına gelir.
//GPS ve GSM modüllerinin dokümanlarında belirtilen baud hızı, Arduino’daki Serial.begin() fonksiyonunda ayarlanmalıdır.
//GSM modülü, GPRS bağlantısını kurar ve MQTT üzerinden mesaj göndermek için kullanılır.
//Bekleme Süresi: Sistem doğru zamanda veri almak için beklerken işleme devam eder.