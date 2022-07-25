#include "main.h"
#include "Value.h"
#include "Lexer.h"
#include <Arduino.h>
#include <FlashStorage.h>

const char* buffer;
bool isRepl = false;

typedef struct {
  char buffer[4096];
} Program;

FlashStorage(program, Program);

void setup()
{
  SerialUSB.begin(9600); // Initialize Serial Monitor USB
  Serial1.begin(9600); // Initialize hardware serial port, pins 0/1
  while (!SerialUSB) ; // Wait for Serial monitor to open
  SerialUSB.println("Press key to enter REPL...");
  SerialUSB.setTimeout(5000);
  
  String data = SerialUSB.readStringUntil('\n');
  SerialUSB.println(data);
  SerialUSB.setTimeout(0);

  
  if (data != "") {
    SerialUSB.println("In REPL");
    isRepl = true;
    Lexer l;
    l.repl_mode = true;
    SerialUSB.print(">> ");
    while (data = SerialUSB.readStringUntil('\n')) {
      if (data != "") {
          l.execute_statement(data.c_str());
          SerialUSB.print(">> ");
        }
    }
    
  } else {
    buffer = "a=0\nDIM ab(5) as Integer\nfor i=0 to 4\nprint(i)\nnext i\nEND";

    Lexer l(buffer);
    l.program();
  }
}

void loop()
{

}
