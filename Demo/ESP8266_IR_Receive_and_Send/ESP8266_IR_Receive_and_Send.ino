/*
 * Dependency - IRremoteESP8226 Library
 * 
 * 
 */

#include <Arduino.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include "ESP8266_UART_TOUT_INT_clarkwise_handler.h"

// The GPIO an IR detector/demodulator is connected to. Recommended: 14 (D5)
const uint16_t kRecvPin = 14;

// GPIO to use to control the IR LED circuit. Recommended: 4 (D2).
const uint16_t kIrLedPin = 4;

// As this program is a special purpose capture/resender, let's use a larger
// than expected buffer so we can handle very large IR messages.
const uint16_t kCaptureBufferSize = 1024;  // 1024 == ~511 bits

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
const uint8_t kTimeout = 50;  // Milli-Seconds

// kFrequency is the modulation frequency all UNKNOWN messages will be sent at.
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.

//flags dealing in the loop
bool flag_learn = false;
bool flag_send = false;
bool flag_learned = false;
bool flag_save = false;
bool flag_diable_controls = false;
String tmp_slot_name = "";
byte tmp_slot_index = 255; //255 means no slot saved

// ==================== end of TUNEABLE PARAMETERS ====================

// The IR transmitter.
IRsend irsend(kIrLedPin);
// The IR receiver.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);

// Somewhere to store the captured message.
const uint8_t slots_num = 6;
decode_results results, emptyResult;
struct result_slot //create a struct to store all 6 slots
{
  decode_results r;
  String name;
} result_slots[slots_num];

void clarkwise_ir_send(byte slot_id);
void display_msg(String msg, String color);
void hide_msg();

void setup() {
  // put your setup code here, to run once:
  irsend.begin();       // Start up the IR sender.

  Serial.begin(9600, SERIAL_8N1);
  while (!Serial){  // Wait for the serial connection to be establised.
    delay(50); 
  }
  install_uart_tout(); //Install the UART RX TOUT interrupt handler
  delay(1000);
  Serial.println("@CTRL 105,2"); //Disable and hide the REC gif control
  Serial.println("@SET 107,Enter a name:"); //Disable and hide the REC gif control
  hide_msg();
}

void loop() {

  if(flag_learn){
    flag_send = false;
    flag_save = false;
    if(!flag_diable_controls){
      irrecv.enableIRIn(); //Start to receive IR signal
      //irrecv.resume(); 
      Serial.println("@CTRL 100,2");
      Serial.println("@CTRL 101,2");
      Serial.println("@CTRL 102,2");
      Serial.println("@CTRL 103,2");
      Serial.println("@CTRL 104,2");
      Serial.println("@CTRL 107,2");
      hide_msg();
      Serial.println("@CTRL 105,1");
      flag_diable_controls = true;
    }

    // Check if an IR message has been received.
    if (irrecv.decode(&results)) {  // We have captured something.
      flag_learn = false;
      flag_learned = true;
      irrecv.resume();
      irrecv.disableIRIn();
      // The capture has stopped at this point.
      Serial.println("@CTRL 100,1");
      Serial.println("@CTRL 101,1");
      Serial.println("@CTRL 102,1");
      Serial.println("@CTRL 103,1");
      Serial.println("@CTRL 104,1");
      Serial.println("@CTRL 107,1");
      Serial.println("@CTRL 105,2");
      flag_diable_controls = false;
      display_msg("Success: IR signal received. Pls save.","Green");
    }
  }

  if(flag_save){
    if(flag_learned){
      if(tmp_slot_name==""){
        display_msg("Error: Enter a name first.","Red");
        flag_save = false;
      } else if(tmp_slot_index==255){
        display_msg("Error: Select a slot to save.","Red");
        flag_save = false;
      } else {
        display_msg("Success: Saved at slot: " + String(tmp_slot_index),"Green");
        result_slots[tmp_slot_index].r = results;
        result_slots[tmp_slot_index].name = tmp_slot_name;
        flag_save = false;
        Serial.println("@CTRL 103,0x20," + String(tmp_slot_index) + "," + tmp_slot_name); //Change the combo box
      }
    } else {
      display_msg("Learn a signal before save it.","Red");
      flag_save = false;
    }
  }

  if(flag_send){
    if(tmp_slot_index==255){
      display_msg("Error: Select a slot to send.","Red");
      flag_send = false;
    } else {
      display_msg("Success: Singal in slot " + String(tmp_slot_index) + " is sent!","Green");
      clarkwise_ir_send(tmp_slot_index);
      flag_send = false;
    } 
  }
}

void display_msg(String msg, String color){
  if(color=="Green"){
    Serial.println("@AUDIO retro-game-notification.wav");
  }
  if(color=="Red"){
    Serial.println("@AUDIO classic-alarm.wav");
  }
  Serial.println("@CTRL 106,0x11," + color);
  Serial.println("@SET 106," + msg);
  Serial.println("@CTRL 106,1");
}

void hide_msg(){
  Serial.println("@CTRL 106,2"); //Disable and hide the error msg string control
}

void clarkwise_returns_handler(){
  switch(rMsg.control_id){
    case 100: //Learn button pressed
      if (rMsg.varInt==1){
        flag_learn = true;
      }
      break;
    case 101: //Save button pressed
      if (rMsg.varInt==1){
        flag_save = true;
      }
      break;
    case 102: //Send button pressed
      if (rMsg.varInt==1){
        flag_send = true;
      }
      break;
    case 103: //Combobox selection changed
      tmp_slot_index = rMsg.varInt;
      break;
    case 104: //Name string input box entered new string
      tmp_slot_name = rMsg.varString;
      break;
  }
}

void clarkwise_ir_send(byte slot_id){
  decode_type_t protocol = result_slots[slot_id].r.decode_type;
  uint16_t size = result_slots[slot_id].r.bits;
  bool success = true;
  // Is it a protocol we don't understand?
  if (protocol == decode_type_t::UNKNOWN) {  // Yes.
    // Convert the results into an array suitable for sendRaw().
    // resultToRawArray() allocates the memory we need for the array.
    uint16_t *raw_array = resultToRawArray(&result_slots[slot_id].r);
    // Find out how many elements are in the array.
    size = getCorrectedRawLength(&result_slots[slot_id].r);
#if SEND_RAW
    // Send it out via the IR LED circuit.
    irsend.sendRaw(raw_array, size, kFrequency);
#endif // SEND_RAW
    // Deallocate the memory allocated by resultToRawArray().
    delete [] raw_array;
  } else if (hasACState(protocol)) {  // Does the message require a state[]?
    // It does, so send with bytes instead.
    success = irsend.send(protocol, result_slots[slot_id].r.state, size / 8);
  } else {  // Anything else must be a simple message protocol. ie. <= 64 bits
    success = irsend.send(protocol, result_slots[slot_id].r.value, size);
  }
}
