/*
Test Master Description :
- Test Master connect with Test Slave 3 Led Matrix
- Matrix 3 X 32 X 64 P5

Baut Rate (9600)
- Ping: Mengecek apakah slave merespons dengan (PING,1), (PING,2), (PING,3)
- Clear: Menghapus tampilan slave dengan (CLEAR,1), (CLEAR,2), (CLEAR,3)
- Brightness: Mengatur tingkat kecerahan (BRIGHT,1,50), (BRIGHT,2,75), (BRIGHT,3,100)
- Parsing Command Otomatis: Bisa mengirim beberapa perintah sekaligus

*/
#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#define RXD1 16 // RX untuk Slave 1
#define TXD1 17 // TX untuk Slave 1

#define RXD2 18 // RX untuk Slave 2
#define TXD2 19 // TX untuk Slave 2

#define RXD3 32 // RX untuk Slave 3
#define TXD3 33 // TX untuk Slave 3

HardwareSerial Slave1(1);          // UART1 untuk Slave 1
HardwareSerial Slave2(2);          // UART2 untuk Slave 2
SoftwareSerial Slave3(RXD3, TXD3); // SoftwareSerial untuk Slave 3

// Fungsi untuk mengirim perintah ke Slave tertentu
void sendToSlave(int slaveNumber, String command)
{
  switch (slaveNumber)
  {
  case 1:
    Slave1.println(command);
    Slave1.flush();
    Serial.println("Sent to Slave 1: " + command);
    break;
  case 2:
    Slave2.println(command);
    Slave2.flush();
    Serial.println("Sent to Slave 2: " + command);
    break;
  case 3:
    Slave3.println(command);
    Slave3.flush();
    Serial.println("Sent to Slave 3: " + command);
    break;
  default:
    Serial.println("{ \"Error\" : \"Invalid slave number\" }");
  }
}

// Fungsi Ping untuk cek apakah slave aktif
void pingSlave(int slaveNumber)
{
  sendToSlave(slaveNumber, "(ping)");
}

// Fungsi Clear untuk membersihkan tampilan di slave
void clearSlave(int slaveNumber)
{
  sendToSlave(slaveNumber, "(clear)");
}

// Fungsi untuk mengatur kecerahan slave
void setBrightness(int slaveNumber, int brightness)
{
  if (brightness < 0 || brightness > 100)
  {
    Serial.println("{ \"Error\" : \"Brightness out of range (0-100)\" }");
    return;
  }
  sendToSlave(slaveNumber, "(B," + String(brightness) + ")");
}

void setup()
{
  Serial.begin(9600);
  Slave1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  Slave2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Slave3.begin(9600);
  Serial.println("Master Ready...");
}

void loop()
{
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    command.trim();

    // Parsing perintah yang dikirim dalam format (X,DATA)
    while (command.length() > 0)
    {
      int start = command.indexOf('(');
      int end = command.indexOf(')');

      if (start != -1 && end != -1 && start < end)
      {
        String singleCommand = command.substring(start + 1, end);
        command = command.substring(end + 1); // Hapus bagian yang sudah diproses

        int commaPos1 = singleCommand.indexOf(',');
        if (commaPos1 != -1)
        {
          String cmdType = singleCommand.substring(0, commaPos1);
          String rest = singleCommand.substring(commaPos1 + 1);

          // Perintah Ping
          if (cmdType == "PING")
          {
            int slaveNumber = rest.toInt();
            pingSlave(slaveNumber);
          }
          // Perintah Clear
          else if (cmdType == "CLEAR")
          {
            int slaveNumber = rest.toInt();
            clearSlave(slaveNumber);
          }
          // Perintah Set Brightness
          else if (cmdType == "BRIGHT")
          {
            int commaPos2 = rest.indexOf(',');
            if (commaPos2 != -1)
            {
              int slaveNumber = rest.substring(0, commaPos2).toInt();
              int brightness = rest.substring(commaPos2 + 1).toInt();
              setBrightness(slaveNumber, brightness);
            }
            else
            {
              Serial.println("{ \"Error\" : \"Invalid brightness command\" }");
            }
          }
          // Perintah Kirim Pesan Biasa
          else
          {
            int slaveNumber = cmdType.toInt();
            if (slaveNumber >= 1 && slaveNumber <= 3)
            {
              sendToSlave(slaveNumber, "(" + cmdType + "," + rest + ")");
            }
            else
            {
              Serial.println("{ \"Error\" : \"Invalid slave number\" }");
            }
          }
        }
        else
        {
          Serial.println("{ \"Error\" : \"Invalid command format\" }");
        }
      }
      else
      {
        break; // Tidak ada lagi format (X,DATA)
      }
    }
  }

  // Cek respon dari masing-masing slave
  if (Slave1.available())
  {
    Serial.println("Slave 1 Respond: " + Slave1.readStringUntil('\n'));
  }
  if (Slave2.available())
  {
    Serial.println("Slave 2 Respond: " + Slave2.readStringUntil('\n'));
  }
  if (Slave3.available())
  {
    Serial.println("Slave 3 Respond: " + Slave3.readStringUntil('\n'));
  }

  delay(50);
}