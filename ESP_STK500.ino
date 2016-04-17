#include <FS.h>
#include <Streaming.h>
#include <SoftwareSerialESP.h>

SoftwareSerialESP ard(D7, D8);

#define ARD_RESET D5

void sendNBytes(File &file, int n) {
  Serial << "sending bytes: " << n << endl;
  ard << _BYTE(0x64) << _BYTE(0) << _BYTE((byte) n) << ' ';
  for (int i=0; i < n; i++)  {
    byte x = (byte)file.read();
    Serial << _HEX(x)<< " ";
    ard << _BYTE(x);
    delay(5);
  }
  ard << _BYTE(0x20);
}

boolean acknowledge() {
  //delay(50);
  return ard.find("\x14\x10");
}

void printBinHex(byte *bin, int len) {
  for (int i = 0; i < len; i++) Serial.print(*(bin++), HEX);
  Serial << endl;
}

boolean startProgrammingMode() {
  ard.setTimeout(1000);
  ard.begin(57600L);
  Serial << "Resetting arduino" << endl;
  pinMode(ARD_RESET, OUTPUT);
  digitalWrite(ARD_RESET, LOW);
  delay(100);
  Serial << " setting to High " << endl;
  digitalWrite(ARD_RESET, HIGH);
  for (int i =0; i < 30; i++) {
    //ard << "0 ";
    ard << _BYTE(0x30) << _BYTE(0x20);
    boolean ack = acknowledge();
    Serial << ack << " ";
    //Serial << _HEX(ard.read()) << " ";
    Serial << "." << i << ".";
    if (ack) break;
    //delay(50);
  }
  //pinMode(ARD_RESET, INPUT);
//  delay(100);
//  ard << (byte) 0;
//  delay(400);

 // if (!ard.find("\x0")) return false;
  Serial << "Connection to Arduino bootloader:" << endl;
  ard << "0 ";
  //ard << 0x30 << 0x20;
//  for (int i =0; i < 20; i++) {
//    delay(100);
//    Serial << ard.read() << " ";
//  }
//  Serial << ard.read();
//  Serial << ard.read();
//  Serial << ard.read();
//  Serial << ard.read();
  if (!acknowledge()) return false;
  
  Serial << "Enter in programming mode:" << endl;
  //ard << 0x50 << 0x20;
  ard << "P ";
  if (!acknowledge()) return false;

  Serial << "Read device signature:" << endl;
  //ard << 0x75 << 0x20;
  ard << "u ";
  if (!ard.find("\x14")) return false;
  byte sig[10];
  ard.readBytesUntil(0, sig, 3);
  printBinHex(sig, 3);
  if (!ard.find("\x10")) return false;  
}

boolean sendFile() {
SPIFFS.begin();
  File file = SPIFFS.open("/blink2.bin", "r");
  Serial << "1" << endl;
  Dir dir = SPIFFS.openDir("/data");
   while (dir.next()) {
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());  
  }
  Serial << "2" << endl;
  dir = SPIFFS.openDir("/");
   while (dir.next()) {
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());  
  }
    Serial << "3" << endl;
  dir = SPIFFS.openDir("");
   while (dir.next()) {
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());  
  }
  startProgrammingMode() ;
  while (file.available() > 0) {
    Serial << "sadsadsa" << endl;
    uint16_t chunkAddress = ((int)file.read()<<8) + file.read();
    uint16_t chunkSize = ((int)file.read()<<8) + file.read();
    Serial << "Chunk Size: " << chunkSize << endl;
    int total= chunkSize;
    if (total >0) {
      Serial << "sasssdsadsa" << endl;
      uint16_t current_page = chunkAddress;
      uint16_t index =0;
      while (total > 0) {
        Serial << "Load memory address " << current_page<<endl;
        ard << _BYTE(0x55) << _BYTE((byte)(current_page&0xFF)) << _BYTE((byte)(current_page>>8)) << _BYTE(0x20);
        if (!acknowledge()) return false;
        Serial << "Program memory address " << endl;
        if (total < 0x80) sendNBytes(file, total);
        else sendNBytes(file, 0x80);
//          for (int i =0; i < 30; i++) {
//            boolean ack = acknowledge();
//            Serial << ack << " ";
//            Serial << "." << i << ".";
//            if (ack) break;
//          }
        if (!acknowledge()) return false;
        Serial << "Set Page" << endl;
        ard << _BYTE(0x55) << _BYTE((byte)(current_page&0xFF)) << _BYTE((byte)(current_page>>8)) << _BYTE(0x20);
        if (!acknowledge()) return false;
  
        Serial << "Read Page" << endl;
        ard << _BYTE(0x74) << _BYTE(0) << _BYTE(0x80) << _BYTE(0x46) << _BYTE(0x20);
        byte buf[200];
        for (int i=0; i < 0x80+2; i++) {
          if (ard.available() == 0) delay(1);
          buf[i] = ard.read();
        }
        for (int i=0; i < 0x80+2; i++) {
          Serial << _HEX(buf[i]) << " ";
        }
        Serial << ".";
        current_page +=0x40;
        total -= 0x80;
      }
    }
  }
}


void setup() {
  Serial.begin(115200L);
  sendFile();
  Serial << "end" << endl;
  // put your setup code here, to run once:

}

void loop() {
 // Serial << ard.read();
  //delay(50);
  // put your main code here, to run repeatedly:

}
