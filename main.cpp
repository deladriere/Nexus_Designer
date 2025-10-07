#include "OLedI2C.h"
#include <Arduino.h>
#include <MIDI.h>
#include <USB-MIDI.h>
#include <Wire.h>
#include <ctype.h>
/*
#hardware
 ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄
▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌
▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌ ▐░▌
▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌
▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌ ▐░▌▐░▌
▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌       ▐░▌▐░▌   ▄
▐░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌  ▐░▌
▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░▌       ▐░▌▐░▌ ▐░▌░▌
▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌     ▐░▌
▐░▌       ▐░▌▐░▌▐░▌ ▐░▌▐░▌▐░▌       ▐░▌▐░▌     ▐░▌  ▐░▌ ▐░▌       ▐░▌▐░▌ ▐░▌▐░▌
▐░▌ ▐░█▄▄▄▄▄▄▄█░▌▐░▌░▌   ▐░▐░▌▐░▌       ▐░▌▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌ ▐░▌▐░▌
▐░▌▐░▌       ▐░▌▐░░░░░░░░░░▌ ▐░░▌     ▐░░▌▐░▌       ▐░▌▐░▌ ▐░▌▐░░░░░░░░░░░▌ ▀ ▀
▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀       ▀▀  ▀         ▀  ▀         ▀
▀▀▀▀▀▀▀▀▀▀▀

*/

#define GREEN_LED 12
#define RED_LED 13

#define RSTN 4
#define TALKING 11

/* MIKROBUS PORT
                           _____________
                          |             |      AUDIO
                  !RST  4 |             |  11  TALKING
                  SEN     |             |  31  RX
                  SCK     |             |  30  TX
                  MISO    |             |      SCL
                  MOSI    |             |      SDA
                  3V3     |             |      5V
                  GND     |             |      GND
                          |_____________/
*/

/* #object
▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
▄▄▄▄▄▄▄▄▄▄▄
▐░░░░░░░░░░░▌▐░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▀▀▀▀▀█░█▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌    ▐░▌          ▐░▌ ▐░▌     ▐░▌
▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌    ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌               ▐░▌
▐░█▄▄▄▄▄▄▄▄▄ ▐░▌       ▐░▌▐░░░░░░░░░░▌      ▐░▌    ▐░░░░░░░░░░░▌▐░▌ ▐░▌
▐░░░░░░░░░░░▌ ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌     ▐░▌    ▐░█▀▀▀▀▀▀▀▀▀ ▐░▌ ▐░▌
▀▀▀▀▀▀▀▀▀█░▌ ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌    ▐░▌          ▐░▌ ▐░▌ ▐░▌
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▄▄▄▄▄█░▌    ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄      ▐░▌
▄▄▄▄▄▄▄▄▄█░▌ ▐░░░░░░░░░░░▌▐░░░░░░░░░░▌▐░░░░░░░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░▌     ▐░░░░░░░░░░░▌ ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀      ▀▀▀▀▀▀▀▀▀▀▀
▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀
*/

auto &Nexus = Serial5;

// USB MIDI instance and HW Serial MIDI instance for callbacks
USBMIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDISER);

/*
 ▄               ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
▄▄▄▄▄▄▄▄▄▄   ▄            ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ ▐░▌
▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌ ▐░▌           ▐░▌ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌
▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░▌
▐░▌  ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌ ▐░▌
▐░▌ ▐░▌       ▐░▌   ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌          ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌     ▐░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌ ▐░▌   ▐░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀      ▐░▌
▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀█░▌ ▐░▌ ▐░▌ ▐░▌
▐░▌▐░▌     ▐░▌       ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          ▐░▌ ▐░▌
       ▐░▐░▌       ▐░▌       ▐░▌▐░▌      ▐░▌  ▄▄▄▄█░█▄▄▄▄ ▐░▌
▐░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄█░▌ ▐░▌        ▐░▌ ▐░▌▐░▌
▐░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌ ▀          ▀         ▀  ▀         ▀
▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀

*/

#define VERSION "1.0"

// LCD globals
OLedI2C LCD;
bool displayPresent = false;

// I2C device detection
static bool checkI2CDevice(uint8_t address) {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("Display found");
    return true;
  } else {
    Serial.println("Display NOT found");
    return false;
  }
}

/*#function
 ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄        ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
▄▄▄▄▄▄▄▄▄▄▄  ▄▄        ▄  ▄▄▄▄▄▄▄▄▄▄▄ ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░▌
▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░▌ ▐░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌░▌     ▐░▌▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀  ▀▀▀▀█░█▀▀▀▀
▐░█▀▀▀▀▀▀▀█░▌▐░▌░▌     ▐░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░▌          ▐░▌       ▐░▌▐░▌▐░▌ ▐░▌▐░▌
▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌▐░▌    ▐░▌▐░▌ ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌       ▐░▌▐░▌
▐░▌   ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌ ▐░▌
▐░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░▌               ▐░▌ ▐░▌
▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░░░░░░░░░░░▌ ▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌   ▐░▌
▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌   ▐░▌ ▐░▌
▀▀▀▀▀▀▀▀▀█░▌ ▐░▌          ▐░▌       ▐░▌▐░▌    ▐░▌▐░▌▐░▌               ▐░▌ ▐░▌
▐░▌       ▐░▌▐░▌    ▐░▌▐░▌          ▐░▌ ▐░▌          ▐░█▄▄▄▄▄▄▄█░▌▐░▌
▐░▐░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌      ▄▄▄▄█░█▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░▌     ▐░▐░▌
▄▄▄▄▄▄▄▄▄█░▌ ▐░▌          ▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░░░░░░░░░░░▌     ▐░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░░░░░░░░░░░▌ ▀            ▀▀▀▀▀▀▀▀▀▀▀  ▀
▀▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀▀▀▀▀▀▀▀▀▀▀

*/

// Sentence storage/parsing
#define MAX_SEGMENTS 12
#define MAX_SEGMENT_LENGTH 256
static char mytextBuffer[1024] = "";
static char parsedSentences[MAX_SEGMENTS][MAX_SEGMENT_LENGTH];
static int totalSegments = 0;
static bool startSpeech = false;

// Nexus voices (indices 0..10)
static const char *nexusVoices[] = {"Perfect Paul", "Vader",          "Big Bob",
                                    "Precise Pete", "Ricochet Randy", "Biff",
                                    "Skip",         "Robo Robert",    "Goliath",
                                    "Alvin",        "Gretchen"};
static const int nexusVoicesCount =
    sizeof(nexusVoices) / sizeof(nexusVoices[0]);
static int currentVoiceIndex = 0;
static int currentSpeed = 0;
static int currentReverb = 0;
static int currentFormant = 50;
static int currentPitch = 50;
static int currentExpression = 5;
static int currentVolume = 5;

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity)

{
  // Serial.print("Channel: ");
  // Serial.print(inChannel);
  // Serial.print("\tMIDI USB NoteOn  ");
  // Serial.print(inNumber);
  // Serial.print("\tvelocity: ");
  // Serial.println(inVelocity);
  digitalWrite(13, HIGH);
  inNumber = constrain(inNumber, 36, 48);
  const char *sentence = parsedSentences[inNumber - 36];

  if (displayPresent) {
    LCD.clearLcd();
    LCD.SendWrapped(sentence, 0, 0);
  }

  // Send the stored sentence to the Nexus serial
  Nexus.println(sentence);
}
void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity) {
  // Serial.print("Channel: ");
  // Serial.print(inChannel);
  // Serial.print("\tMIDI USB NoteOff ");
  // Serial.print(inNumber);
  // Serial.print("\tvelocity: ");
  // Serial.println(inVelocity);
  digitalWrite(13, LOW);
}

static void displayHelp() {
  Serial.println("/h = display help");
  Serial.println("/a = about this code");
  Serial.println("/c = clear screen");
  Serial.println("/ex = set expression (0-9)");
  Serial.println("/f = firmware version");
  Serial.println("/i = info about current voice and speed settings");
  Serial.println("/m = show MIDI mapping note per sentence");
  Serial.println("/o = original voice settings");
  Serial.println("/pxx = set pitch (0-99)");
  Serial.println("/rx = set reverb (0-9)");
  Serial.println("/sxx = set speed (0-13)");
  Serial.println("/txx = set formant (0-99)");
  Serial.println("/ux = set volume (0-9)");
  Serial.println(
      "/vxx = set voice (0-10) (Perfect Paul, Vader, Big Bob, Precise Pete, "
      "Ricochet Randy, Biff, Skip, Robo Robert, Goliath, Alvin, Gretchen)");
  Serial.println(
      "enter = type your text then press enter to start speech \n\tuse @ as "
      "MIDI "
      "note delimiter; start the line with @ to store: @one@two@three");
}

static void debugParsedSentences() {
  Serial.println("Parsed segments:");
  for (int i = 0; i < totalSegments; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(parsedSentences[i]);
  }
}

static void parseInputText(const char *inputText) {
  totalSegments = 0; // Reset segment count

  // Clear previous sentences to avoid stale values on shorter updates
  for (int i = 0; i < MAX_SEGMENTS; i++) {
    parsedSentences[i][0] = '\0';
  }

  // Tokenize the input string using '@' as the delimiter
  char local[sizeof(mytextBuffer)];
  strncpy(local, inputText, sizeof(local) - 1);
  local[sizeof(local) - 1] = '\0';

  char *token = strtok(local, "@");
  while (token != nullptr && totalSegments < MAX_SEGMENTS) {
    // Skip empty tokens (handles leading/trailing '@')
    if (token[0] != '\0') {
      // Trim leading '@' and whitespace just in case
      const char *cleanStart = token;
      while (*cleanStart == '@' || isspace((unsigned char)*cleanStart)) {
        cleanStart++;
      }

      // Copy the token into the array, ensuring it doesn't overflow
      strncpy(parsedSentences[totalSegments], cleanStart,
              MAX_SEGMENT_LENGTH - 1);
      parsedSentences[totalSegments][MAX_SEGMENT_LENGTH - 1] = '\0';

      // Trim trailing whitespace and stray '@'
      size_t len = strlen(parsedSentences[totalSegments]);
      while (len > 0) {
        char c = parsedSentences[totalSegments][len - 1];
        if (c == '@' || isspace((unsigned char)c)) {
          parsedSentences[totalSegments][len - 1] = '\0';
          len--;
        } else {
          break;
        }
      }

      // Only count non-empty results after trimming
      if (len > 0) {
        totalSegments++;
      }
    }

    // Get the next token
    token = strtok(nullptr, "@");
  }
}

// forward decls
void splash();

void getCommand() {
  static char inputBuffer[1024] = "";
  static int bufferIndex = 0;

  while (Serial.available()) {
    char receivedChar = Serial.read();

    if (receivedChar == '\n' || receivedChar == '\r') {
      if (bufferIndex == 0) {
        if (strlen(mytextBuffer) > 0) {
          startSpeech = true;
        }
        continue;
      }

      inputBuffer[bufferIndex] = '\0';

      if (inputBuffer[0] == '/') {
        char command[8] = {0};
        char parameter[256] = "";
        sscanf(inputBuffer, "/%7s %[^\n]", command, parameter);

        if (strcmp(command, "h") == 0) {
          Serial.print("\033[2J");
          Serial.print("\033[;H");
          displayHelp();
        } else if (strcmp(command, "a") == 0) {
          Serial.print("\033[2J");
          Serial.print("\033[;H");
          splash();
        } else if (strcmp(command, "m") == 0) {
          Serial.println();
          Serial.println("MIDI settings:");
          for (int i = 0; i < totalSegments; i++) {
            Serial.print("MIDI Note: ");
            Serial.print(36 + i);
            Serial.print(": ");
            Serial.println(parsedSentences[i]);
          }
        } else if (strcmp(command, "c") == 0) {
          Serial.print("\033[2J");
          Serial.print("\033[;H");
        } else if (strncmp(command, "e", 1) == 0 &&
                   strcmp(command, "ex") != 0) {
          // /e or /e9 style
          const char *numStr = nullptr;
          if (strcmp(command, "e") == 0) {
            numStr = parameter;
          } else {
            numStr = command + 1;
          }
          if (numStr == nullptr || *numStr == '\0') {
            Serial.println("Usage: /e x   where x is 0..9");
          } else {
            int expression = atoi(numStr);
            if (expression < 0)
              expression = 0;
            if (expression > 9)
              expression = 9;
            currentExpression = expression;
            char buffer[8];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%de", expression);
            Nexus.write(0x7);
            Nexus.println(buffer);
            delay(3);
            Serial.println();
            Serial.print("Expression set to: ");
            Serial.println(expression);
          }
        } else if (strcmp(command, "f") == 0) {
          Serial.println();
          Serial.println(VERSION);
        } else if (strcmp(command, "i") == 0) {
          Serial.println();
          Serial.print("Current Voice: ");
          Serial.print(currentVoiceIndex);
          Serial.print(":");
          if (currentVoiceIndex >= 0 && currentVoiceIndex < nexusVoicesCount)
            Serial.print(nexusVoices[currentVoiceIndex]);
          else
            Serial.print("(unknown)");
          Serial.print(" - Speed: ");
          Serial.print(currentSpeed);
          Serial.print(" - Reverb: ");
          Serial.print(currentReverb);
          Serial.print(" - Formant: ");
          Serial.print(currentFormant);
          Serial.print(" - Pitch: ");
          Serial.print(currentPitch);
          Serial.print(" - Expression: ");
          Serial.print(currentExpression);
          Serial.print(" - Volume: ");
          Serial.println(currentVolume);
        } else if (strcmp(command, "o") == 0) {
          // Reset to original voice settings (voice 0, speed 6, reverb 0,
          // formant 50, pitch 50, expression 5, volume 5)
          currentVoiceIndex = 0;
          currentSpeed = 6;
          currentReverb = 0;
          currentFormant = 50;
          currentPitch = 50;
          currentExpression = 5;
          currentVolume = 5;

          // Set voice to 0
          char voiceCmd[4];
          sprintf(voiceCmd, "%do", currentVoiceIndex);
          Nexus.write(0x07);
          Nexus.println(voiceCmd);
          delay(3);

          // Set speed to 6
          char speedBuffer[8];
          memset(speedBuffer, 0, sizeof(speedBuffer));
          sprintf(speedBuffer, "%ds", currentSpeed);
          Nexus.write(0x7);
          Nexus.println(speedBuffer);
          delay(3);

          // Set reverb to 0
          char reverbBuffer[8];
          memset(reverbBuffer, 0, sizeof(reverbBuffer));
          sprintf(reverbBuffer, "%dr", currentReverb);
          Nexus.write(0x7);
          Nexus.println(reverbBuffer);
          delay(3);

          // Set formant to 50
          char formantBuffer[8];
          memset(formantBuffer, 0, sizeof(formantBuffer));
          sprintf(formantBuffer, "%df", currentFormant);
          Nexus.write(0x7);
          Nexus.println(formantBuffer);
          delay(3);

          // Set pitch to 50
          char pitchBuffer[8];
          memset(pitchBuffer, 0, sizeof(pitchBuffer));
          sprintf(pitchBuffer, "%dp", currentPitch);
          Nexus.write(0x7);
          Nexus.println(pitchBuffer);
          delay(3);

          // Set expression to 5
          char expressionBuffer[8];
          memset(expressionBuffer, 0, sizeof(expressionBuffer));
          sprintf(expressionBuffer, "%de", currentExpression);
          Nexus.write(0x7);
          Nexus.println(expressionBuffer);
          delay(3);

          // Set volume to 5
          char volumeBuffer[8];
          memset(volumeBuffer, 0, sizeof(volumeBuffer));
          sprintf(volumeBuffer, "%dv", currentVolume);
          Nexus.write(0x7);
          Nexus.println(volumeBuffer);
          delay(3);

          Serial.println();
          Serial.println(
              "Voice settings reset to original (Voice 0, Speed 6, "
              "Reverb 0, Formant 50, Pitch 50, Expression 5, Volume 5)");
        } else if (strncmp(command, "p", 1) == 0 &&
                   strcmp(command, "px") != 0) {
          // /p or /p50 style
          const char *numStr = nullptr;
          if (strcmp(command, "p") == 0) {
            numStr = parameter;
          } else {
            numStr = command + 1;
          }
          if (numStr == nullptr || *numStr == '\0') {
            Serial.println("Usage: /p x   where x is 0..99");
          } else {
            int pitch = atoi(numStr);
            if (pitch < 0)
              pitch = 0;
            if (pitch > 99)
              pitch = 99;
            currentPitch = pitch;
            char buffer[8];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%dp", pitch);
            Nexus.write(0x7);
            Nexus.println(buffer);
            delay(3);
            Serial.println();
            Serial.print("Pitch set to: ");
            Serial.println(pitch);
          }
        } else if (strncmp(command, "r", 1) == 0 &&
                   strcmp(command, "rx") != 0) {
          // /r or /r5 style
          const char *numStr = nullptr;
          if (strcmp(command, "r") == 0) {
            numStr = parameter;
          } else {
            numStr = command + 1;
          }
          if (numStr == nullptr || *numStr == '\0') {
            Serial.println("Usage: /r x   where x is 0..9");
          } else {
            int reverb = atoi(numStr);
            if (reverb < 0)
              reverb = 0;
            if (reverb > 9)
              reverb = 9;
            currentReverb = reverb;
            char buffer[8];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%dr", reverb);
            Nexus.write(0x7);
            Nexus.println(buffer);
            delay(3);
            Serial.println();
            Serial.print("Reverb set to: ");
            Serial.println(reverb);
          }
        } else if (strncmp(command, "s", 1) == 0 &&
                   strcmp(command, "sx") != 0) {
          // /s or /s5 style
          const char *numStr = nullptr;
          if (strcmp(command, "s") == 0) {
            numStr = parameter;
          } else {
            numStr = command + 1;
          }
          if (numStr == nullptr || *numStr == '\0') {
            Serial.println("Usage: /s x   where x is 0..13");
          } else {
            int speed = atoi(numStr);
            if (speed < 0)
              speed = 0;
            if (speed > 13)
              speed = 13;
            currentSpeed = speed;
            char buffer[8];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%ds", speed);
            Nexus.write(0x7);
            Nexus.println(buffer);
            delay(3);
            Serial.println();
            Serial.print("Speed set to: ");
            Serial.println(speed);
          }
        } else if (strncmp(command, "t", 1) == 0 &&
                   strcmp(command, "tx") != 0) {
          // /t or /t50 style
          const char *numStr = nullptr;
          if (strcmp(command, "t") == 0) {
            numStr = parameter;
          } else {
            numStr = command + 1;
          }
          if (numStr == nullptr || *numStr == '\0') {
            Serial.println("Usage: /t x   where x is 0..99");
          } else {
            int formant = atoi(numStr);
            if (formant < 0)
              formant = 0;
            if (formant > 99)
              formant = 99;
            currentFormant = formant;
            char buffer[8];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%df", formant);
            Nexus.write(0x7);
            Nexus.println(buffer);
            delay(3);
            Serial.println();
            Serial.print("Formant set to: ");
            Serial.println(formant);
          }
        } else if (strncmp(command, "u", 1) == 0 &&
                   strcmp(command, "ux") != 0) {
          // /u or /u5 style
          const char *numStr = nullptr;
          if (strcmp(command, "u") == 0) {
            numStr = parameter;
          } else {
            numStr = command + 1;
          }
          if (numStr == nullptr || *numStr == '\0') {
            Serial.println("Usage: /u x   where x is 0..9");
          } else {
            int volume = atoi(numStr);
            if (volume < 0)
              volume = 0;
            if (volume > 9)
              volume = 9;
            currentVolume = volume;
            char buffer[8];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%dv", volume);
            Nexus.write(0x7);
            Nexus.println(buffer);
            delay(3);
            Serial.println();
            Serial.print("Volume set to: ");
            Serial.println(volume);
          }
        } else if (strncmp(command, "v", 1) == 0 &&
                   strcmp(command, "vx") != 0) {
          // /v or /v5 style
          const char *numStr = nullptr;
          if (strcmp(command, "v") == 0) {
            numStr = parameter;
          } else {
            numStr = command + 1;
          }
          if (numStr == nullptr || *numStr == '\0') {
            Serial.println("Usage: /v x   where x is 0..10");
          } else {
            int v = atoi(numStr);
            if (v < 0)
              v = 0;
            if (v > 10)
              v = 10;
            currentVoiceIndex = v;
            char cmd[4];
            sprintf(cmd, "%do", v); // e.g., "5o"
            Nexus.write(0x07);
            Nexus.println(cmd);
            delay(3);
            Serial.println();
            Serial.print("Voice set to ");
            Serial.print(v);
            Serial.print(":");
            if (v >= 0 && v < nexusVoicesCount)
              Serial.println(nexusVoices[v]);
            else
              Serial.println("(unknown)");
          }
        } else if (strcmp(command, "vx") == 0) {
          Serial.println();
          Serial.println("Voices:");
          for (int i = 0; i < nexusVoicesCount; i++) {
            Serial.print(i);
            Serial.print(": ");
            Serial.println(nexusVoices[i]);
          }
        } else {
          Serial.print("Unrecognized command: /");
          Serial.println(command);
        }
      } else {
        strncpy(mytextBuffer, inputBuffer, sizeof(mytextBuffer) - 1);
        mytextBuffer[sizeof(mytextBuffer) - 1] = '\0';

        // Only parse and store to MIDI notes if input starts with '@'
        if (mytextBuffer[0] == '@') {
          parseInputText(mytextBuffer);
          Serial.println(); // Add newline before debug output
          debugParsedSentences();
          startSpeech = true;
        } else {
          // Do not store to MIDI notes; just speak the raw input once
          if (displayPresent) {
            LCD.clearLcd();
            LCD.SendWrapped(mytextBuffer, 0, 0);
          }
          Nexus.println(mytextBuffer);
          Serial.println(); // Add newline only for non-@ input
          // Don't set startSpeech for immediate speech
        }
      }

      memset(inputBuffer, 0, sizeof(inputBuffer));
      bufferIndex = 0;
      Serial.print("$ ");
    } else if (bufferIndex < (int)sizeof(inputBuffer) - 1) {
      Serial.print(receivedChar);
      inputBuffer[bufferIndex++] = receivedChar;
    }
  }
}

void Talking() { digitalWrite(GREEN_LED, digitalRead(TALKING)); }

void NEXUS_Reset() {
  digitalWrite(RSTN, LOW);
  delay(2); // datasheet say Min 1 ms
  digitalWrite(RSTN, HIGH);
}

void initPins() {
  // led
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  // nexus

  pinMode(RSTN, OUTPUT);
  pinMode(TALKING, INPUT_PULLUP);
  digitalWrite(RSTN, HIGH);
}

void splash() {
  Serial.println(" _        _______                    _______ ");
  Serial.println("( (    /|(  ____ \\|\\     /||\\     /|(  ____ \\");
  Serial.println("|  \\  ( || (    \\/( \\   / )| )   ( || (    \\/");
  Serial.println("|   \\ | || (__     \\ (_) / | |   | || (_____ ");
  Serial.println("| (\\ \\) ||  __)     ) _ (  | |   | |(_____  )");
  Serial.println("| | \\   || (       / ( ) \\ | |   | |      ) |");
  Serial.println("| )  \\  || (____/\\( /   \\ )| (___) |/\\____) |");
  Serial.println("|/    )_)(_______/|/     \\|(_______)\\_______)");
  Serial.println("                                             ");
  Serial.println();
  Serial.println("Nexus Vax serial MIDI command interface");
  Serial.println("                          by Polaxis");
  Serial.print("Version ");
  Serial.println(VERSION);
}

// the setup function runs once when you press reset or power the board
void setup() {
  initPins();
  // initialize digital pin LED_BUILTIN as an output.
  NEXUS_Reset();
  attachInterrupt(digitalPinToInterrupt(TALKING), Talking, CHANGE);

  // I2C for OLED
  Wire.begin();

  Serial.begin(115200);
  splash();
  while (Serial.available()) {
    Serial.read();
  }
  Serial.print("$ ");

  Nexus.begin(115200);
  Nexus.println(
      "Welcome to the Nexus MIDI"); // whatever this is to sample the baud rate
  delay(100);

  // Detect and initialize OLED display (0x3C)
  displayPresent = checkI2CDevice(0x3C);
  if (displayPresent) {
    LCD.init();
    LCD.airportString("NEXUS SER. MIDI", 0, 0, 40);
    LCD.sendString("Version", 0, 1);
    LCD.sendString(VERSION, 8, 1);
  }

  Nexus.write(0x1);
  Nexus.println("148G"); // acctivate realtime control
  Nexus.println("Welcome to Nexus Serial MIDI");
  delay(100);

  Nexus.write(0x1);  // change command to \a (07)
  Nexus.write(0x07); //

  delay(10);
  Nexus.print("Version");
  Nexus.println(VERSION);

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  MIDISER.begin(MIDI_CHANNEL_OMNI);
  MIDISER.setHandleNoteOn(handleNoteOn);
  MIDISER.setHandleNoteOff(handleNoteOff);
}

// the loop function runs over and over again forever
void loop() {
  getCommand();
  if (startSpeech && totalSegments > 0) {
    // Parse and store segments for MIDI mapping only (disabled: keep storage
    // for MIDI only) for (int i = 0; i < totalSegments; i++) {
    //   const char *sentence = parsedSentences[i];
    //   if (displayPresent) {
    //     LCD.clearLcd();
    //     LCD.SendWrapped(sentence, 0, 0);
    //   }
    //   Nexus.println(sentence);
    // }
    startSpeech = false;
  }
  MIDI.read();
  MIDISER.read();
  // if (Nexus.available()) {
  //   int inByte = Nexus.read();
  //   Serial.write(inByte);
  // }

  // if (Serial.available()) {
  //   int inByte = Serial.read();
  //   if (inByte == 36) // $
  //   {
  //     Nexus.write(0x7);
  //   } else {
  //     Nexus.write(inByte);
  //   }
  // }
}