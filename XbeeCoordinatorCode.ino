// Coordinator Code:

// frame array to be sent from the coordinator to the router(s)
byte frame[] = {0x7E, 0x0, 0x10, 0x17, 0x0, 0x0, 0x0, 0x0, 0x0,
                0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFE, 0x02};
                
char binary[] = {0, 0, 0, 0}; //array to store input for LED on/off states
char ascii[] = {'0', '1', '2', '3'}; //array to convert int to ASCII 

// 64 bit addresses for the Routers
byte MYAddr[][8] = {
{0x00, 0x13, 0xA2, 0x00, 0x41, 0x07, 0x04, 0x80}, //Sean1's router address
{0x00, 0x13, 0xA2, 0x00, 0x41, 0x55, 0xC1, 0x98}, //Kyle's router address
{0x00, 0x13, 0xA2, 0x00, 0x41, 0x07, 0x04, 0xB4}, //JT's router address
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF} //broadcast to all address
                   };

void setup() {
   Serial.begin(9600);    //sets baud rate for Serial to 9600 bps
   Serial2.begin(9600);   //sets baud rate for Serial2 to 9600 bps
}

void loop() {
  Serial.println("Who am I sending it to (0 = Sean1, Kyle = 1, JT = 2, all = 3)? ");
  while (Serial.available() == 0){}             // waits for user input
  int addr = Serial.parseInt();                 // reads user input
  for (int i = 0; i < 8; i++){
    frame[i + 5] = MYAddr[addr][i];  //updates frame destination address
  }
  Serial.println("What 4 bit binary should be displayed?");
  while (Serial.available() == 0){}         // waits for user input
  String userInput = Serial.readString();       //reads user input
  userInput.toCharArray(binary, 5);    // splits user input into char array
  for (int i = 0; i < 4; i++){
    // calls function to set state of pin "DX" (X = 4 - i)
    setRemoteState(ascii[3 - i], (binary[i] == '1') ? 0x5 : 0x4);  
  }
}

void setRemoteState(char pin, char value){
  long checksum = 0;            // sets checksum value to zero
  for (int i = 0; i < sizeof(frame)/sizeof(byte); i++){
    Serial2.write(frame[i]);    //writes frame info to Serial2 byte by byte
    // updates checksum starting with the Frame type byte
    if (i > 2) checksum += frame[i];       
  }
  Serial2.write('D');//writes first half of AT command of frame to Serial2  
  Serial2.write(pin);//writes second half of AT command of frame to Serial2
  Serial2.write(value);//writes value of the AT command of frame to Serial2
  checksum += ('D' + pin + value);          // updates checksum
  //writes checksum value of frame to Serial2
  Serial2.write( 0xFF - (checksum & 0xFF));
}
