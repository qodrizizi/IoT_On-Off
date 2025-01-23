#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "UMSU_WIFI_LT7";       // Nama WiFi Anda
const char* password = "";                // Password WiFi Anda
const char* serverUrl = "https://exam1.unimed.ac.id/iot/tampil.php"; // Ubah ke HTTPS

const int ledPin = D5;    // Pin untuk LED
const int pirPin = D6;    // Pin untuk PIR sensor

bool lastMotionState = false; // Untuk menyimpan status gerakan sebelumnya

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  
  // Koneksi ke WiFi
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  bool motionDetected = digitalRead(pirPin); // Membaca status PIR
 
  if (motionDetected != lastMotionState) { // Jika ada perubahan status
    lastMotionState = motionDetected;

    Serial.print("Status gerakan: ");
    Serial.println(motionDetected ? "Terdeteksi" : "Tidak terdeteksi");

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClientSecure wifiClient;
      wifiClient.setInsecure(); // Menonaktifkan verifikasi sertifikat (hanya untuk pengujian)

      HTTPClient http;
      String url = serverUrl + String("?payload=") + (motionDetected ? "1" : "0"); // Kirim payload sesuai status
      Serial.println("Mengirim data ke URL: " + url);
      http.begin(wifiClient, url);

      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.println("Payload terkirim dengan sukses.");
        Serial.println("Kode respon server: " + String(httpResponseCode));
        Serial.println("Respon dari server: " + http.getString());
      } else {
        Serial.println("Gagal mengakses URL, kode respon: " + String(httpResponseCode));
      }

      http.end();
    } else {
      Serial.println("WiFi tidak terhubung. Coba lagi.");
    }
  }

  // Kontrol LED berdasarkan status PIR
  digitalWrite(ledPin, motionDetected ? HIGH : LOW);
  Serial.print("Status LED: ");
  Serial.println(motionDetected ? "ON" : "OFF");

  delay(100); // Jeda untuk menghindari pembacaan terlalu cepat
}