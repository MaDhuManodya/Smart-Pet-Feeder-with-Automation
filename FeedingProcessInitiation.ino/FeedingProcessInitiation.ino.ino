#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HX711_ADC.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>

// Replace with your WiFi credentials and Telegram bot token
const char *ssid = "Your_WiFi_SSID";
const char *password = "Your_WiFi_Password";
#define BOT_TOKEN "Your_Telegram_Bot_Token"
#define CHAT_ID "Your_Telegram_Chat_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

const int HX711_dout = 16;
const int HX711_sck = 4;
const int relayPin = 5;

HX711_ADC LoadCell(HX711_dout, HX711_sck);
const char *messageStatic = "Weight: ";

bool feeding = false;
bool weightSent = false;

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("SMART PET FEEDER");

    Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.println("Starting...");

    LoadCell.begin();
    unsigned long stabilizingtime = 2000;
    boolean _tare = true;
    LoadCell.start(stabilizingtime, _tare);

    if (LoadCell.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU > HX711 wiring and pin designations");
        while (1);
    } else {
        LoadCell.setCalFactor(316.18); // Adjust calibration factor as needed
        Serial.println("Startup is complete");
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

    myservo.attach(13);
    pinMode(relayPin, OUTPUT);
}

void loop() {
    int feedCount = 0;
    digitalWrite(relayPin, LOW);

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    for (int i = 0; i < numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        if (text == "/feed" && !feeding) {
            feeding = true;
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Start feeding");
            bot.sendMessage(CHAT_ID, "Start feeding", "");

            // Feeding process 7 times
            while (feedCount < 7) {
                myservo.write(40); // Adjust servo angle as needed
                delay(500);
                myservo.write(0);
                delay(500);
                feedCount++;
            }

            feedCount = 0;

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Feeding finished");
            bot.sendMessage(CHAT_ID, "Feeding finished", "");

            float weight = LoadCell.getData();

            String weightMessage = "Weight after feeding: " + String(weight);
            bot.sendMessage(CHAT_ID, weightMessage, "");

            weightSent = false;

            delay(10000);

            digitalWrite(relayPin, HIGH);
            delay(50000);

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Water pumping");
            bot.sendMessage(CHAT_ID, "Water pumping", "");
            lcd.print("Water pumping finished");
            bot.sendMessage(CHAT_ID, "Water pumping finished", "");

            digitalWrite(relayPin, LOW);

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Water not pumping");

            feeding = false;
        }
    }

    if (LoadCell.update()) {
        float weight = LoadCell.getData();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(messageStatic);
        lcd.print(weight);

        if (!weightSent) {
            String weightMessage = "Weight: " + String(weight);
            bot.sendMessage(CHAT_ID, weightMessage, "");
            weightSent = true;
        }

        lcd.setCursor(0, 1);
        lcd.print("Smart Pet Feeder");
    }

    if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') {
            LoadCell.tareNoDelay();
        }
    }

    if (LoadCell.getTareStatus() == true) {
        Serial.println("Tare complete");
    }
}
