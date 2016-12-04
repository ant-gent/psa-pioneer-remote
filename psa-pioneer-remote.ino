#include <SPI.h>
#include <mcp_can.h>

#define KOHMS(i) 256-((i/100.0)*256.00)

// Pioneer WR resistance values
#define WR_SOURCE KOHMS(1.2)
#define WR_ATT KOHMS(3.5)
#define WR_DISPLAY KOHMS(5.75)

// PSA Remote CAN message values
#define REMOTE_ID 0x21F
#define FORWARD   B10000000
#define BACKWARD  B01000000
#define VOL_UP    B00001000
#define VOL_DOWN  B00000100
#define SOURCE    B00000010

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
unsigned char remoteState = 0;
char msgString[128];                        // Array to store serial string


#define CAN0_INT    3
#define CAN0_CS    10
#define DIGIPOT_CS  9

MCP_CAN CAN0(CAN0_CS); 
 
void setup() {
  
  Serial.begin(115200);
  
  pinMode(13, OUTPUT);
  pinMode (DIGIPOT_CS, OUTPUT); 
  SPI.begin();
  shutdown();
  
  int canOk = 0;
  // Initialize MCP2515 running at 16MHz with a baudrate of 125kb/s and the masks and filters disabled.
  
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    if(canOk = CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_16MHZ) == CAN_OK)
      Serial.println("MCP2515 Initialized Successfully!");
    else
      Serial.println("Error Initializing MCP2515...");


  CAN0.setMode(MCP_LISTENONLY);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
  
  pinMode(CAN0_INT, INPUT);
  
  Serial.println("Setup OK");

}
 
void loop() {

  if(!digitalRead(CAN0_INT)){
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    
    if((rxId & 0xFFF) != REMOTE_ID) return;
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
//Serial.print(msgString);
    //Serial.println();


    //Serial.println(rxId);
    //print_binary(rxBuf[0], 8);
    //Serial.println();    
        
    if(remoteState == rxBuf[0]){
      return;
    } else {
      remoteState = rxBuf[0];
    }
    
    if(remoteState == 0){
      shutdown();
    }
    
    if(remoteState & SOURCE){
      digitalPotWrite(WR_SOURCE);
      Serial.println("Source");
    }
    
    if((remoteState & VOL_UP) && (remoteState & VOL_DOWN)){
      digitalPotWrite(WR_ATT);
      Serial.println("Mute");
      return;
    }
    
    if(remoteState & VOL_UP){
      Serial.println("Volume up");
    }
    
    if(remoteState & VOL_DOWN){
      Serial.println("Volume down");
    }
    
    if(remoteState & FORWARD){
      Serial.println("Forward");
    }
    
    if(remoteState & BACKWARD){
      Serial.println("Backward");
    }
  }

}
 
void digitalPotWrite(byte value) 
{
  digitalWrite(CAN0_CS, HIGH);
    // take the SS pin low to select the chip:
  digitalWrite(DIGIPOT_CS,LOW);
  SPI.transfer(B00010001); // The command byte
  SPI.transfer(value);     // The data byte
  // take the SS pin high to de-select the chip
  digitalWrite(DIGIPOT_CS,HIGH);
}

void shutdown(){
  digitalWrite(CAN0_CS, HIGH);
  // take the SS pin low to select the chip:
  digitalWrite(DIGIPOT_CS,LOW);
  SPI.transfer(B00100001); // The command byte
  SPI.transfer(0);     // The data byte
  // take the SS pin high to de-select the chip
  digitalWrite(DIGIPOT_CS,HIGH); 
}

void print_binary(int v, int num_places)
{
    int mask=0, n;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask;  // truncate v to specified number of places

    while(num_places)
    {

        if (v & (0x0001 << num_places-1))
        {
             Serial.print("1");
        }
        else
        {
             Serial.print("0");
        }

        --num_places;
        if(((num_places%4) == 0) && (num_places != 0))
        {
            Serial.print("_");
        }
    }
}

