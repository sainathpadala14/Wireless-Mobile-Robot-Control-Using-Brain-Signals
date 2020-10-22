////////////////////////////////////////////////////////////////////////
// Arduino Bluetooth Interface with Mindwave
// 
// This is example code provided by NeuroSky, Inc. and is provided
// license free.
////////////////////////////////////////////////////////////////////////

//#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0

#define LM1  3
#define LM2  4
#define RM1  7
#define RM2  8


#define powercontrol 10

// checksum variables
byte generatedChecksum = 0;
byte checksum = 0; 
int payloadLength = 0;
byte payloadData[64] = {
  0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;

//////////////////////////
// Microprocessor Setup //
//////////////////////////
void setup() {


  pinMode(LM1, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM2, OUTPUT);
  Serial.begin(BAUDRATE);           // USB

}

////////////////////////////////
// Read data from Serial UART //
////////////////////////////////
byte ReadOneByte() {
  int ByteRead;

  while(!Serial.available());
  ByteRead = Serial.read();

#if DEBUGOUTPUT  
  Serial.print((char)ByteRead);   // echo the same byte out the USB serial (for debug purposes)
#endif

  return ByteRead;
}

/////////////
//MAIN LOOP//
/////////////
void loop() {

  // Look for sync bytes
  if(ReadOneByte() == 170) {
    if(ReadOneByte() == 170) {
      payloadLength = ReadOneByte();
      if(payloadLength > 169)                      //Payload length can not be greater than 169
          return;

      generatedChecksum = 0;        
      for(int i = 0; i < payloadLength; i++) {  
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
      }   
      checksum = ReadOneByte();                      //Read checksum byte from stream      
      generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum
        if(checksum == generatedChecksum) {    
        poorQuality = 200;
        attention = 0;
        meditation = 0;

        for(int i = 0; i < payloadLength; i++) {    // Parse the payload
          
          switch (payloadData[i]) {
          case 2:
            i++;            
            poorQuality = payloadData[i];
            bigPacket = true;            
            break;
          case 4:
            i++;
            attention = payloadData[i];                        
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;      
            break;
          default:
            break;
          } // switch
        } // for loop
#if !DEBUGOUTPUT

        // *** Add your code here ***

        if(bigPacket) {
          Serial.print("PoorQuality: ");
          Serial.print(poorQuality, DEC);
          Serial.print(" Attention: ");
          Serial.print(attention, DEC);
          Serial.print(" Meditation: ");
          Serial.print(meditation, DEC);
          Serial.print(" Time since last packet: ");
          Serial.print(millis() - lastReceivedPacket, DEC);
          lastReceivedPacket = millis();
          Serial.print("\n");
          
          switch(attention / 20) {
            case 0:
              digitalWrite(LM1,HIGH);
              digitalWrite(LM2,LOW);
              digitalWrite(RM1,HIGH);
              digitalWrite(RM2,LOW);
              break;
            case 1:
              digitalWrite(LM1,LOW);
              digitalWrite(RM1,LOW);
              digitalWrite(LM2,LOW);
              digitalWrite(RM2,LOW);
              break; 
            case 2:
              digitalWrite(LM1,LOW);
              digitalWrite(LM2,LOW);
              digitalWrite(RM1,LOW);
              digitalWrite(RM2,LOW);
              break; 
              
            case 3:
               digitalWrite(LM1,LOW);
              digitalWrite(LM2,HIGH);
              digitalWrite(RM1,LOW);
              digitalWrite(RM2,HIGH);
              break; 
              
            case 4:
               digitalWrite(LM1,LOW);
              digitalWrite(LM2,HIGH);
              digitalWrite(RM1,LOW);
              digitalWrite(RM2,HIGH);
              break; 

              case 5:
               digitalWrite(LM1,LOW);
              digitalWrite(LM2,HIGH);
              digitalWrite(RM1,LOW);
              digitalWrite(RM2,HIGH);
              break; 
          
          }                     
        }
#endif        
        bigPacket = false;        
      }
      else {
        // Checksum Error
      }  // end if else for checksum
    } // end if read 0xAA byte
  } // end if read 0xAA byte
}



