#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Adafruit_FONA.h>
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

char replybuffer[255];

#define CS 10
#define DC 8
#define RST 9
int potPin = A0;
int button = 7;

// Color definitions
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF


#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);


Adafruit_SSD1351 tft = Adafruit_SSD1351(CS, DC, RST);
String months[] = {
  "January", "February", "March", "April",
  "May", "June", "July", "August",
  "September", "October", "November", "December"
};

int prevPot = 0;
int buttonState = LOW;
uint8_t type;
bool temp = true;
int cs = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  fonaSerial->begin(4800);

  tft.begin();
  tft.setTextColor(WHITE, BLACK);
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  pinMode(button, OUTPUT);
  if (! fona.begin(*fonaSerial)) {
    tft.setCursor(0, 45);
    tft.println("Couldn't find\n\t FONA");
    while (1);
  }
  delay(10);
  type = fona.type();
  tft.setCursor(0, 10);
  switch (type) {
    case FONA800L:
      tft.println(F("FONA 800L")); break;
    case FONA800H:
      tft.println(F("FONA 800H")); break;
    case FONA808_V1:
      tft.println(F("FONA 808 (v1)")); break;
    case FONA808_V2:
      tft.println(F("FONA 808 (v2)")); break;
    case FONA3G_A:
      tft.println(F("FONA 3G (American)")); break;
    case FONA3G_E:
      tft.println(F("FONA 3G (European)")); break;
    default:
      tft.println(F("???")); break;
  }
  delay(1000);
  tft.setCursor(0, 24);
  if (!fona.enableNetworkTimeSync(true))
    tft.println(F("Failed to enable"));
  else
    tft.println(F("Network Synced"));
  delay(1000);
  tft.setCursor(0, 38);
  tft.fillScreen(BLACK);
}

void loop() {
  while (temp) {
    char command = Serial.read();
    Serial.println(command);
    delay(5000);
    printStack();
    temp = false;
  }

  int potVal = analogRead(potPin);
  buttonState = digitalRead(button);
  potVal = map(potVal, 0, 1024, 0, 8);
  if (buttonState == HIGH) {
    tft.setCursor(50, 0);
    tft.fillRect(50, 0, 50, 8, BLACK);

    if ( cs == 0) {
      switch (potVal) {
        case 1:
          tft.print("Call");
          cs = 1;
          calls();
          break;
        case 2:
          tft.print("SMS");
          break;
        case 3:
          tft.print("Time");
          break;
        case 4:
          tft.print("Settings");
          break;
        case 5:
          tft.print("Somthn ls");
          break;
        case 6:
          tft.print("Sixth");
          break;
        case 7:
          tft.print("LAST");
          break;
      }
    } else if (cs == 1) {
      switch (potVal) {
        case 1:

          inCall("Bennett", "0549169113");

          break;
        case 2:
          inCall("David", "0207836012");
          break;
        case 3:
          inCall("Barnabas", "0503292623");
          break;
        case 0:
          cs = 0;
          printStack();
          break;

      }
      calls();

    }


  }

  if (potVal != prevPot) {
    prevPot = potVal;
    tft.fillRect(0, 0, 96, 128, BLACK);
    //  tft.setCursor(0,potVal*16);
    switch (cs) {
      case 0:
        printStack();
        break;
      case 1:
        calls();
        break;
    }
    tft.drawRect(0, (potVal * 16) - 4, 90, 16, WHITE);
  }
  //  tft.setCursor(0, 0);
  //  tft.print(String(prevPot) + "     " + String(potVal));

  while (fona.getCallStatus() == 3) {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 32);
    tft.println("incoming call");
   

    buttonState = digitalRead(button);

    if (buttonState == HIGH){
      if (! fona.pickUp()) {
          tft.println(F("call ended"));
        } else {
          cs = 0;
          callStats();
        }
    }


  }

}

void printStack() {
  tft.setCursor(5, 16);
  tft.println("Call");
  tft.setCursor(5, 32);
  tft.println("SMS");
  tft.setCursor(5, 48);
  tft.println("Time");
  tft.setCursor(5, 64);
  tft.println("Settings");
  tft.setCursor(5, 80);
  tft.println("Something Else");
  tft.setCursor(5, 96);
  tft.println("Sixth");
  tft.setCursor(5, 112);
  tft.println("Last");

}

void calls() {
  tft.setCursor(0, 0);
  tft.setTextColor(RED, BLACK);
  tft.print("Back");
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(5, 16);
  tft.println("Bennett");
  tft.setCursor(5, 32);
  tft.println("David");
  tft.setCursor(5, 48);
  tft.println("Barnabas");
  tft.setCursor(5, 64);
  tft.println("Jorge");
  tft.setCursor(5, 80);
  tft.println("Hive");
  tft.setCursor(5, 96);
  tft.println("Awal");
  tft.setCursor(5, 112);
  tft.println("Kwesi");
}

void inCall(String namE, char num[30]) {
  tft.print(namE);
  tft.fillRect(0, 8, 96, 120, BLACK);
  tft.setCursor(0, 16);
  if (!fona.callPhone(num)) {
    tft.println(F("Failed"));
  } else {
    tft.println("Calling " + String(namE));
  } delay(400);
  callStats();

  tft.fillRect(0, 8, 96, 120, BLACK);
}
void callStats() {
  bool getMil = true;
  unsigned long millVal;
  while (fona.getCallStatus() == 4) {
    while (getMil) {
      millVal = millis();
      getMil = false;
      break;
    }
    tft.setCursor(3, 32);
    tft.print(String(((millis() - millVal) / 3600000) % 24) + ":" + String(((millis() - millVal) / 60000) % 60) + ":" + String(((millis() - millVal) / 1000) % 60));
    buttonState = digitalRead(button);
    if (buttonState == HIGH ) {
      tft.setCursor(3, 48);
      if (fona.hangUp()) {
        break;
      } else {
        Serial.println(F("Unable to End Call"));
      }

    }
  }
  tft.println(F("Call Ended"));
  delay(500);
}

