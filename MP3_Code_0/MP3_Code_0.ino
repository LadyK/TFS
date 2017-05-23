#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <Wire.h>

SdFat sd;
SFEMP3Shield MP3player;
const int address = 4;  // the address to be used by the communicating devices

//bool trigger = 0;
int8_t current_track; // = 0001;
//File myFile; //
//SdFile myFile;

SdFile root;


void setup() {

  Serial.begin(115200);

  //start the shield
  sd.begin(SD_SEL, SPI_HALF_SPEED);
  MP3player.begin();
  MP3player.setVolume(10, 10);
  //start playing track 1
  //MP3player.playTrack(current_track);
  Wire.begin(4);
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);
}

//do something else now
void loop() {
  delay(100);
  /*
    if (!MP3player.isPlaying() && (trigger == 0)) {
      trigger = 1;
      //delay(10000);

    }
    else if (!MP3player.isPlaying() && trigger == 1) {
      Serial.println("I'm bored!");
      delay(20000);
      current_track++;
      trigger = 0;
      MP3player.playTrack(current_track);
      Serial.print("Current file is: ");Serial.println(current_track);
    }
    Serial.print("Current file is: ");Serial.println(current_track);

    if(current_track == 27){
      current_track = 0;
    }
    */
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
  /*
  String suffix = ".mp3";
  String file = x + suffix;
  myFile = SD.open(file);
  */
  /*
  if (!myFile.open(file, O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }
  
  int siz = myFile.size();
  Serial.print("File size is: "); Serial.println(siz);
  
  myFile.close();
  */
 
  MP3player.playTrack(x);



}
