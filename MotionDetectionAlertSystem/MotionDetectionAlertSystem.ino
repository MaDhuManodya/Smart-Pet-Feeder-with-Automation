#include <WiFi.h>
#include "time.h"
#include "sntp.h"
#include <HTTPClient.h>
#include "esp_camera.h"

// Replace with your WiFi credentials and Telegram bot token
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";
const char* botToken = "Your_Telegram_Bot_Token";
const char* chatID = "Your_Telegram_Chat_ID";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 19800; // Adjust as per your time zone offset
const int daylightOffset_sec = 0;
const int pirPin = 13;
bool pirPowerOn = false;
bool messagePrinted = false;
camera_fb_t * fb = NULL;

WiFiServer server(80);

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("No time available (yet)");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void timeavailable(struct timeval *t) {
    Serial.println("Got time adjustment from NTP!");
    printLocalTime();
}

void sendMessageToTelegram(const char* message) {
    HTTPClient http;
    http.begin("https://api.telegram.org/bot" + String(botToken) + "/sendMessage?chat_id=" + String(chatID) + "&text=" + message);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        Serial.print("Telegram message sent, response code: ");
        Serial.println(httpResponseCode);
    } else {
        Serial.print("Error sending message to Telegram, error code: ");
        Serial.println(httpResponseCode);
    }
    http.end();
}

void sendPhotoToTelegram(uint8_t* data, size_t len) {
    HTTPClient http;
    http.begin("https://api.telegram.org/bot" + String(botToken) + "/sendPhoto?chat_id=" + String(chatID));
    http.addHeader("Content-Type", "application/octet-stream");

    int httpResponseCode = http.POST(data, len);
    if (httpResponseCode > 0) {
        Serial.print("Photo sent to Telegram, response code: ");
        Serial.println(httpResponseCode);
    } else {
        Serial.print("Error sending photo to Telegram, error code: ");
        Serial.println(httpResponseCode);
    }
    http.end();
}

void setup() {
    Serial.begin(115200);

    sntp_set_time_sync_notification_cb(timeavailable);
    sntp_servermode_dhcp(1);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println(" CONNECTED");
    pinMode(pirPin, INPUT);

    camera_config_t config;
    // Camera configuration parameters
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 5;
    config.pin_d1 = 18;
    config.pin_d2 = 19;
    config.pin_d3 = 21;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;
    config.pin_xclk = 0;
    config.pin_pclk = 22;
    config.pin_vsync = 25;
    config.pin_href = 23;
    config.pin_sscb_sda = 26;
    config.pin_sscb_scl = 27;
    config.pin_pwdn = 32;
    config.pin_reset = -1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    server.begin();
}

void loop() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }

    if (!pirPowerOn) {
        if (digitalRead(pirPin) == HIGH) {
            pirPowerOn = true;
            Serial.println("PIR sensor powered on!");
        } else {
            Serial.println("Waiting for PIR sensor to power on...");
            delay(1000);
            return;
        }
    }

    WiFiClient client = server.available();
    if (client) {
        while (client.connected()) {
            if (client.available()) {
                String request = client.readStringUntil('\r\n');
                if (request.indexOf("/photo") != -1) {
                    Serial.println("Received /photo command");
                    sendMessageToTelegram("Command received: /photo");
                    delay(5000);
                    fb = esp_camera_fb_get();
                    if (fb) {
                        sendPhotoToTelegram(fb->buf, fb->len);
                        esp_camera_fb_return(fb);
                    }
                }
                break;
            }
        }
        client.stop();
    }

    if ((timeinfo.tm_hour >= 8 && timeinfo.tm_hour < 9) ||
        (timeinfo.tm_hour >= 12 && timeinfo.tm_hour < 13) ||
        (timeinfo.tm_hour >= 20 && timeinfo.tm_hour < 21)) {
        if (digitalRead(pirPin) == HIGH && !messagePrinted) {
            Serial.println("Motion detected!");
            sendMessageToTelegram("Motion detected!");
            delay(5000);
            fb = esp_camera_fb_get();
            if (fb) {
                sendPhotoToTelegram(fb->buf, fb->len);
                esp_camera_fb_return(fb);
            }
            messagePrinted = true;
        }
    } else {
        messagePrinted = false;
    }

    delay(1000);
}
