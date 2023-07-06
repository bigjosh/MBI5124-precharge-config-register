// Tool for manually messing around with the Macroblock MBI5124 LED driver
// To use, connect the LED module pins to the Arduino pins as configured below,
// then connect to the Arduino using the Serial Monitor function in the Arduino IDE.
// From there you can send commands to the Arduino that will send signals to the ICs. 

// To test, enter the string '1Llo' to light the first LED. This clocks out a 1 bit, latches it, then sets Output Enable low (which is active). 

// How many MBI5124 ICs are connected to the data/clock lines? 
#define CHIP_STRING_LEN 4

struct pin {
  char *name;
  int number;
  char key;
  boolean startup;
};


#define OE_PIN  5
#define SDA_PIN 7
#define CLK_PIN 6
#define LAT_PIN 8

struct pin pins[] {

  {"MXA" , 2        , 'A' , 0},      // Mux pins
  {"MXB" , 3        , 'B' , 0},
  {"MXC" , 4        , 'C' , 0},
  {"MXDC", 9        , 'D' , 0},

  {"~OE" , OE_PIN   , 'O' , 1},      // Output enable
  {"CLK" , CLK_PIN  , 'K' , 0},      // Clock
  {"SDA" , SDA_PIN  , 'D' , 0},      // Data
  {"LAT" , LAT_PIN  , 'L' , 0},      // Latch

  {"LED" , LED_BUILTIN , 'S' , 0 },    // On Arduino board LED for testing

};

int pins_count = sizeof(pins) / sizeof(pins[0]);



void sendBit( boolean b ) {
  
   digitalWrite( SDA_PIN , b ? HIGH: LOW );

   digitalWrite( CLK_PIN , 1 );
   digitalWrite( CLK_PIN , 0 );
  
}


void send16bits( unsigned d ) {

  // Clock out the bits

  // Send most significant bit first on this chip
  auto mask=0b1000000000000000;     // Start with bit #15 and work our way down. 

  for( auto b=0; b<16; b++ ) {

   sendBit ( d & mask  ) ;

   mask >>= 1;
    
  }  
}


void send16bitsToAllChips( unsigned d ) {
  
    for( auto a = 0 ; a < CHIP_STRING_LEN; a++ ) {

      send16bits( d );
    
  }
}



// From datasheet page 12
#define BLUE_PRECHARGE_REG_VALUE 0b1110110101101011

// Set the internal precharge config registers. This involves clocking out the 16 bit values and then
// toggling the `clk` line 4 times while `lat` is high as per datasheet page 12.

void setConfigRegisters( unsigned r ) {
  
  send16bitsToAllChips( r ); 

  digitalWrite( LAT_PIN , 1 );

  // 4 clocks
  digitalWrite( CLK_PIN , 1 );
  digitalWrite( CLK_PIN , 0 );
  
  digitalWrite( CLK_PIN , 1 );
  digitalWrite( CLK_PIN , 0 );
  
  digitalWrite( CLK_PIN , 1 );
  digitalWrite( CLK_PIN , 0 );
  
  digitalWrite( CLK_PIN , 1 );
  digitalWrite( CLK_PIN , 0 );
  
  digitalWrite( LAT_PIN , 0 );

}


// Scan all the values for the precharge register and measure and print the corresponding output voltage on the OUT pin.
// Assumes Arduino pin PRECHARGE_TAP_PIN is connected to an OUT pin with no LED connected.
// If you must lead LED connected, best to also select the row of LED that the tap is connected to before scanning or else the max voltage will be 
// limited by the reverse breakdown of an LED in that row. 
// Output is CSV format to inport into a spreadsheet.

#define PRECHARGE_TAP_PIN A0

void scanPrechargeValues() {
  
  pinMode( PRECHARGE_TAP_PIN , INPUT );

  unsigned r=0;


  // We can't use a normal for loop here becuase we want to try every possible `r` value. 
  // (I guess we could have made `r` a long, but this is clear)
  
  do {

    setConfigRegisters( r );

    unsigned v = analogRead( PRECHARGE_TAP_PIN );

    Serial.print( r );
    Serial.print( ",");
    Serial.print( v );
    Serial.println();

    r++;
    
  } while (r!=0);

  
}


void setup() {

  Serial.begin(9600);
  // put your setup code here, to run once:

  Serial.println("Initializing pins...");

  for( auto p : pins ) {

    Serial.print("[");
    Serial.print( p.key );
    Serial.print("]");

    Serial.print( p.name );
    Serial.print( ":");
    pinMode( p.number, OUTPUT );     
    digitalWrite( p.number, p.startup ); 
    Serial.println( p.startup ); 

  }

  Serial.println("Press [key] in caps to set high, or lower to set low.");
  Serial.println("([1] or [0] to clock out a bit on SDA+CLK.");

  Serial.println("[!] All LEDs ON");    
  Serial.println("[)] All LEDs OFF");  
  Serial.println("[)] All LEDs alternating");  
  

  Serial.println("[$] Set precharge register to 0x0000 (lowest voltage available)");    
  Serial.println("[%] Set precharge register to recommended blue voltage");    
  
}

void processKey( char inChar ) {

  // Check for send data bits
  
  if (inChar=='0' || inChar=='1') {
  
    int outbit = inChar=='1';
    
    digitalWrite( CLK_PIN , LOW );
    delay(1);
    digitalWrite( SDA_PIN , outbit );
    delay(1);      
    digitalWrite( CLK_PIN , HIGH );
    delay(1);
    digitalWrite( CLK_PIN , LOW );
  
    Serial.print( outbit ? '1':'0' );
  
    return;
          
  } 
  
  // Check for pin toggle keys
  
for( auto p : pins ) {

  if (inChar == toupper(p.key)) {
    
    Serial.print( p.name );
    digitalWrite( p.number , 1 );
    Serial.println(":1");

    return;
    
  } else if (inChar == tolower(p.key)) {

    Serial.print( p.name );
    digitalWrite( p.number , 0 );
    
    Serial.println(":0");

    return;
    
  }
}


  switch (inChar) {


    case '$':
      setConfigRegisters( 0x0000 );
      Serial.println("Precharge register set to 0x0000");    
      return;
      
    case '%':
      setConfigRegisters( BLUE_PRECHARGE_REG_VALUE );
      Serial.println("Precharge register set to BLUE_PRECHARGE_REG_VALUE");    
      return;

    case '!':
      send16bitsToAllChips( 0xFFFF );
      digitalWrite( LAT_PIN , HIGH );
      digitalWrite( LAT_PIN , LOW );
      
      Serial.println("All LEDs on.");    
      return;

    case '#':
      send16bitsToAllChips( 0b1010101010101010 );
      digitalWrite( LAT_PIN , HIGH );
      digitalWrite( LAT_PIN , LOW );
      
      Serial.println("All LEDs Alternating.");    
      return;
      

    case ')':
      send16bitsToAllChips( 0x00 );
      digitalWrite( LAT_PIN , HIGH );
      digitalWrite( LAT_PIN , LOW );
      
      Serial.println("All LEDs OFF.");    
      return;
      
      
  }        
  
  Serial.print("Unknown key:");    
  Serial.print( (int) inChar);      // casting to int makes it print as a number rather than char
  Serial.print("[");    
  Serial.print(inChar);    
  Serial.print("]");    
  Serial.println("");    
}

void loop() {
  // put your main code here, to run repeatedly:

  
  // check for incoming serial data:

  if (Serial.available() > 0) {

    // read incoming serial data:

    char inChar = Serial.read();

    processKey( inChar );

  }
}
