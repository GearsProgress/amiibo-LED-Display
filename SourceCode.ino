/*
   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

#define UPPER_PAGE 21
#define LOWER_PAGE 22

#define BUTTON_PIN 53

#define PHOTO1_PIN A0
#define PHOTO2_PIN A1
#define PHOTO3_PIN A2

#define MOSI_PIN 51  // static, Blue
#define MISO_PIN 50  // static, Yellow
#define SCK_PIN 52   // static, Green

#define NFC1_RST_PIN 46  // White
#define NFC1_SDA_PIN 49  // Grey

#define NFC2_RST_PIN 45  // Orange
#define NFC2_SDA_PIN 48  // Brown

#define NFC3_RST_PIN 44  // Light Purple
#define NFC3_SDA_PIN 47  // Dark Purple

#define LED_DATA_PIN1 5
#define LED_DATA_PIN2 4
#define LED_DATA_PIN3 3

#define NUM_AMIIBO 97
#define AMIIBO_HEAD 0
#define AMIIBO_TAIL 1
#define AMIIBO_COLOR 2

#define SLOT_FILLED true
#define SLOT_EMPTY false
#define AMIIBO_PLACED 1
#define AMIIBO_REMOVED 2

const long AMIIBO_DATABASE[NUM_AMIIBO][3] = {
  { 0x00000000, 0x00000002, 0xFF3F25 },  //Mario
  { 0x00080000, 0x00030002, 0xFFDD43 },  //Donkey Kong
  { 0x01000000, 0x00040002, 0x00A0DF },  //Link
  { 0x05C00000, 0x00060002, 0x002A8B },  //Samus
  { 0x05C30000, 0x03800002, 0x100097 },  //Dark Samus
  { 0x00030000, 0x00020002, 0x6DFF32 },  //Yoshi
  { 0x1F000000, 0x000A0002, 0xFFD5E2 },  //Kirby
  { 0x05800000, 0x00050002, 0x1077FE },  //Fox
  { 0x19190000, 0x00090002, 0xFEB912 },  //Pikachu
  { 0x00010000, 0x000C0002, 0x3EE800 },  //Luigi
  { 0x22800000, 0x002C0002, 0xFF211C },  //Ness
  { 0x06000000, 0x00120002, 0x6E66FF },  //Captian Falcon
  { 0x19270000, 0x00260002, 0xFA92FE },  //Jigglypuff
  { 0x00020000, 0x00010002, 0xFFBFF7 },  //Peach
  { 0x00130000, 0x037A0002, 0xFFC11D },  //Daisy
  { 0x00050000, 0x00140002, 0x00B863 },  //Bowser
  { 0x078F0000, 0x03810002, 0x9FC9FF },  //Ice Climbers
  { 0x01010100, 0x00170002, 0x4B40FF },  //Shiek
  { 0x01010000, 0x000E0002, 0xFFEBA2 },  //Zelda
  { 0x00000100, 0x00190002, 0xFE5556 },  //Dr. Mario
  { 0x19AC0000, 0x03850002, 0xFDFA00 },  //Pichu
  { 0x05810000, 0x001C0002, 0x47B7FF },  //Falco
  { 0x21000000, 0x000B0002, 0x25BEFF },  //Marth
  { 0x21020000, 0x00290002, 0x45ABFF },  //Lucina
  { 0x01000000, 0x037C0002, 0xAFFF31 },  //Young Link
  { 0x01020100, 0x001B0002, 0x3019FF },  //Ganondorf
  { 0x19960000, 0x023D0002, 0x7C41FF },  //Mewtwo
  { 0x21040000, 0x02520002, 0x53FE12 },  //Roy
  { 0x21080000, 0x03880002, 0x8383FF },  //Chrom
  { 0x07800000, 0x002D0002, 0xFFE035 },  //Mr. Game and Watch
  { 0x1F010000, 0x00270002, 0x0339FF },  //Meta Knight
  { 0x07400000, 0x00100002, 0xA6DAFF },  //Pit
  { 0x07410000, 0x00200002, 0x7F1BFF },  //Dark Pit
  { 0x05C00100, 0x001D0002, 0x0057C4 },  //Zero Suit Samus
  { 0x00070000, 0x001A0002, 0xFFE71E },  //Wario
  { 0x37800000, 0x038A0002, 0x008AFD },  //Snake
  { 0x21010000, 0x00180002, 0xF42200 },  //Ike
  { 0x1D400000, 0x03870002, 0xFFC003 },  //Pokemon Trainer
  { 0x19070000, 0x03840002, 0x8EF1FE },  //Squirtle
  { 0x19020000, 0x03830002, 0x0DFE77 },  //Ivysaur
  { 0x19060000, 0x00240002, 0xFF5F1A },  //Charizard
  { 0x00090000, 0x000D0002, 0xFF3E32 },  //Diddy Kong
  { 0x22810000, 0x02510002, 0xFE5B14 },  //Lucas
  { 0x32000000, 0x00300002, 0x5199FE },  //Sonic
  { 0x1F020000, 0x00280002, 0xFFEA78 },  //King Dedede
  { 0x06400100, 0x001E0002, 0xCAFFA0 },  //Olimar
  { 0x1AC00000, 0x00110002, 0xB5E3FF },  //Lucario
  { 0x07810000, 0x00330002, 0x70BDFF },  //ROB (NES)
  { 0x07810000, 0x002E0002, 0x54BCFF },  //ROB (Famicom)
  { 0x01000100, 0x00160002, 0x74FF70 },  //Toon Link
  { 0x05840000, 0x037E0002, 0xFF9999 },  //Wolf
  { 0x01800000, 0x00080002, 0x2BFF6A },  //Villager
  { 0x34800000, 0x00310002, 0x66CAFF },  //Mega Man
  { 0x34800000, 0x02580002, 0xFFEF66 },  //Mega Man (Gold)
  { 0x07000000, 0x00070002, 0x86FF56 },  //Wii Fit Trainer
  { 0x00040100, 0x00130002, 0x6EFFEB },  //Rosalina and Luma
  { 0x06C00000, 0x000F0002, 0x00EC63 },  //Little Mac
  { 0x1B920000, 0x00250002, 0x005CFB },  //Greninja
  { 0x07C00000, 0x00210002, 0xFF4529 },  //Mii Brawler
  { 0x07C00100, 0x00220002, 0x22D9FF },  //Mii Swordfighter
  { 0x07C00200, 0x00230002, 0xFF9344 },  //Mii Gunner
  { 0x07420000, 0x001F0002, 0x77FFC4 },  //Palutena
  { 0x33400000, 0x00320002, 0xFFB53E },  //Pac-Man
  { 0x21030000, 0x002A0002, 0xD461FF },  //Robin
  { 0x22400000, 0x002B0002, 0xFF3A60 },  //Shulk
  { 0x06400100, 0x001E0002, 0xCAFFA0 },  //Alph
  { 0x00060000, 0x00150002, 0x3CF700 },  //Bowser Jr.
  { 0x07820000, 0x002F0002, 0xC65400 },  //Duck Hunt
  { 0x34C00000, 0x02530002, 0xFF231E },  //Ryu
  { 0x34C10000, 0x03890002, 0xAAE2FF },  //Ken
  { 0x36000000, 0x02590002, 0x00D3AD },  //Cloud
  { 0x36000100, 0x03620002, 0x00AB8C },  //Cloud P2
  { 0x21050000, 0x025A0002, 0x23C4FF },  //Corrin
  { 0x21050100, 0x03630002, 0x09CEFE },  //Corrin P2
  { 0x32400000, 0x025B0002, 0x8982FF },  //Bayonetta
  { 0x32400100, 0x03640002, 0x8871FF },  //Bayonetta P2
  { 0x08000100, 0x03820002, 0xFF1E7A },  //Inkling
  { 0x05C20000, 0x037F0002, 0xB500AD },  //Ridley
  { 0x37C00000, 0x038B0002, 0x840800 },  //Simon
  { 0x37C10000, 0x038C0002, 0x006DDA },  //Richter
  { 0x00C00000, 0x037B0002, 0x6BC200 },  //King K. Rool
  { 0x01810000, 0x037D0002, 0xE0FE52 },  //Isabelle
  { 0x1BD70000, 0x03860002, 0xFFD901 },  //Incineroar
  { 0x00240000, 0x038D0002, 0x00AA82 },  //Piranah Plant
  { 0x3A000000, 0x03A10002, 0xB51100 },  //Joker
  { 0x36400000, 0x03A20002, 0xC9A7FF },  //Hero
  { 0x3B400000, 0x03A30002, 0xFFDE73 },  //Banjo and Kazooie
  { 0x3C800000, 0x03A40002, 0x6EB5FF },  //Terry
  { 0x210B0000, 0x03A50002, 0xAFFFC9 },  //Byleth
  { 0x0A400000, 0x041D0002, 0xD5D583 },  //Min Min
  { 0x3DC00000, 0x04220002, 0x8DBBD9 },  //Steve
  { 0x3DC10000, 0x04230002, 0x8DBBD9 },  //Alex
  { 0x36010000, 0x04210002, 0x246C6C },  //Sephiroth
  { 0x33C00000, 0x04200002, 0x7ED4BC },  //Pyra
  { 0x22410000, 0x041E0002, 0x7ED4BC },  //Mythra
  { 0x22420000, 0x041F0002, 0xA43E37 },  //Kazuya
  { 0x3F000000, 0x042E0002, 0xFCF5F5 },  //Sora
};

class photoresistor {
public:
  int pin;
  int high_threshold;
  int low_threshold;
  bool curr_state;

  photoresistor(int nPin) {
    pinMode(nPin, INPUT);
    curr_state = SLOT_EMPTY;
    pin = nPin;
  };

  void set_threshold() {
    high_threshold = analogRead(pin) * 0.70;
    low_threshold = analogRead(pin) * 0.40;
    Serial.print("Threshold: ");
    Serial.print(low_threshold);
    Serial.print("-");
    Serial.println(high_threshold);
    delay(300);
  }

  int get_slot_state() {
    int val = analogRead(pin);
    if (val < low_threshold) {  // Dark - slot is filled
      if (curr_state == SLOT_FILLED) {
        return 0;
      } else {
        curr_state = SLOT_FILLED;
        return AMIIBO_PLACED;
      }
    } else if (val > high_threshold) {  // Light - slot is not filled
      if (curr_state == SLOT_EMPTY) {
        return 0;
      } else {
        curr_state = SLOT_EMPTY;
        return AMIIBO_REMOVED;
      }
    }
  }
};

photoresistor ph1(PHOTO1_PIN);
photoresistor ph2(PHOTO2_PIN);
photoresistor ph3(PHOTO3_PIN);

MFRC522 board1(NFC1_SDA_PIN, NFC1_RST_PIN);
MFRC522 board2(NFC2_SDA_PIN, NFC2_RST_PIN);
MFRC522 board3(NFC3_SDA_PIN, NFC3_RST_PIN);

Adafruit_NeoPixel pixels1(12, LED_DATA_PIN1, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(12, LED_DATA_PIN2, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixels3(12, LED_DATA_PIN3, NEO_RGB + NEO_KHZ800);


void setup() {
  pinMode(NFC1_SDA_PIN, OUTPUT);
  pinMode(NFC2_SDA_PIN, OUTPUT);
  pinMode(NFC3_SDA_PIN, OUTPUT);

  pixels1.begin();
  pixels1.clear();
  pixels2.begin();
  pixels2.clear();
  pixels3.begin();
  pixels3.clear();

  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial)
    ;                                // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                       // Init SPI bus
  board1.PCD_Init();                 // Init MFRC522
  board1.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  board2.PCD_Init();                 // Init MFRC522
  board2.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  board3.PCD_Init();                 // Init MFRC522
  board3.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Arduino amiibo reader. Put your NFC-tag to the reader"));

  ph1.set_threshold();
  ph2.set_threshold();
  ph3.set_threshold();
}

long dump_byte_array(byte* buffer, byte bufferSize) {
  long out =
    (buffer[3] * 0x1) + (buffer[2] * 0x100) + (buffer[1] * 0x10000) + (buffer[0] * 0x1000000);
  return out;
}

void set_slot_pixels(int slot, long color) {
  switch (slot) {
    case 0:
      for (int i = 0; i < 4; i++) {
        pixels1.setPixelColor(i, color);
      }
      pixels1.show();
      break;
    case 1:
      for (int i = 0; i < 4; i++) {
        pixels2.setPixelColor(i, color);
      }
      pixels2.show();
      break;
    case 2:
      for (int i = 0; i < 4; i++) {
        pixels3.setPixelColor(i, color);
      }
      pixels3.show();
      break;
  }
}

long get_led_value(long upper, long lower) {
  Serial.println(upper);
  Serial.println(lower);
  for (int i = 0; i < NUM_AMIIBO; i++) {
    if (upper == -1048576 && lower == 2949122){
      Serial.print("worked");
      return AMIIBO_DATABASE[random(0, NUM_AMIIBO)][AMIIBO_COLOR];
    }
    if (AMIIBO_DATABASE[i][AMIIBO_HEAD] == upper /*&& AMIIBO_DATABASE[i][AMIIBO_TAIL] == lower*/) {
      return AMIIBO_DATABASE[i][AMIIBO_COLOR];
    }
  }
  return 0;
}

void get_id(MFRC522 currBoard, int slot) {
  Serial.println("Board Start");
  byte buffer[10];
  byte buffer1_size = 10;
  int i = 0;
  while (i < 50) {
    i++;
    if (!currBoard.PICC_IsNewCardPresent()) {
      //Serial.print("N");
    } else if (!currBoard.PICC_ReadCardSerial()) {
      //Serial.print("L");
    } else {
      byte upper[18];
      byte lower[18];
      byte size = 18;
      currBoard.MIFARE_Read(UPPER_PAGE, upper, &size);
      currBoard.MIFARE_Read(LOWER_PAGE, lower, &size);
      long output = get_led_value(dump_byte_array(upper, 4), dump_byte_array(lower, 4));
      set_slot_pixels(slot, output);
      Serial.print(output, HEX);
      Serial.println();
      currBoard.PICC_HaltA();
      currBoard.PCD_StopCrypto1();
      Serial.println("Board Finish");
    }
  }
}

int num = 0;
void loop() {
  // Select one of the cards

  switch (ph1.get_slot_state()) {
    case AMIIBO_PLACED:
      get_id(board1, 0);
      Serial.println("placed in slot 1");
      break;
    case AMIIBO_REMOVED:
      set_slot_pixels(0, 0);
      Serial.println("removed from slot 1");
      break;
  }


  switch (ph2.get_slot_state()) {
    case AMIIBO_PLACED:
      get_id(board2, 1);
      Serial.println("placed in slot 2");
      break;
    case AMIIBO_REMOVED:
      set_slot_pixels(1, 0);
      Serial.println("removed from slot 2");
      break;
  }

  switch (ph3.get_slot_state()) {
    case AMIIBO_PLACED:
      get_id(board3, 2);
      Serial.println("placed in slot 3");
      break;
    case AMIIBO_REMOVED:
      set_slot_pixels(2, 0);
      Serial.println("removed from slot 3");
      break;
  }
}