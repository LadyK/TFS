
/*
   one screen working flipping through the message, sends file to MP3 arduino, timed files via duration (via file size)

   next issues:
   - automation

   Version 2: refactor code
   - dynamically figuring out file size vs array)
   - breaking into functions, use of pointers

*/
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <Wire.h>

const int chipSelect = 10;  // SD card
LiquidCrystal lcd(9);

File myFile;
int start;
int indice = 0;
int newIndice = 0;
int finish;
char line[21]; // hold char for each line for screen before gone over  // screen is 0-19 (20 slots), but need (1??)null at end of arrays. Hence, we have 21 slots
char line0[21]; // holds line for screen, post space run
char nxtspc[3]; // to hold the next item

//int trackSize[28] = {0, 366, 325, 269, 363, 351, 242, 295, 346, 579, 597, 139, 248, 259, 380, 343, 248, 348, 336, 297, 231, 399, 4, 176, 173, 341, 91, 151};

String screenSt; // one line, inital letters captured in a string
String xtra;  // string to capture what the next letters would be

long randomNumber;  // randomSeed
boolean playToggle = 0;
long timeInterval = 20000;
long interval_window;
long startTime;

int capacity = 0;
int numFiles = 27;
int storageH[28]; // need to store zero slot, but will never use that slot
int newVM;
String suffix = ".txt";
int lag = 1000;  // time to hold one screen. change for debugging 10000 is good.

bool endFile = 0;
bool seekFile = 0;
bool peekFile = 0;

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");


  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("hello, world!!");
  delay(2000);
  lcd.clear();
  randomSeed(analogRead(A0));  // seeding random

  pinMode(7, INPUT);
  Wire.begin();                // join i2c bus with address #8
  //Wire.onRequest(requestEvent); // register event
  //newFile();


  lcd.noCursor();
}

void loop() {
  /*
    Serial.print("interval_window is: ");
    Serial.println(interval_window);
  */


  if (playToggle == 0) { // and if we aren't already paused
    playToggle = 1;
    // start timer
    startTime = millis();
    Serial.print("**startTime is: ");
    Serial.println(startTime);
    Serial.println();
    Serial.print("**interval_window is: ");
    Serial.println(interval_window);
  }

  else if (playToggle == 1 && ((millis() - startTime) > interval_window) ) {
    bool endFile = 0;
    bool seekFile = 0;
    //bool peekFile = 0;

    newVM = int(random(1, numFiles + 1)); // select new file  newVM = ; int(random(1, numFiles));
    Serial.print(F("random number is: "));
    Serial.println(newVM);
    String file = newVM + suffix;
    //Serial.print("Our file title is ");
    //Serial.println(file);
    delay(2000);
    while (alreadyShown(newVM) == 1) {
      Serial.println(F("I've been seen already"));
      newVM = int(random(1, numFiles + 1));
      Serial.print(F("New rand file is: ")); Serial.println(newVM);
      alreadyShown(newVM);
    }
    Serial.println();
    file = newVM + suffix;
    newFile(file, newVM);

  }
  else {
    //startTime = 0;
    //playToggle = 0;


  }
  //Serial.print("calculation is: ");
  //Serial.println(millis() - startTime);

} // loop




void newFile(String file, int rand_indice) {  //int rand_indice
  //Serial.println(rand_indice);
  //String file = rand_indice + ".txt";
  Serial.println(file);
  //myFile.openNextFile();
  seekFile = 0;
  peekFile = 0;
  indice = 0;  // exchange indice here
  newIndice = 0;
  lcd.clear();
  Serial.flush();
  myFile = SD.open(file);
  unsigned long siz = myFile.size();    // how many char in the file?
  Serial.print(F("File size is: ")); Serial.println(siz);
  int screens = ceil(siz / 80 );  // how many screens will we need
  Serial.print("We supposedly will need "); Serial.print(screens); Serial.println(" screens.");
  switch (screens) {
    case 0:
      screens = 1;
      break;
    case 1:
      screens = 1;
      break;
    case 2:
      screens = screens + 1;
      break;
    case 3:
      //screens++;
      screens = screens + 1;
      break;
    case 4:
      screens = screens + 1;
      break;
    case 5:
      screens = screens + 2;
      break;
    case 6:
      screens = screens + 2;
      break;
    case 7:
      screens = screens + 2;
      break;
    case 8:
      screens = screens + 2;
      break;
    case 9:
      screens = screens + 2;
      break;
    case 10:
      screens = screens + 2;
      break;
    case 11:
      screens = screens + 3;
      break;
    case 12:
      screens = screens + 3;
      break;

  }




  Serial.print(F("We REALLY will need ")); Serial.print(screens); Serial.println(F(" screens."));
  //Serial.println();
  myFile.close();
  //Serial.println(screens);

  /**********
    String prefix = "/tunes/";
    String suffix = ".mp3";
    String file = prefix + rand_indice + suffix;
    Serial.print("track is: "); Serial.println(file);
    myFile = SD.open(file);
    //unsigned long trackSize = myFile.size();
    siz = 0;
    siz = myFile.size();
    Serial.print("Track size is: "); Serial.println(siz);
    myFile.close();
  ********/

  //float trackDur = (trackSize[rand_indice] * 8) / 64;
  float trackDur = (siz * 8) / 64;
  Serial.print(F("TrackDuration is: ")); Serial.println(trackDur);
  float perScreen = ceil(trackDur / screens);
  Serial.print(F("perscreen is: ")); Serial.println(perScreen);
  // tweaking the time perScreen a bit to be closer to length of audio msg
  // this needs to be calculated independent or more generally relative to size
  if(perScreen >= 1 && perScreen < 2) perScreen = perScreen + 2;
  else if (perScreen >= 7 && perScreen < 8) perScreen--;
  else if (perScreen >= 9 ) perScreen = perScreen - 3; //&& perScreen < 10
  else if (perScreen >= 11) perScreen = perScreen - 4;
  Serial.print(F("perscreen is: ")); Serial.println(perScreen);

  Wire.beginTransmission(4); //transmit to device
  Wire.write(rand_indice);   // sends
  Wire.endTransmission();  // stop transmitting

  for (int i = 0; i < screens; i++) {
    // play the message on the screen
    oneScreen(file, perScreen); // pass in file name****** and indice
    lcd.clear();
    //Serial.flush();
    lcd.setCursor(0, 0);


    // Serial.print(i + 1);
    // Serial.println("  SCREENS(S) COMPLETED");
    // Serial.println();
  }
  long tI_min;
  // store indice in history array
  storageCheck(rand_indice);
  //endFile = 0;   // reset this
  //Serial.print("base interval is: ");
  //Serial.println(timeInterval);
  tI_min = 10000;
  interval_window = random(tI_min, timeInterval); // select a new random interval for silence
  Serial.print("interval_window is just set as: ");
  Serial.println(interval_window);
  Serial.println();
  ///*
  startTime = millis();
  Serial.print("new start time: ");
  Serial.println(startTime);
  //*/

    
}

void oneScreen(String filename, int pS) {  // int lag
  for ( int i = 0; i < 4; i ++) { // each loop is one scren
    // Serial.println("opening up the file again for a go");
    lcd.setCursor(0, i);
    myFile = SD.open(filename);

    if (!myFile) {
      //Serial.println("Another fucking issue");
    }

    String screenSt; // one line
    char line[21]; // one line
    if (myFile) {
      while (myFile.available()) { //Check if there are any bytes available for reading from the file.
        newIndice += indice;
        //Serial.print("Indice to seek from is: ");
        //Serial.println(newIndice);
        myFile.seek(newIndice); // move it to beyond one space where we left off ? <--- can get rid of if statement entirely

        if (!myFile.seek(newIndice)) { // else we are at end of file
          seekFile = 1;
          // reset peekFile here?
          // peekFile = 0;
          /*
            indice = 0;
            endFile = 1;
            nxtspc[0] = 0; // making this zero so can compare it elsewhere
            xtra = "   ";
          */

          // Serial.println("Caught at seek");
          // Serial.println();
          String xtra;
          String screenSt;
          char line[21];
          indice = 0;
          break; // choose new file. we've gone through this one****
        }

        if (myFile.peek() == -1 && peekFile == 0) {
          peekFile = 1;
          // clear them out by re-declaring them
          /*
            String screenSt = " "; // one line, inital letters captured in a string
            String xtra;  // string to capture what the next letters would be
            char line[21];
          */
          // Serial.println("We are at the end of the file. peek");
          // Serial.println();
          /*
            indice = 0;
          */
          endFile = 1;
          nxtspc[0] = 0; // making this zero so can compare it elsewhere
          xtra = "   ";

          //break; // breaks out of while-loop
        }


        String xtra;

        for (int i = newIndice; i < (20 + newIndice + 2 ); i++) { // data for one line. Doing one line at a time. Store it. (< most linesX + indice  + ???? extra?)



          char readByte = char(myFile.read()); // returns the next byte

          if ( i == newIndice) {

            screenSt += readByte; // capture the bytes into a string
          }
          else if ( i > newIndice && i < (19 + newIndice) ) { // leaving out the last item in the array - the end of line char. 19 is 20th char on screen
            screenSt += readByte; // capture the bytes into a string
          }
          else if ( i == (19 + newIndice)) {

            screenSt += readByte; // capture the bytes into a string
          }
          else if (i >= (20 + newIndice) ) {
            xtra += readByte;
          }


        } // for loop to capture chars


        // Serial.println("When do we cross here?");
        // Serial.println();

        xtra.toCharArray(nxtspc, 2);
        //Serial.print("nxtspc array is: ");
        //Serial.print(nxtspc); Serial.println("_end");

        //Serial.println(screenSt);
        screenSt.toCharArray(line, 21); // translate that string to a char array ; 21 is length, not indice. leave the last indice to be 0
        myFile.close();
        //Serial.print("line fresh off the press: ");
        //Serial.print(line); Serial.println("_end");


      }// while

      myFile.close();

      // now let's address what we've got and break it up properly to fit on the screen without breaking in the middle of a word

      // Serial.println();
      // if the next char is a space, or puncuation '.' or a ','
      if ( seekFile != 1 && peekFile != 1) {
        //Serial.println(F("Addressing line to find breakpoint."));

        if (isSpace(nxtspc[0]) || (nxtspc[0] == 44) || (nxtspc[0] == 46)  ) { //  nxtspc[0] == 46) || nxtspc[0] == 44 ) { // the equiv to a space
          // this will be added to the current indice point, and when read again will be the new start point
          indice = 21; // not the next item in the array, but where we should *read from for next time*. skip the space since at the end of a line **check for issues
          // copy from that point to beginning to form the line
          char line0[21]; // making this one larger since space is at breaking point of screen line. AND need the null char **

          strncpy(line0, line, indice - 1); // leave this at indice -1, until more problems come up
          // Serial.print("Line for the LCD: ");
          // Serial.println(line0);
          // Serial.println();
          lcd.print(line0);
          //break; // break out of for loop  <-- does this break all the way out? YES
        } // if

        // if we are at a space
        else if (isSpace(line[19]) || line[19] == 46 || line[19] == 44) {  // , = 44; .= 46
          indice = 20; // not the next item in the array, but where we should *read from for next time*. skip the space since at the end of a line **check for issues
          // copy from that point to beginning to form the line
          char line0[21]; // making this one larger since space is at breaking point of screen line. AND need the null char **

          strncpy(line0, line, indice + 1); // leave this at indice -1, until more problems come up
          // Serial.print("Line for the LCD   : ");
          // Serial.println(line0);
          // Serial.println();
          lcd.print(line0);
        }

        // if we are in the middle of a word, digit, including apostrophes
        else if ( ( isAlpha(nxtspc[0]) || (nxtspc[0] == 39) || isDigit(nxtspc[0]) )  && ( isAlpha(line[19]) || (line[19] == 39) || isDigit(line[19]) )  ) {
          // first clear out the array we are going to print from
          for (int i = 0; i < sizeof(line0) - 1; i++) {
            line0[i] = ' ';
          }
          // Serial.print("line is before crunching: "); Serial.println(line);
          // Serial.println();
          // otherwise, cycle backwards through the array to find the space between words
          for (int i = 19; i > 0; i--) {  // starting at indice 19. indice 20 is the null char required for string arrays
            char test;
            test = line[i];
            if (isSpace(test)) { // for some reason can't join this
              indice = i; // is this getting attached to newindice correctly ??
              for (int i = 19; i > indice; i--) {
                line[i] = ' ';
              }
              indice++;
              char line0[21];
              strncpy(line0, line, sizeof(line0) / sizeof(char));
              //     Serial.print("Line for the LCD: ");
              //     Serial.println(line0);
              //     Serial.println();
              lcd.print(line0);
              break;
            }
          } // for
        }// if we are in the middle of a word

        else if (isAlpha(line[19]) && isPunct(nxtspc[0]) ) { // trying to catch when there is a line, but the last line is small

          indice = 20;


          // Serial.println(indice);

          char line0[21]; // making this one larger since space is at breaking point of screen line. AND need the null char **
          strncpy(line0, line, indice);
          // Serial.print("Line for the LCD  , : ");
          // Serial.println(line0);
          // Serial.println();
          lcd.print(line0);
          //indice++;
          //break;

        }

        else if (isCrack(line[19])) {
          bool lastChar = 0;
          // Serial.println(F("looping bckwrds thru crap "));
          for (int i = 19; i >= 0; i--) {  // starting at indice 19. indice 20 is the null char required for string arrays
            char test;
            test = line[i];

            if (isCrack(test) ) {
              lastChar = 1; // we are the mucked up symbol
            }
            else if (lastChar == 1) {
              lastChar = 0;
              indice = i;
              //  Serial.print(F("found info at: ")); Serial.println(indice);
              break;
            }

          }
          // Serial.println(F("we broke"));
          indice++; // move this above the break, don't get bearclaw on 25. otherwise do, but on #7 get last period. strange.

          // Serial.println(indice);

          // char line0[21]; // making this one larger since space is at breaking point of screen line. AND need the null char **
          strncpy(line0, line, indice);
          // Serial.print("Line for the LCD: ");
          // Serial.println(line0);
          // Serial.println();
          lcd.print(line0);

          break;

        }
        // not sure about below still needed / working properly
        else if (isPunct(line[19]) || isAlphaNumeric[19]) {

        }

        else if (peekFile == 0 && seekFile == 0) { // trying to catch when there is a line, but the last line is small
          Serial.println(F("in peek + seek"));
          bool lastChar = 0;
          for (int i = 19; i > 0; i--) {  // starting at indice 19. indice 20 is the null char required for string arrays
            char test;
            test = line[i];
            if (!isAlphaNumeric(test) && !isSpace(test) && !isDigit(test) && !isPunct(test) ) {
              lastChar = 1; // we are the mucked up symbol
            }
            else if (lastChar == 1) {
              lastChar = 0;
              indice = i;

              break;
            }
            //Serial.println(lastChar);


          }
          // Serial.println(F("we broke"));
          // Serial.println(line[19]);
          // Serial.println();
          indice++; // move this above the break, don't get bearclaw on 25. otherwise do, but on #7 get last period. strange.

          // Serial.println(indice);

          // char line0[21]; // making this one larger since space is at breaking point of screen line. AND need the null char **
          strncpy(line0, line, indice);
          //  Serial.print("Line for the LCD: ");
          //  Serial.println(line0);
          //  Serial.println();
          lcd.print(line0);

          break;

        }




        else if (nxtspc[0] == 0 && endFile == 1 && ( isAlpha(line[19]) || (isPunct(line[19]) || isDigit(line[19]) ) ) && peekFile == 1 ) { // we are at the last line of the message
          if (line0[19] != 42) {
            Serial.println(F("Yes, triggered endfile, zeroing out")); // Serial.println("nxtspc is BLANK");
            char line0[21]; // making this one larger since space is at breaking point of screen line. AND need the null char **
            strncpy(line0, line, sizeof(line0) / sizeof(char));
            //   Serial.print("Line for the LCD: ");
            //   Serial.println(line0);
            //  Serial.println();
            lcd.print(line0);
            indice = 0;
            peekFile = 0;
            line0[19] = 42;
            break;  // we are finished with this message
          }
        }



      }
      else if (peekFile == 1 && endFile == 1 && line0[19] == 42 && seekFile == 1) {
        // Serial.println("Yes, triggered endfile, zeroing out second laps");
        // line = 0;
        char line0[21]; // making this one larger since space is at breaking point of screen line. AND need the null char **
        // file is dead, so don't need to print out any more lines
        // strncpy(line0, line, sizeof(line0) / sizeof(char));
        for (int l = 0; l < sizeof(line0) - l; l++) {
          line0[l] = ' ';
        }
        // Serial.print("Line for the LCD: ");
        // Serial.println(line0);
        // Serial.println();
        // lcd.print(line0);  //<--- keeps repeating the last couple line like crazy, then shortens. something w/flags, indice, # of screens, breaks
        // seekFile = 0; // ?
        // peekFile = 1;
        break;  //  or print a clear line
      }


      int(myFile) = 0;
    }// else
    //Serial.print(i + 1);
    // Serial.println("  line completed");
    // Serial.println();
  } // for // end for - loop for one screen
  int duration = ceil(lag * pS);
  Serial.print(F("lag is: "));
  Serial.println(duration);
  delay(duration); // change this back to five

  // once we have 4 lines, pause
} // main loop


bool alreadyShown(int msg) {

  // if statement or variable for first time as 0 slot
  for (int i = 1; i < numFiles + 1; i++) {
    if (storageH[i] == msg) {
      // Serial.println("I've been seen already");
      //break;
      return 1;
    }
  }
  return 0;

}


void storageCheck(int indice) {
  // Serial.println();
  Serial.println(F("checking storage"));
  // Serial.println();
  // store the file in the next empty slot.
  for (int i = 1; i < numFiles + 1; i++) {
    if (storageH[i] == 0) { // need to check this about 1st slot & 0 showing
      storageH[i] = indice; // if the storage is empty, put the value in there
      capacity++;
      Serial.println(F("storaging"));
      //  Serial.println();
      break;
    }
  }
  /*

    int capacity = 0;
    for (int i = 1; i < numFiles + 1; i++) {
    Serial.print("checking files to see if at capactiy");
    Serial.println();
    //if this slot isn't equal to zero, increment capacity
    if (storageH[i] != 0) {
      capacity++;
      Serial.print("nope, increasing count");
      Serial.println();
    }
    }
  */
  // this resets things if we have shown everything
  // otherwise if we've run through all of the files, double check to see if we have truly done so
  if (capacity == numFiles) { // 14 slots, but can't add 0, the first slot. 13 images
    for (int i = 1; i < numFiles + 1; i++) {
      storageH[i] = 0;
    }
    Serial.print(F("at capcity, zeroing out"));
    //Serial.println();
    //Serial.println();
    capacity = 0;
    // select a new file??
  }

}


bool isCrack(char ick) {
  //Serial.println(F("Am I crap?"));
  if (!isAlphaNumeric(ick) && !isSpace(ick) && !isDigit(ick) && !isPunct(ick)) {
    // Serial.println(F("yes"));
    return 1;

  }
  else {
    // Serial.println(F("no"));
    return 0;


  }
}

