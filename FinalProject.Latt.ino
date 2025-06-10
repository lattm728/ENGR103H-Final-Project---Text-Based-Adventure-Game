/* SOME NOTES:
  This might take a little while to upload, so please be patient!
  This game utilizes the serial monitor mainly, as well as the NeoPixels on the board and the speaker!
  Please keep that in mind while you play. 
  If you cannot play with the speaker, then unfortunately the best way to turn it off is to go in and comment out all the code
  that is listed as "CircuitPlayground.playTone".
  (I recommend you ctrl-f for that, there is a lot of code in here.)
  Good luck, and have fun!
*/

#include <Adafruit_CircuitPlayground.h> 

// ISR Flags
volatile bool switchFlag = false;
volatile bool leftFlag = false;
volatile bool rightFlag = false;
volatile bool switchState = 0;

// Pins
const int switchPin = 7;
const int rightPin = 5;
const int leftPin = 4;

// Notes
const float aFlat = 415.30;
const float c = 261.63;
const float eFlat = 311.13;
// yknow I don't really have a place to test these

// Progress / Looping Flags
volatile bool lockedPrisonFlag = true;
volatile bool openPrisonFlag = false; // when you break out and reenter
volatile bool bridgeFlag = false;
volatile bool deckFlag = false;
volatile bool smashFlag = false; // ignore the name
  // dialogue flags:
volatile bool maxDialogueFlag = false;
volatile bool flintDialogueFlag = false;
volatile bool superDialogueFlag = false;
volatile bool stinkyFlag = false;
  // fight flags:
volatile bool fightFlag = false;
volatile bool winFlag, loseFlag, defendFlag, enemyTurnFlag, oneLinerFlag = false; // can I stack them
// note that winFlag should be used to check if you've done the fight successfully or not

// Item Flags
volatile bool waterGunFlag = false;
  // three parts needed to beat skunkape:
volatile bool spaceWarperFlag = false;
volatile bool magneticBaseFlag = false;
volatile bool powerCoreFlag = false;
volatile bool wormHoleGeneratorFlag = false;


// Counts for Repeated Dialogue and the Fight
int pageCount = 0; // starting at the beginning
int maxCount = 1;
int flintCount = 1;
// no superball count because talks as soon as you enter the bridge and checks your progress
int searchCount = 1;
int deckCount = 1;

int samHitPoints = 18;
int enemyHitPoints = 20;
long randomEnemyDamage = 0; // I want it to roll a new number each time
long randomSamDamage = 0;
long randomMaxDamageChance = 0; // to see if he attacks
int maxDamage = 1000000; // he'll always deal 1,000,000 damage. why? cause it's funny

// ISRs Delays
volatile unsigned long lastLeftPress = 0;
volatile unsigned long lastRightPress = 0;
volatile unsigned long lastSwitchChange = 0;
const unsigned long debounceTime = 200; // adjust as needed


void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }
  CircuitPlayground.begin();

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(leftPin, INPUT_PULLDOWN); // whatever this is
  pinMode(rightPin, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(leftPin), leftISR, RISING);
  attachInterrupt(digitalPinToInterrupt(rightPin), rightISR, RISING);

  switchState = digitalRead(switchPin);

  randomSeed(analogRead(1)); // if this doesn't work, try a different pin

  lockedPrisonFlag = true;
}
// ISRs are up here because I need to be able to see them
void switchISR() {
  unsigned long now = millis();
  if (now - lastSwitchChange > debounceTime) {
    switchState = digitalRead(switchPin);
    switchFlag = true;
    lastSwitchChange = now;
  } 
  // these edits to the ISRs were suggested by ChatGPT. I was having issues with the buttons being pressed twice.
  // This fixes it so that if it triggers twice while the flag is turns on and then off for a dialogue options, it'll correct it to only pressing once.
  // I used this for all the ISRs, so this one, the left button and the right button ISRs.
}

void leftISR() {
  unsigned long now = millis();
  if (now - lastLeftPress > debounceTime) {
    leftFlag = true;
    lastLeftPress = now;
  }
}

void rightISR() {
  unsigned long now = millis();
  if (now - lastRightPress > debounceTime) {
    rightFlag = true;
    lastRightPress = now;
  }
}

void loop() {
  switchState = digitalRead(switchPin);
  delay(100);


  switch(pageCount) {
    case 0: // locked prison
      lockedPrisonFlag = true; // we'll see if this messes it up
      startUpDialogue();
      lockedPrisonOptions();
      lockedPrisonSpaceShip();

      skunkApeEntrance();
      // includes both skunkape's monologue and superball's exposition
      break;

    case 1: // ship's bridge
      bridgeFlag = true; 
      bridgeOptions();
      bridgeSpaceShip();

      break;

    case 2: // ship's deck
      if (deckCount == 1) {
        deckStartUp();
      } // explains the deck
      deckFlag = true; 
      deckOptions();
      deckSpaceShip();
      break;

    case 3: // unlocked prison
      openPrisonFlag = true;
      openPrisonOptions(); 
      openPrisonSpaceShip();

      break;
    case 4: // skunk ape smash!!!
      smashFlag = true;
      skunkApeSmash();
      break;
    
    case 5:
      delay(50);
      break;
      // holds at the end of case 4, but just in case
  }
}


void startUpDialogue() { // dialogue only, no options
  Serial.println("WELCOME TO MY GAME!");
  Serial.println("Everything, from dialogue to options, will be printed here on the Serial Monitor.");
  Serial.println("In order to select and option, please orient your Circuit Plaground board with the cord on the top of the circle.");
  Serial.println("When the switch is to the left, then the left and right buttons will correspond to A and B.");
  Serial.println("When the switch is to the right, then the left and right buttons will correspond to C and D.");
  Serial.println("If you reset the board, you will lose all of your progress.");
  Serial.println("Hope you enjoy!");
  Serial.println("Welcome to SAM & MAX in DESTRUCTION FOR ALL AGES!*");
  Serial.println("*As seen in the hit documentary ''SAM & MAX: FREELANCE POLICE' and ENCOURAGING VIOLENCE to CHILDREN'");
  Serial.println();

  Serial.println("You are playing as SAM, a 6 foot tall talking dog in a suit and hat. You wake up to find yourself lying on the cold ground.");
  Serial.println("SAM's partner MAX, a kinetic (though not right now) white lagomorph, lies on the ground next to, slowly stumbling onto his giant rabbit feet.");
  Serial.println("SAM: MAX! Are you alright??");
  Serial.println("MAX: Yeah, SAM, I'm alright! ... Where are we?");
  Serial.println("Now that you're standing up, you and MAX seem to be trapped in a small, metallic room. There are a few box in the corner, and a suspicious looking 'window' on one wall.");
  Serial.println("You can see purple wiring running up and down the walls, and a gorrila-shaped logo...");
  Serial.println("SAM: That's a good question little buddy! ...Wait, is that...");
  Serial.println("MAX: We're in SKUNKAPE's spaceship!");
  Serial.println("SAM: Not again! Quick, let's find a way out of here!");
  Serial.println();
}

void lockedPrisonSpaceShip() {

  while (lockedPrisonFlag == true) { // only turns false when you break out of the room

    while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }

    switch (switchState) { 
      case 1: // if the switch is to the left, which is technically normally the right
      
        if (leftFlag) { // A: Bang on the Door
          leftFlag = false; // note to always have this first so it doesn't trigger two button presses
          Serial.println("You bang on the door. Nothing happens.");
          Serial.println("SAM: We know it's you, SKUNKAPE!");
          Serial.println();
          delay(50);
          break;

        } else if (rightFlag) { // B: Check the 'Window'
          rightFlag = false; // idk if this needs the else if here but sure
          Serial.println("You check the window. It seems to be just a poster. Behind the poster is a keycard with a note attached to it.");
          Serial.println("The note reads: 'JERRY, use this in case you get stuck in here again! Love, MICHAEL'.");
          Serial.println("SAM: Aw, that's sweet.");
          Serial.println("MAX: HAHA, yes! Let's use it!");
          Serial.println("You use the key card on a slot next to the door. It unlocks and opens up.");
          Serial.println();
          // MAX: HAHA, yes! Let's use it!
          // (jerry keeps getting locked in there, so they need to keep a keycard in case he gets locked in again)
          lockedPrisonFlag = false;
          
          delay(50);
          break;

        } else { // this might be unnecessary
          delay(50);
          switchState = digitalRead(switchPin);
        } // note to self to test this eventually

      case 0: // if the switch is to the right
        if (leftFlag) { // C: Search the Room
          leftFlag = false;
        // this will need to played twice in order to find the proper weapon for the fight 
        // why twice? because I'm bored and this is a bad game
          if (searchCount == 1) {
            // don't find very much
            Serial.println("You search the room vaguely, looking for something to help you.");
            Serial.println("MAX: Whatcha looking for, SAM?");
            Serial.println("SAM: Something to help us, melonhead! I'm not having any luck, though.");
            Serial.println("MAX: Ooooh, lemme look! I'm great at finding weapons!!");
            Serial.println();
          } else if (searchCount == 2) {
            // find the weapon
            Serial.println("MAX tears apart the room and finds the WATER GUN.");
            Serial.println("SAM: ... I dunno if this will help us very much, MAX.");
            Serial.println("MAX: I thought I was the forgetful one in the relationship, SAM! You know FLINT hates being sprayed with water!");
            Serial.println("SAM: You're right, little buddy! This will help us beat FLINT in a fight.");
            Serial.println();
            Serial.println("You've obtained the WATER GUN!");
            Serial.println();
            // insert fun stuff here (aka LEDs)
            for (int i = 0; i < 10; i++) {
              CircuitPlayground.setPixelColor(i, 0, 30, 200); // teal ish
              delay(10);
            }
            CircuitPlayground.playTone(aFlat, 200);
            CircuitPlayground.playTone(c, 200);
            CircuitPlayground.playTone(eFlat, 200);
            // A flat, C, E flat
            delay(1000);
            CircuitPlayground.clearPixels();
            
            waterGunFlag = true;
          } else {
            // so you don't "find the weapon" again
            Serial.println("SAM: I don't think there's anything else useful in here.");
          }

          searchCount++;
        
          delay(50);
          break;

        } if (rightFlag) { // D: Talk to Max
        // I generally want this available, but it doesn't have to be available in all situations. 
        // if you need a hint, you can go to some place where you can talk to max to get a hint (this room is accessible later)
        // I wanna head back here, so I'll keep locked / openPrisonFlag true
          rightFlag = false; 
          maxDialogueStartUp();
        
          delay(50);
          break;
        
        } else { // this might be unnecessary
          delay(50);
          switchState = digitalRead(switchPin);

        }
    }
    if (lockedPrisonFlag == true) {
      lockedPrisonOptions(); // to print options again
    }
  } // end of while loop
 // Serial.println("Heading back to loop...");
  delay(50);
  // will break if the flag is off, heading back to loop
}

void lockedPrisonOptions() {
  delay(50);
  
    Serial.println("A: Bang on the Door");
    Serial.println("B: Check the 'Window'");
    if (searchCount == 2) {
      Serial.println("C: Let MAX 'Search' the Room");
    } else  { // 1 or greater than 2 
      Serial.println("C: Search the Room");
    }
    Serial.println("D: Talk to MAX");
    Serial.println();
}

void openPrisonOptions() {
  Serial.println("A: Ruminate");
  Serial.println("B: Head to the Bridge");
  if (searchCount == 2) {
    Serial.println("C: Let MAX 'Search' the Room");
  } else  { // 1 or greater than 2 
    Serial.println("C: Search the Room");
  }
  Serial.println("D: Talk to MAX");
  Serial.println();
}

void openPrisonSpaceShip() {
   while (openPrisonFlag == true) {
    // what happens when you go  back in here

    // reminder to exactly copy and paste case 0 in here, they will be the exactly the same

        while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }

    switch (switchState) { 
      case 1: // switch is left
      
        if (leftFlag) { // A: Ruminate
          leftFlag = false; 
          Serial.println("SAM: This room reminds me of the last time we were trapped in SKUNKAPE's spaceship, except with less trauma!");
          Serial.println("MAX: And no more pyschic powers...");
          Serial.println("SAM: Considering what happened last time, maybe it's for the best, little buddy.");
          Serial.println();
          delay(50);
          break;

        } else if (rightFlag) { // B: Head to the Bridge
          rightFlag = false; 
          // dialogue
          Serial.println("Heading to the bridge...");
          Serial.println();
          openPrisonFlag = false;
          pageCount = 1;
          delay(50);
          break;

        } 

      case 0:  // switch is right
        if (leftFlag) { // C: Search the Room
          leftFlag = false;
        // this will need to played twice in order to find the proper weapon for the fight 
        // why twice? because I'm bored and this is a bad game
          if (searchCount == 1) {
            // don't find very much
            Serial.println("You search the room vaguely, looking for something to help you.");
            Serial.println("MAX: Whatcha looking for, SAM?");
            Serial.println("SAM: Something to help us, melonhead! I'm not having any luck, though.");
            Serial.println("MAX: Ooooh, lemme look! I'm great at finding weapons!!");
            Serial.println();
          } else if (searchCount == 2) {
            // find the weapon
            Serial.println("MAX tears apart the room and finds the WATER GUN.");
            Serial.println("SAM: ... I dunno if this will help us very much, MAX.");
            Serial.println("MAX: I thought I was the forgetful one in the relationship, SAM! You know FLINT hates being sprayed with water!");
            Serial.println("SAM: You're right, little buddy! This will help us beat FLINT in a fight.");
            Serial.println();
            Serial.println("You've obtained the WATER GUN!");
            Serial.println();
            // insert fun stuff here (aka LEDs)
            for (int i = 0; i < 10; i++) {
              CircuitPlayground.setPixelColor(i, 0, 30, 200); // teal ish
              delay(10);
            }
            CircuitPlayground.playTone(aFlat, 200);
            CircuitPlayground.playTone(c, 200);
            CircuitPlayground.playTone(eFlat, 200);
            // A flat, C, E flat
            delay(1000);
            CircuitPlayground.clearPixels();

            waterGunFlag = true;
          } else {
            // so you don't "find the weapon" again
            Serial.println("SAM: I don't think there's anything else useful in here.");
            Serial.println();
          }

          searchCount++;
        
          delay(50);
          break;

        } if (rightFlag) { // D: Talk to Max
          rightFlag = false; 
          maxDialogueStartUp();
        
          delay(50);
          break;
        
        } else { // this might be unnecessary
          delay(50);
          switchState = digitalRead(switchPin);

        }
    } // end of switch
  if (openPrisonFlag == true) {
    openPrisonOptions();
  }
  } // end of while 
}

void maxDialogueStartUp() {
  // turn off button flag
  rightFlag, leftFlag = false; // just in case

  if (maxCount == 1) { // on first time only
    Serial.println("SAM: MAX, I'm glad I'm here with you on another adventure!");
    Serial.println("MAX: Stop it, SAM, you're making me blush!");
    Serial.println();
    delay(50);

  } else { // every other time
    Serial.println("MAX: How's it cookin', good lookin'?");
    Serial.println();
    delay(50);
  } // you can add more dialogue things that happen, whether random or plays the fifth time or whatever

  maxCount++; // why startup doesn't repeat every time
  maxDialogueFlag = true;
  maxDialogue();
}

void maxDialogue() {
  while (maxDialogueFlag == true) { // breaks out of loop once the flag is turned off
    // insert options here and stuff
    Serial.println("A: SKUN'KA'PE");
    Serial.println("B: MAX");
    Serial.println("C: Hint");
    Serial.println("D: Leave");
    Serial.println();

    while (!leftFlag && !rightFlag) { // good old delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }

    switch (switchState) {
      case 1: // left switch
        if (leftFlag) { // A: SKUN'KA'PE
          leftFlag = false;
          Serial.println("MAX: Y'know, this is reminding me of our previous adventures! Almost a little too close to our previous adventures...");
          Serial.println("SAM: Don't think about it too hard, MAX. SKUNKAPE is back and we need to defeat him again!");
          Serial.println("MAX: Oh goodie! His theme music slaps!");
          Serial.println();
          delay(50);
          break;

        } else if (rightFlag) { // B: MAX
          rightFlag = false;
          Serial.println("SAM: How are you feeling, little buddy?");
          Serial.println("MAX: Ready to beat up SKUNKAPE again!");
          Serial.println("SAM: Glad your violent tendecies haven't been affected by the gravity of this situation!");
          Serial.println();
          delay(50);
          break;
        }
      case 0: // right switch
        if (leftFlag) { // C: Hint
          leftFlag = false;

          if (lockedPrisonFlag == true) { 
            // have you checked the fake window?
            // this one can reasonably be left by itself. the main goal here is to get out of the room, and it's literally just done by checking the window
            Serial.println("MAX: Wow, that 'window' looks super nice and not at all like there's something behind it!");
            Serial.println();

          } else if (wormHoleGeneratorFlag == true) {
            // takes priority
            Serial.println("MAX: Let's go beat up SKUNKAPE! Oooh, I'm so excited!");
            Serial.println();

          } else if (magneticBaseFlag && powerCoreFlag) {
            // chat with superball
            Serial.println("MAX: SAM, I think we got all of the pieces for SUPERBALL's worm thing!");
            Serial.println("SAM: WORMHOLE GENERATOR, little buddy. But you're right, let's go chat with Superball!");
            Serial.println();

          } else if (!lockedPrisonFlag && !powerCoreFlag && !waterGunFlag && loseFlag) {
            // have you checked around the room thoroughly?
            // might need to add another if statement in here so this doesn't play when you're trying to figure out the other part
            // I do want this to only play if you've tried the fight before without the weapon
            Serial.println("MAX: SAM, you know I have an eye for weapons! Let's go back to the cell to look for a weapon to help you against FLINT!");
            Serial.println("SAM: I can take him just fine in a bought of fisticuffs! ... It just might take a couple tries.");
            Serial.println("MAX: That's why we gotta find you something to help!");
            Serial.println();

          } else if (!stinkyFlag && !magneticBaseFlag && !loseFlag && !powerCoreFlag) {
            // recommends stinky first because it's low effort
            Serial.println("MAX: Can we please go bother STINKY? Please, pretty please?");
            Serial.println();

          } else if (stinkyFlag && !magneticBaseFlag && !loseFlag && !powerCoreFlag) {
            // go to the supply closet
            Serial.println("MAX: I don't normally trust STINKY, SAM, but I think she wants out of here as much as we do.");
            Serial.println("SAM: MAX, I'm inclined to be wary, but I think we better check out that supply closet to find what she wants.");
            Serial.println();

          } else if (magneticBaseFlag && !powerCoreFlag && !winFlag) {
            // go chat with flint
            Serial.println("MAX: OMG SAM, FLINT PAPER is here! He's so cool!! Let's go talk with him!!!");
            Serial.println();

          } // I think that's enough hints, also jesus christ

          delay(50);
          break;

        } if (rightFlag) { // D: Leave
          rightFlag = false;
          // leaving dialogue
          Serial.println("SAM: Stay safe out there, little buddy.");
          Serial.println("MAX giggles.");
          Serial.println();
          maxDialogueFlag = false;
          break;

        }
    } // switch
  } // while

  // once you clear the loop, head back to wherever you started
  // so if you were in the room 
  /* aka if (lockedPrisonSpaceShipFlag == true) then head back to those options*/
  // I'm telling you to use flags to head back to the right function
}

void skunkApeEntrance() {
  // insert dialogue here
  Serial.println("You exit the cell to find two gorilla-aliens standing in guard in front of the door. They block your path until you hear a booming voice coming from the deck.");
  Serial.println("SKUN'KA'PE: If it isn't SAM & MAX, petty Earth freelance police. I, GENERAL SKUN'KA'PE, have captured you again!");
  Serial.println("SKUN'KA'PE: This is because I am very clever and evil, and I have very good theme music.");
  Serial.println("SKUN'KA'PE: (https://youtu.be/bfTalTK-6vA?si=r59glHJQg16sdKin)");
  Serial.println("SAM: SKUNKAPE! What are you doing alive and back on Earth?");
  Serial.println("SKUN'KA'PE: You thought your puny Earth nukes could kill me? Pah!");
  Serial.println("SKUN'KA'PE: I am here to destroy your Seven Wonders of the World as the first step towards erasing Terran-kind for good! You are nothing but stains on this beautiful galaxy!");
  Serial.println("MAX: Which ones? You mean like... Bosco's hot dogs?");
  Serial.println("SAM: No, knucklehead, he means like... uh... Mount Everest?");
  Serial.println("SKUN'KA'PE: No, I am destroying... your monoliths and such... like... oh! Those blasted Great Pyramids of yours!");
  Serial.println("SKUN'KA'PE: Those pitiful things do not even compare to Q'onos's Icositetragons!");
  Serial.println("MAX: Oh, like the Pyramids!");
  Serial.println("SKUN'KA'PE: Yes, like the Pyramids. I have captured you so you do not interfere with my plans this time.");
  Serial.println("SKUN'KA'PE: Ask inane questions and open dialogue trees all that you like, you don't have any chance to stop me!");
  Serial.println("With that, SKUN'KA'PE's lackeys stop blocking you, and you find yourself on the bridge of the spaceship.");
  Serial.println();
  Serial.println("Glancing around, you see STINKY, a mermaid-turned-chef-turned-attempted-murderer-of-her-grandpa next to a supply closet.");
  Serial.println("You also spot SUPERBALL, former Secret Service Agent to MAX when he was president, and now current President of the United States.");
  Serial.println("SUPERBALL: Good to see you sirs. If I could speak to you for a moment, I believe I have a way for you to stop GENERAL SKUN'KA'PE.");


  // Serial.println("SKUNKAPE: I am evil, yes. I also have very good theme music.");
  Serial.println();

  // I'd also like to add superball's debriefing dialogue here too
  // so that way you get the information before you can go off and do other things
  // because I'm mean like that
  // and so I don't have to add more fucking if statements
  // ... I might add a small little wait with one option and that is to talk to super ball
  // because otherwise that's a shit ton of dialogue
  // yeah I'll do that

  Serial.println("A: Debrief with SUPERBALL");
  Serial.println();

   while (!leftFlag && !rightFlag) { 
      switchState = digitalRead(switchPin); 
      delay(50);
    } // you cannot leave if you do not talk with superball
    // this should only run once ever, and that is right after breaking out of the cell
    // this would only run multiple times if I ever added this functions somewhere accidentally or set the page number to 0 

    switch(switchState) {
      case 1: 
        if (leftFlag) { // A: Debrief with Superball
          leftFlag = false;
          Serial.println("You walk over to SUPERBALL.");
          Serial.println("SUPERBALL: Good afternoon, sirs.");
          Serial.println("MAX: Supes!! My favorite former Secret Service Agent and second favorite President! Besides me, of course!");
          Serial.println("SAM: President SUPERBALL! How'd you get on SKUNKAPE's spaceship?");
          Serial.println("SUPERBALL: I am here to stop GENERAL SKUN'KA'PE from destroying the Seven Wonders of the World. It is very important to me as POTUS to protect Earth's history.");
          Serial.println("SAM: Uh huh...");
          Serial.println("SUPERBALL: The golf course under Machu Picchu also has great martinis.");
          Serial.println("MAX: Oh! I love that place!");
          Serial.println("SAM: Under Machu Picchu? How come I never heard about this, MAX?");
          Serial.println("MAX: They kicked me out for eating the golf balls! Also, I thought it was under the McDonald's on Ninth!");
          Serial.println("SUPERBALL: Let's get to business, sirs. In order to stop SKUN'KA'PE and get off this spaceship, we must send him through a wormhole.");
          Serial.println("MAX: Ooooh, very sci-fi! I hope he gets Borg-ified!");
          Serial.println("SAM: A wormhole... and how do we do that, SUPERBALL?");
          Serial.println("SUPEBALL: You must collect the three parts for the WORMHOLE GENERATOR. I have the SPACETIME WARPER here.");
          delay(500);
          Serial.println();
          Serial.println("You've obtained the SPACETIME WARPER!");
          // insert LEDs here
          for (int i = 0; i < 10; i++) {
              CircuitPlayground.setPixelColor(i, 255, 0, 30); //  pink (hopefully)
              delay(10);
          }
            CircuitPlayground.playTone(aFlat, 200);
            CircuitPlayground.playTone(c, 200);
            CircuitPlayground.playTone(eFlat, 200);
            // A flat, C, E flat
          delay(1000);
          CircuitPlayground.clearPixels();
          spaceWarperFlag = true;

          Serial.println();
          delay(500);
          Serial.println("SUPEBALL: You must get two more parts to make the GENERATOR: the MAGNETIC BASE and the POWER CORE.");
          Serial.println("SUPERBALL: STINKY over there has the MAGNETIC BASE, and FLINT PAPER down on the deck has the POWER CORE.");
          Serial.println("SAM: So we need to get them to give us those parts, right?");
          Serial.println("SUPERBALL: Yes sirs. Talk to me when you've collected those parts. I can also tell you your progress and where you need to go if you are stuck.");
          Serial.println("MAX: Oooooh, a fetch quest! I hope it's more than just random dialogue!");
          Serial.println("SAM: C'mon little buddy, let's go get those parts!");
          Serial.println();
          
          delay(50);
          break;
        }
    }

  pageCount = 1; // should head to the bridge
}

void bridgeOptions() {
  Serial.println("A: Debrief with SUPERBALL");
  if (stinkyFlag == true) {
    Serial.println("B: Check Supply Closet");
  } else {
    Serial.println("B: Look Around");
  }
  Serial.println("C: Annoy STINKY");
  Serial.println("D: Go Somewhere Else");
  Serial.println();
}

void bridgeSpaceShip() {
  while (bridgeFlag == true) {

    while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }

    switch (switchState) { 
      case 1: // left switch
      
        if (leftFlag) { // A: Debrief with SUPERBALL
          leftFlag = false; 
          superDialogueFlag = true;
          superBallDialogue();
          delay(50);
          break;

        } else if (rightFlag) { // B: Look Around / Check Supply Closet
          rightFlag = false; 

          if (stinkyFlag == true) {
            // grab something that's def not nice for skunkape

            if (magneticBaseFlag == false) {
              Serial.println("You search the supply closet, and find a small BOX with a label that says DO NOT OPEN.");
              Serial.println("It shakes a little bit, and seems to be making disconcerting snarling noises.");
              Serial.println("MAX: SAM this is awesome! Can we open it up?");
              Serial.println("SAM: ... I'd rather not, knucklehead. Let's take this to STINKY.");
              Serial.println("You head back to give STINKY the BOX.");
              Serial.println();
              delay(500);
              Serial.println("STINKY takes the BOX. She holds out the MAGNETIC BASE.");
              Serial.println("STINKY: Here, go beat up SKUN'KA'PE or something.");
              Serial.println("SAM: Good luck with your... BOX...");
              Serial.println("MAX: Wish we could stick around to see that!");
              Serial.println();

              magneticBaseFlag = true;
              Serial.println("You've found the MAGNETIC BASE!");
              // leds here
              for (int i = 0; i < 10; i++) {
              CircuitPlayground.setPixelColor(i, 30, 0, 255); //  purple (hopefully)
              delay(10);
              }
              CircuitPlayground.playTone(aFlat, 200);
              CircuitPlayground.playTone(c, 200);
              CircuitPlayground.playTone(eFlat, 200);
              // A flat, C, E flat
              delay(1000);
              CircuitPlayground.clearPixels();
              
              Serial.println();
            }
            if (powerCoreFlag && spaceWarperFlag && magneticBaseFlag) {
              // insert leds here, reminder that this will be the same as the other one down in the fight win screen
              for (int i = 0; i < 10; i++) {
                CircuitPlayground.setPixelColor(i, 255, 255, 255); // white
                delay(10);
              }
              Serial.println("You've collected all the parts for the WORMHOLE GENERATOR! Go back and talk to SUPERBALL on the bridge!");
              Serial.println();
              delay(1000);
              CircuitPlayground.clearPixels();
            }
            stinkyFlag = false; // clears it

          } else { // regular dialogue
            // looking around, talking about bridge or whatever
            Serial.println("There's not really anything interesting around here except the way you came from, which was the cell, ");
            Serial.println("STINKY, over by the supply closet, and SUPERBALL.");
            Serial.println();
          }

          delay(50);
          break;

        } else { 
          delay(50);
          switchState = digitalRead(switchPin);
        } 

      case 0: // switch right
        if (leftFlag) { // C: Annoy STINKY
          leftFlag = false;
          // tells you to grab her something from the supply closet
          // idk it's that easy
          if (magneticBaseFlag == false) {
            Serial.println("STINKY: If it isn't Abbott and Costello.");
            Serial.println("STINKY rolls her eyes.");
            Serial.println("SAM: Hiya STINKY, delightful to see you too, we hear you have a MAGNETIC BASE?");
            Serial.println("STINKY: I don't want to be here more than you guys do. I only worked with SKUN'KA'PE before to ...deal... with my grandpa.");
            Serial.println("MAX: *stage whispsers* She tried to kill GRANDPA STINKY last time we saw her! It wasn't cool! I like his corndogs!!");
            Serial.println("SAM: Then what are you doing here now?");
            Serial.println("STINKY: Trying to get back to Earth. If you get me the BOX from the supply closet, I'll give you the MAGNETIC BASE.");
            Serial.println("MAX: Oooh, what's in the BOX?");
            Serial.println("STINKY: You don't want to know, Grogu. I need it to get out of here. And since you guys are *unfortunately* my best shot out of here...");
            Serial.println();
            Serial.println("MAX: I don't trust her, SAM!");
            Serial.println("SAM: I know you don't, little buddy, but we need that MAGNETIC BASE!");
            Serial.println();
            stinkyFlag = true;

          } else if (magneticBaseFlag == true) {
            Serial.println("STINKY: Leave me alone.");
            Serial.println("SAM: Got it.");
            Serial.println();
          }
          delay(50);
          break;

        } if (rightFlag) { // D: Go Somewhere Else
          rightFlag = false; 
          leavingBridge();
          delay(50);
          break;
        
        }
    }
    if (bridgeFlag == true) {
      bridgeOptions(); // to print options again
    }
  } // end of while loop
}

void leavingBridge() {
  Serial.println("Where would you like to go?");
  Serial.println();
  Serial.println("A: Back to the Cell");
  Serial.println("B: Down to the Deck");
  Serial.println("C: Question Purpose");
  Serial.println("D: Stay");
  Serial.println();
  bridgeFlag = false;

  while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }
  
  switch (switchState) { 
      case 1: // left switch
      
        if (leftFlag) { // A: Back to the Cell
          leftFlag = false; 
          Serial.println("Heading back to the cell...");
          Serial.println();
          pageCount = 3;
          delay(50);
          break;

        } else if (rightFlag) { // B: Down to the Deck
          rightFlag = false; 
          Serial.println("Heading down to the deck...");
          Serial.println();
          pageCount = 2;
          delay(50);
          break;

        } else { 
          delay(50);
          switchState = digitalRead(switchPin);
        } 

      case 0: // switch right
        if (leftFlag) { // C: Question Purpose
          leftFlag = false;
          Serial.println("MAX: ...what are we doing here, Sam?");
          Serial.println("SAM: We're here to stop SKUNKAPE, little buddy!");
          Serial.println("MAX: Oh, right!");
          Serial.println();
          bridgeFlag = true;
          pageCount = 1; // also snaps back to the bridge
          delay(50);
          break;

        } if (rightFlag) { // D: Stay
          rightFlag = false; 
          bridgeFlag = true; // should snap back to bridge
          pageCount = 1;
          delay(50);
          break;
        
        } else { // this might be unnecessary
          delay(50);
          switchState = digitalRead(switchPin);

        }
  }

}


void superBallDialogue() {
  // start up dialogue, will be the same every time because i'm lazy
  Serial.println("MAX: Hi, SUPERBALL!");
  Serial.println("SUPERBALL: Hello again, sirs.");
  Serial.println();

  while (superDialogueFlag == true) {

    Serial.println("A: Presidency");
    Serial.println("B: Progress on Parts");
    Serial.println("C: SKUN'KA'PE");
    Serial.println("D: Leave");
    Serial.println();
    
    while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }
    switch (switchState) { 
      case 1: // left switch
      
        if (leftFlag) { // A: Presidency
          leftFlag = false; 
          Serial.println("SAM: How's presidency going, SUPERBALL?");
          Serial.println("SUPERBALL: It's kind of embarrasing, sirs. The US has used so much money making universal healthcare and reducing poverty that we had to reduce our security budget.");
          Serial.println("SUPERBALL: Normally I can secure myself just fine, but I had heard that SKUN'KA'PE was threatening the Seven Wonders of the World so I let myself be taken.");
          Serial.println("MAX: Universal healthcare? What's next, world peace?");
          Serial.println("SUPERBALL: That's scheduled for next Thursday, sir.");
          Serial.println();
          // yknow I like the joke but I don't like the setup. it's fine this is due tomorrow
          delay(50);
          break;

        } else if (rightFlag) { // B: Progress on Parts
          rightFlag = false; 
          if (!magneticBaseFlag && !powerCoreFlag) { // neither of the two parts
            Serial.println("SUPERBALL: You still need the MAGNETIC BASE and the POWER CORE to make the WORMHOLE GENERATOR, sirs.");
            Serial.println("SUPERBALL: According to my sources, STINKY is on the bridge and has the MAGNETIC BASE, and FLINT PAPER is on the deck and has the POWER CORE.");
            Serial.println();

          } else if (!magneticBaseFlag && powerCoreFlag) { // only power core
            Serial.println("SUPERBALL: Good work getting the POWER CORE, sirs. You still need the MAGNETIC BASE to make the WORMHOLE GENERATOR.");
            Serial.println("SUPERBALL: According to my sources, STINKY is on the bridge and has the MAGNETIC BASE.");
            Serial.println();

          } else if (magneticBaseFlag && !powerCoreFlag) { // only magnetic base
            Serial.println("SUPERBALL: Good work getting the MAGNETIC BASE, sirs. You still need the POWER CORE to make the WORMHOLE GENERATOR.");
            Serial.println("SUPERBALL: According to my sources, FLINT PAPER is on the deck and has the POWER CORE.");
            Serial.println();

          } else if (magneticBaseFlag && powerCoreFlag) { // found all the parts!
            // dialogue
            Serial.println("SUPERBALL: Thank you for getting all the parts, sirs. Now we can make the WORMHOLE GENERATOR.");
            Serial.println();
            Serial.println("You've obtained the WORMHOLE GENERATOR!");
            Serial.println();
            // leds
            for (int i = 0; i < 10; i++) {
              CircuitPlayground.setPixelColor(i, 255, 0, 0); // red
              delay(10);
            }
            CircuitPlayground.playTone(aFlat, 200);
            CircuitPlayground.playTone(c, 200);
            CircuitPlayground.playTone(eFlat, 200);
            // A flat, C, E flat
            delay(1000);
            CircuitPlayground.clearPixels();

            Serial.println("SUPERBALL: Please go confront SKUN'KA'PE on the deck when you're ready.");
            Serial.println();
            wormHoleGeneratorFlag = true; 
          }
          delay(50);
          break;

        } 

      case 0: // switch right
        if (leftFlag) { // C: SKUN'KA'PE
          leftFlag = false;
          Serial.println("SAM: I thought SKUN'KA'PE exploded last time we saw him?");
          Serial.println("SUPERBALL: We thought so too, sirs. However, our sources indicate that he survived so that he could torment SAM & MAX in a terrible text-based adventure game.");
          Serial.println("MAX: Oh, the humanity!! This is torture!!");
          Serial.println("SAM: Quiet, bonehead.");
          Serial.println();
          delay(50);
          break;

        } if (rightFlag) { // D: Leave
          rightFlag = false; 
          // dialogue
          Serial.println("MAX: See you around, Supes!");
          Serial.println("SUPERBALL: Good luck, sirs.");
          Serial.println();
          superDialogueFlag = false;
          delay(50);
          break;
        
        } 
    } // end of switch

  }

}

void deckStartUp() {
  // dialogue
  deckCount++; // so this doesn't play another time
  Serial.println("You walk down to the deck. GENERAL SKUN'KA'PE stands at the helm staring through the window at the giant blue marble that is the Earth.");
  Serial.println("His lackeys frantically run around, mostly ignoring you. SKUN'KA'PE himself seems too concerned with logistics than to be worried about you or MAX.");
  Serial.println("FLINT PAPER is in one corner of the deck, riffling through a large case with text you can't read.");
  Serial.println();
  
}

void deckOptions() {
  Serial.println("A: Chat with FLINT PAPER");
  if (!wormHoleGeneratorFlag) {
    Serial.println("B: Talk with SKUN'KA'PE");
  } else if (wormHoleGeneratorFlag == true) {
    Serial.println("B: Confront SKUN'KA'PE");
  }
  Serial.println("C: Look Around");
  Serial.println("D: Move Elsewhere");
  Serial.println();
}

void deckSpaceShip() {
  while (deckFlag == true) {
    
    while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }
    switch (switchState) { 
      case 1: // left switch
      
        if (leftFlag) { // A: Chat with FLINT PAPER
          leftFlag = false; 
          flintStartUp();
          
          delay(50);
          break;

        } else if (rightFlag) { // B: Talk with / Confront SKUN'KA'PE
          rightFlag = false; 
          // doesn't run any options, only dialogue here
          
          if (!wormHoleGeneratorFlag) {
            // regular dialgoue
            Serial.println("SAM: SKUNKAPE! You should be dead!");
            Serial.println("SKUN'KA'PE: And yet I am not, puny Earthling. You seem to underestimate the resilience of my kind.");
            Serial.println("SAM: And you seem to underestimate the power of my fist!");
            Serial.println("SKUN'KA'PE: I invite you to try, dog.");
            Serial.println("MAX: While I'm all for senseless violence, SAM, I think we better get the parts for the worm thingy first!");
            Serial.println();
          } else if (wormHoleGeneratorFlag == true) {
            // time for skunk ape smash
            Serial.println("SAM: It's time to take you down, SKUNKAPE!");
            Serial.println("MAX: Oh goodie! Time to beat up SKUNKAPE!");
            Serial.println();
            deckFlag = false;
            pageCount = 4;
          }
          
          delay(50);
          break;

        } else { 
          delay(50);
          switchState = digitalRead(switchPin);
        } 

      case 0: // switch right
        if (leftFlag) { // C: Look Around
          leftFlag = false;
          Serial.println("Other than the hulking cyber-gorilla form of SKUN'KA'PE at the helm surveying the Earth and FLINT PAPER lounging in a corner, ");
          Serial.println("the unknowable sci-fi knickknacks and occasional plant are too boring to investigate.");
          Serial.println(); 
          delay(50);
          break;

        } if (rightFlag) { // D: Move Elsewhere
          rightFlag = false; 
          leavingDeck();
          
          delay(50);
          break;
        
        } else { // this might be unnecessary
          delay(50);
          switchState = digitalRead(switchPin);

        }
    } // end of switch
  if (deckFlag == true) {
    deckOptions();
  }

  } // end of while loop

}

void leavingDeck() {
  Serial.println("Where would you like to go?");
  Serial.println();
  Serial.println("A: Back to the Cell");
  Serial.println("B: Up to the Bridge");
  Serial.println("C: Question Purpose");
  Serial.println("D: Stay");
  Serial.println();
  deckFlag = false;

  while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }
  
  switch (switchState) { 
      case 1: // left switch
      
        if (leftFlag) { // A: Back to the Cell
          leftFlag = false; 
          Serial.println("Heading back to the cell...");
          Serial.println();
          pageCount = 3;
          delay(50);
          break;

        } else if (rightFlag) { // B: Up to the Bridge
          rightFlag = false; 
          Serial.println("Heading up to the bridge...");
          Serial.println();
          pageCount = 1;
          delay(50);
          break;

        } else { 
          delay(50);
          switchState = digitalRead(switchPin);
        } 

      case 0: // switch right
        if (leftFlag) { // C: Question Purpose
          leftFlag = false;
          Serial.println("MAX: ...what are we doing here, Sam?");
          Serial.println("SAM: We're here to stop SKUNKAPE, little buddy!");
          Serial.println("MAX: Oh, right!");
          Serial.println();
          deckFlag = true;
          pageCount = 1; // should snap back to the deck
          delay(50);
          break;

        } if (rightFlag) { // D: Stay
          rightFlag = false; 
          deckFlag = true; // should also snap back to the deck
          pageCount = 1;
          delay(50);
          break;
        
        } else { // this might be unnecessary
          delay(50);
          switchState = digitalRead(switchPin);

        }
  }
}

void flintStartUp() {

  if (flintCount == 1) { // on first time only
    Serial.println("FLINT PAPER: If it isn't my favorite furry pals!");
    Serial.println("MAX: FLINT!!!");
    Serial.println("SAM: Hey FLINT, what are you doing on SKUNKAPE's shapeship?");
    Serial.println("FLINT PAPER: Well, my PI business has been slow lately, so I've been looking for a lost cat. My sources say it's somewhere on this spaceship.");
    // the lost cat is the thing in the box with stinky. why she has it idk. it probably has something else in there too
    Serial.println("SAM: Sure... well, good luck with that, FLINT.");
    Serial.println("FLINT PAPER: Thanks, SAM! Oh, I have the POWER CORE you need! I need you do something for me first.");
    Serial.println("MAX: Anything for you, FLINT!");
    Serial.println("FLINT PAPER: I need to know you two can beat SKUN'KA'PE into the ground like an overripe orange, so I want to test your fighting skills first.");
    Serial.println("SAM: So, a bit of fisticuffs, like the good old days?");
    Serial.println("FLINT PAPER: You got it, Sam-O! Lemme know when you're ready!");
    Serial.println("MAX: Oh, this will be fun!");
    if (waterGunFlag == false) {
       Serial.println("SAM: It's been a while since I've trained with FLINT, I might need something more than my fists to beat him.");
    }
    Serial.println();
    delay(50);

  } else { // every other time
    Serial.println("MAX: Hiya FLINT!");
    Serial.println("FLINT PAPER: Hey pals!");
    Serial.println();
    delay(50);
  } // you can add more dialogue things that happen, whether random or plays the fifth time or whatever

  flintCount++; // why startup doesn't repeat every time
  flintDialogueFlag = true;
  flintDialogue();
}

void flintDialogue() {
  while (flintDialogueFlag == true) {

    Serial.println("A: SKUN'KA'PE");
    Serial.println("B: STINKY");
    Serial.println("C: Fight FLINT");
    Serial.println("D: Leave");
    Serial.println();
    
    while (!leftFlag && !rightFlag) { // VERY IMPORTANT!! Delay loop
      switchState = digitalRead(switchPin); 
      delay(50);
    }
    switch (switchState) { 
      case 1: // left switch
      
        if (leftFlag) { // A: SKUN'KA'PE
          leftFlag = false; 
          // he's kinda nice, but flint wants to get home because he's looking for a lost cat
          // why a lost cat? the PI business is a little light rn
          Serial.println("SAM: Why would SKUNKAPE have a lost cat on his spaceship?");
          Serial.println("FLINT PAPER: That I don't know, Sam-O, and I don't question either. You guys beating SKUN'KA'PE to a pulp means we can get home sooner, though!");
          Serial.println("MAX: Why don't you beat him up yourself? You're stronger than SAM!");
          Serial.println("SAM: Hey!");
          Serial.println("FLINT PAPER: I'm busy looking for that cat! I promised a sweet old lady I would get her precious Potato back for her, and that's what I'm here to do!");
          Serial.println("MAX: The cat is named Potato?");
          Serial.println("SAM: Me and MAX can beat up SKUNKAPE just fine ourselves!");
          Serial.println();
          delay(50);
          break;

        } else if (rightFlag) { // B: STINKY
          rightFlag = false; 
          // talking about his previous case with stinky and sal
          // thought stinky was dead
          // even after the revelations in 305 he's kinda moved past it and almost forgot about her
          Serial.println("SAM: Do you know what STINKY is doing here, FLINT?");
          Serial.println("FLINT PAPER: Nope! That was an old case. I'm here on my current case.");
          Serial.println("SAM: I thought you would be more interested in her coming back from the dead and all... what if she tries to kill GRANDPA STINKY again?");
          Serial.println("FLINT PAPER: Then I'll get hired again to investigate her! That's how I pay the bills.");
          Serial.println("MAX: You pay bills? I am betrayed, FLINT.");
          Serial.println();
          delay(50);
          break;

        } else { 
          delay(50);
          switchState = digitalRead(switchPin);
        } 

      case 0: // switch right
        if (leftFlag) { // C: Fight Flint
          leftFlag = false;
          fightFlag = true;
          fightStartUp();
          
          delay(50);
          break;

        } if (rightFlag) { // D: Leave
          rightFlag = false; 
          // dialogue
          Serial.println("MAX: See you later, FLINT!");
          Serial.println();
          flintDialogueFlag = false;
          delay(50);
          break;
        
        } else { // this might be unnecessary
          delay(50);
          switchState = digitalRead(switchPin);

        }
    } // end of switch

  }

}

void fightStartUp() {

  fightFlag = true;
  samHitPoints = 18; // for if you run the fight again, then it should reset your HP
  enemyHitPoints = 20;
  loseFlag = false; // so you can restart the fight

  Serial.println("The FIGHT has begun! SAM is up first!");

  fightDialogue();
}

void fightDialogue() {
  while(fightFlag == true) {

    Serial.print("SAM's HP: ");
    Serial.println(samHitPoints);
    Serial.print("FLINT PAPER's HP: ");
    Serial.println(enemyHitPoints);
    Serial.println();

    Serial.println("A: Attack");
    Serial.println("B: Defend");
    Serial.println("C: One Liner");
    Serial.println("D: Max"); // max will butt in, but sam still gets his turn
    // which means the other options need to break a flag
    Serial.println();

    while (!leftFlag && !rightFlag) { // good old delay loop
      switchState = digitalRead(switchPin); 
      delay(5); // might not double click when this is shorter
    }
    switch (switchState) {  // Sam's Turn
      case 1: // left switch
        if (leftFlag) { // A: Attack
          leftFlag = false;

          if (waterGunFlag == true) {
            randomSamDamage = random(1,11); // from 1 to 10 damage, makes the fight pretty easy
            enemyHitPoints = max(0, (enemyHitPoints - randomSamDamage)); // floor is now always zero
            Serial.println();
            Serial.print("SAM attacks with the WATER GUN for ");
            Serial.print(randomSamDamage);
            Serial.println(" damage!");
            Serial.println();

            Serial.print("FLINT PAPER's HP: ");
            Serial.println(enemyHitPoints);
            Serial.println();

          } else if (waterGunFlag == false) {
            randomSamDamage = random(1,4); // deals from 1 to 3 damage (harder to beat enemy this way)
            enemyHitPoints = max(0, (enemyHitPoints - randomSamDamage)); // floor is now always zero
            Serial.print("SAM attacks for ");
            Serial.print(randomSamDamage);
            Serial.println(" damage!");
            Serial.println();

            Serial.print("FLINT PAPER's HP: ");
            Serial.println(enemyHitPoints);
            Serial.println();
          }

          enemyTurnFlag = true;
          delay(50);
          break;

        } else if (rightFlag) { // B: Defend
          rightFlag = false;

          Serial.println("SAM defends against the next attack!");
          Serial.println();

          defendFlag = true;
          enemyTurnFlag = true;
          delay(50);
          break;
        }
      case 0: // right switch
        if (leftFlag) { // C: One Liner
          leftFlag = false;

          Serial.println("SAM says a witty one-liner!");
          Serial.println("SAM: Did you know that half of the bones in your body are in your hands and feet?");
          Serial.println("MAX: ... That wasn't very witty, SAM.");
          Serial.println("FLINT PAPER looks confused. He will deal less damage next attack!");
          Serial.println();

          oneLinerFlag = true;
          enemyTurnFlag =  true;
          delay(50);
          break;

        } if (rightFlag) { // D: Max (says something random)
          rightFlag = false;
          Serial.println("MAX: GET HIM SAM!!!! BEAT HIM UP!!!");
          Serial.println("SAM: Well you're eager, little buddy!");
          Serial.println();

          randomMaxDamageChance = random(0,10);
          if (randomMaxDamageChance == 0) { // one in 100 chance
            maxDamage = 1000000;
            enemyHitPoints = max(0, (enemyHitPoints - maxDamage));
            Serial.println("MAX attacks for 1000000 damage!");

            Serial.print("FLINT PAPER's HP: ");
            Serial.println(enemyHitPoints);
            Serial.println();
          } // skips if this doesn't play

          // doesn't turn enemyTurnFlag on, so it should loop back to Sam's turn
          delay(50);
          break;

        }
    }

    delay(500); // waits a bit

    if (enemyHitPoints == 0) { // "you win" screen
      enemyTurnFlag = false; // this works out anyway because I needed to stop the turn somehow
      fightFlag = false; // break from the fight
      winFlag = true;
      samHitPoints = 25; // just in case, so it doesn't run the loss screen
      Serial.println("~~~ YOU WIN!!! ~~~");
      Serial.println();
      Serial.println("FLINT PAPER: You got me! I think you guys are ready to take on SKUN'KA'PE!");
      Serial.println();

      if (powerCoreFlag == false) {
        powerCoreFlag = true;
        for (int i = 0; i < 10; i++) {
          CircuitPlayground.setPixelColor(i, 200, 50, 0); // hopefully some kind of yellow
        }
        CircuitPlayground.playTone(aFlat, 200);
        CircuitPlayground.playTone(c, 200);
        CircuitPlayground.playTone(eFlat, 200);
        // A flat, C, E flat
        delay(1000);
        CircuitPlayground.clearPixels();

        Serial.println("You've found the POWER CORE!");
        Serial.println();
      }
      if (powerCoreFlag, spaceWarperFlag, magneticBaseFlag == true) {
        // insert leds here
        for (int i = 0; i < 10; i++) {
          CircuitPlayground.setPixelColor(i, 255, 255, 255); // white
          delay(10);
        }
        Serial.println("You've collected all the parts for the WORMHOLE GENERATOR! Go back and talk to SUPERBALL on the bridge!");
        Serial.println();
        delay(1000);
        CircuitPlayground.clearPixels();
      }
    }

    if (enemyTurnFlag == true) { // runs the enemy's turn
      enemyTurnFlag = false;

      if (defendFlag == true) {
        // blocks damage
        randomEnemyDamage = random(1,6); // between 1 and 5
        Serial.print("FLINT PAPER attacks for ");
        Serial.print(randomEnemyDamage);
        Serial.println(" damage!");
        Serial.println("SAM blocks the attack and receives no damage!");
        Serial.println();
        // keeps sam's HP at the same value
        defendFlag = false;

      } else if (defendFlag == false) { 
        // attacking. defending is too complicated to code
        if (oneLinerFlag == true) { // deals less damage
          randomEnemyDamage = random(1,4); // between 1 and 3
          samHitPoints = max(0, (samHitPoints - randomEnemyDamage)); // should subtract from current HP, min is 0
          // this line was suggested by Chat GPT to adjust for dealing damage greater than the current HP, so it doesn't go to zero.
          // the process of subtracting the damage from the HP is the same, it just using a max function to cap the minimum to zero.

          Serial.print("FLINT PAPER attacks for ");
          Serial.print(randomEnemyDamage);
          Serial.println(" damage!");
          Serial.println();

          Serial.print("SAM's HP: ");
          Serial.println(samHitPoints);
          Serial.println();

        } else if (oneLinerFlag == false) { // deals normal damage
          randomEnemyDamage = random(1,6); // between 1 and 5
          samHitPoints = max(0, (samHitPoints - randomEnemyDamage)); // should subtract from current HP, min is 0
          Serial.print("FLINT PAPER attacks for ");
          Serial.print(randomEnemyDamage);
          Serial.println(" damage!");
          Serial.println();

          Serial.print("SAM's HP: ");
          Serial.println(samHitPoints);
          Serial.println();
        }
      }
    }

    if (samHitPoints == 0) { // "you lose" screen
      enemyTurnFlag = false;
      loseFlag = true;
      fightFlag = false; // breaks from fight
      
      Serial.println("~~~ YOU LOSE ~~~");
      Serial.println();
      Serial.println("FLINT PAPER: Oh, you might need a little more work to be able to beat up SKUN'KA'PE!");
      if (waterGunFlag == false) {
        Serial.println("SAM: A weapon of some kind might work better than my fists here...");
      }
      Serial.println();
      // insert hint dialogue here, only first time probably

    }
    defendFlag = false; 
    enemyTurnFlag = false; // at the end of each loop, should restart the turn
    // Serial.println("Resetting turns..."); // for debugging's sake
    delay(500);
  }
}


void skunkApeSmash() {
  // dialogue time baby
  Serial.println("SKUN'KA'PE: You puny Earthlings can do nothing against me, the great GENERAL SKUN'KA'PE-- hey, what's that--");
  Serial.println();
  Serial.println("A: Throw the WORMHOLE GENERATOR at SKUN'KA'PE!");
  Serial.println();

  while (smashFlag == true) {

    while (!leftFlag && !rightFlag) { 
      switchState = digitalRead(switchPin); 
      delay(50);
    }


    switch(switchState) {
      case 1: // switch is left
        if (leftFlag) {
          leftFlag = false;
          Serial.println("SAM: Take this, SKUNKAPE!");
          Serial.println("You throw the WORMHOLE GENERATOR as hard as you can at SKUN'KA'PE. It activates, and wormhole opens up behind him."); 
          Serial.println("He gets pulled into the wormhole, but grabs onto the helm with one hand.");
          Serial.println("SKUN'KA'PE: You won't get away with this, SAM & MAX!");
          Serial.println("MAX walks up to the helm and giggles. He peels SKUN'KA'PE's fingers away from the helm one at a time.");
          Serial.println("SKUN'KA'PE: No!!");
          Serial.println("MAX: Hope you have fun in deep space!");
          Serial.println("As MAX pries SKUN'KA'PE's last finger from the helm, he falls into the wormhole, and it closes behind him.");
          Serial.println();
          Serial.println("~~~ THE END ~~~");
          Serial.println();

          Serial.println("Congratulations on beating SAM & MAX: DESTRUCTION FOR ALL AGES!");
          Serial.println("I hope you enjoyed, and thanks for playing my game!");

          delay(50);
          // never breaks
        }
    }
  }
}


