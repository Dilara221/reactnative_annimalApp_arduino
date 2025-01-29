#include <Adafruit_SleepyDog.h> // Güç tasarrufu ve sistem izleme için Watchdog Timer kütüphanesi.
#include <SoftwareSerial.h>  // Yazılımsal seri haberleşme için kütüphane.
#include "Adafruit_FONA.h"  // FONA GSM/GPRS modülü için kullanılan kütüphane.

#define halt(s) { Serial.println(F( s )); while(1);  } // Hata durumunda mesaj yazdırır ve sistemi durdurur.

// FONA ve yazılımsal seri port nesneleri başka bir yerden tanımlanmış, burada yeniden erişiliyor.
extern Adafruit_FONA fona;   // FONA GSM/GPRS modülü için bir nesne.
extern SoftwareSerial fonaSS;  // FONA için yazılımsal seri haberleşme portu.

boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password) {
  Watchdog.reset();  // Watchdog Timer'ı sıfırlar, sistemin kitlenmesini önler.

  Serial.println(F("Initializing FONA....(May take 3 seconds)"));
  
  fonaSS.begin(9600); // Yazılımsal seri port hızını (baud rate) 9600 olarak ayarlar. FONA ile haberleşme başlar.
  if (! fona.begin(fonaSS)) {  // FONA modülünü başlatmaya çalışır. Başarısız olursa:   
    Serial.println(F("Couldn't find FONA"));  // Fonksiyon başarısızlıkla döner.
    return false;
  }
  fonaSS.println("AT+CMEE=2");  // FONA'nın hata mesajlarını detaylı olarak gösterecek AT komutunu gönderir.
  Serial.println(F("FONA is OK")); // FONA başarılı şekilde başlatıldı mesajı.
  Watchdog.reset();

  Serial.println(F("Checking for network...")); // Ağ bağlantısı kontrolü yapılıyor mesajı.
  while (fona.getNetworkStatus() != 1) {  // Modül hücresel ağa bağlanana kadar bekler.
   delay(500);  // Her 500 ms'de bir ağ durumunu kontrol eder.
  }

  Watchdog.reset();
  delay(5000);  //bağlantı stabilleşene kadar bekle
  Watchdog.reset();
  
  fona.setGPRSNetworkSettings(apn, username, password); // APN, kullanıcı adı ve şifreyi ayarlar.

  Serial.println(F("Disabling GPRS"));   // GPRS bağlantısı kapatılıyor mesajı.
  fona.enableGPRS(false);
  
  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();

  Serial.println(F("Enabling GPRS"));
  if (!fona.enableGPRS(true)) {
    Serial.println(F("Failed to turn GPRS on"));  
    return false;
  }
  Watchdog.reset();

  return true;
}