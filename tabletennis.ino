// includes the LiquidCrystal library for the LCD
#include <LiquidCrystal.h>

// Setting up constants for the pin numbers.
// This isn't needed, but it should make it easier to follow below.

// Sets pins for 4 buttons.
const int buttonOne = 10;
const int buttonTwo = 9;
const int buttonThree = 7;
const int buttonFour = 8;

// Sets RGB LED pins.
// RGB LED requires 3 pins, single color LED's only require one.
const int bluePin = 11;
const int greenPin = 12;
const int redPin = 13;

// Sets white LED pin.
const int whitePin = 0;

//Setting up LCD pins
LiquidCrystal lcd(6, 5, 4, 3, 2, 1);

// Creating and setting variables.
// ASP & BSP = set points for player A and B
// AGP & BGP = game points for player A and B
// The serves variable is to keep track of how many serves
// per side. We have 2 serves per side until 10 points is reached,
// then we switch to 1 serve per side.
// tempA and tempB keep temporary scores in case of decrementing
int ASP = 0;
int BSP = 0;
int AGP = 0;
int BGP = 0;
int tempA;
int tempB;
String tempLast;
int serves = 0;
// Instead of variables swapping sides, they just swap names.
String p1 = "P1";
String p2 = "P2";

void setup() {
  // The LiquidCrystal library can be used with many different
  // LCD sizes. We're using one that's 2 lines of 16 characters,
  // so we'll inform the library of that:
  lcd.begin(16, 2);

  // Setting the initial scores of zero.
  lcd.setCursor(0, 0);
  lcd.print("P1  0");
  lcd.setCursor(7, 0);
  lcd.print(": P2  0");
  lcd.setCursor(0, 1);
  lcd.print("SET 0  : SET 0");

  // Set up the pushbutton pins to be an input:
  pinMode(buttonTwo, INPUT);
  pinMode(buttonThree, INPUT);
  pinMode(buttonFour, INPUT);
  pinMode(buttonOne, INPUT);

  // Set up the RGB and White LED pins to be outputs:
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(whitePin, OUTPUT);
}

void loop() {
  // Local variable to hold the pushbutton states
  int buttonTwoState;
  int buttonThreeState;
  int buttonFourState;
  int buttonOneState;

  // Reads the digital state of buttonPin with digitalRead() function and stores the
  // Value in the corresponding buttonState variable
  buttonTwoState = digitalRead(buttonTwo);
  buttonThreeState = digitalRead(buttonThree);
  buttonFourState = digitalRead(buttonFour);
  buttonOneState = digitalRead(buttonOne);

  // Uses a pull-up resistor to keep the pin in HIGH state
  // When the button is pressed, the power is grounded and moved to LOW state
  // 4 if statements check for LOW state and then run their respective code,
  // incrementing or decrementing the score.
  // Delay prevents bounce for 300 miliseconds.
  if (buttonOneState == LOW) {
    serves++;
    ASP++;
    gamePointIncrementer();
    serveSwap();
    refreshLCD();
    delay(300);
  }
  if (buttonTwoState == LOW) {
    decrementChecker();
    ASP--;
    refreshLCD();
    delay(300);
  }
  if (buttonThreeState == LOW) {
    serves++;
    BSP++;
    gamePointIncrementer();
    serveSwap();
    refreshLCD();
    delay(300);
  }
  if (buttonFourState == LOW) {
    decrementChecker();
    BSP--;
    refreshLCD();
    delay(300);
  }
}

// Checks to see if serves or scores should be reversed after decrementing;
void decrementChecker() {
  if (ASP == 0 && BSP == 0) {
    ASP = tempA;
    BSP = tempB;
    if (ASP > BSP) {
      AGP--;
    } else {
      BGP--;
    }
  }
}

// Checks to see if one or both players have a score over 10 points
// and that one score is at least 2 points ahead of the other.
// If true, then increment A or B game points and reset serve points and serves to 0.
void gamePointIncrementer() {
  if ((ASP - BSP > 1 || BSP - ASP > 1) && (ASP > 10 || BSP > 10)) {
    if (ASP > BSP) {
      AGP++;
      tempLast = p1;
    } else {
      BGP++;
      tempLast = p2;
    }
    tempA = ASP;
    tempB = BSP;
    ASP = 0;
    BSP = 0;
    serves = 0;
  }
}

// Checks to see who is serving.
// Number of serves per side is reduced from 2 to 1 if either player has 10 points.
// A light on either side of the board indicates which side is serving.
void serveSwap() {
  int serveLimit;
  if (ASP > 10 || BSP > 10) {
    serveLimit = 1;
  } else {
    serveLimit = 2;
  }
  
  // If the serve limit is reached, the player display and scores swap sides
  int tempPoints;
  if (serves == serveLimit) {
    tempPoints = AGP;
    AGP = BGP;
    BGP = tempPoints;
    String tempNames = p1;
    p1 = p2;
    p2 = tempNames;
    tempPoints = ASP;
    ASP = BSP;
    BSP = tempPoints;
    serves = 0;

    // Turns on a light to display which side is serving
    if (p1 == "P2") {
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, HIGH);
      digitalWrite(whitePin, LOW);
    } else {
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);
      digitalWrite(whitePin, HIGH);
    }
  }
}

// Updates the LCD to display the correct score.
// Also calls winnerWinner to check for a winner.
void refreshLCD() {

  // Clears the screen, sets the cursor and prints text or a score 6 times
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(p1 + "  ");
  lcd.setCursor(4, 0);
  lcd.print(String(AGP)); // int variables need to be converted to String
  lcd.setCursor(7, 0);
  lcd.print(": " + p2 + "  ");
  lcd.setCursor(13, 0);
  lcd.print(String(BGP));
  lcd.setCursor(0, 1);
  lcd.print("SET " + String(ASP));
  lcd.setCursor(7, 1);
  lcd.print(": SET " + String(BSP));
  winnerWinner();
}

// Checks for a winner.
// If true, displays celebratory strobe and disco lights.
void winnerWinner() {

  // Best of 5 would mean first player to 3 game points.
  if (AGP > 2 || BGP > 2) {
    String winner;
    if (AGP > BGP) {
      if (p1 == "P1"){
        winner = "Player 1";
      } else {
        winner = "Player 2";
      }
    } else {
      if (p1 == "P1") {
        winner = "Player 2";
      } else {
        winner = "Player 1";
    }
    }

    // Displays the winner for 2 seconds.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(winner);
    lcd.setCursor(0, 1);
    lcd.print("Wins the game!");
    delay(5000);

    // Then celebration for an absurdly long time!
    for (int win = 0; win < 100; win++) {
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);
      digitalWrite(whitePin, LOW);
      lcd.clear();
      delay(300);
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, LOW);
      digitalWrite(whitePin, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("WINNER WINNER");
      lcd.setCursor(0, 1);
      lcd.print("CHICKEN DINNER");
      delay(300);
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, HIGH);
      digitalWrite(whitePin, LOW);
      lcd.clear();
      delay(300);
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, HIGH);
      digitalWrite(whitePin, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("WINNER WINNER");
      lcd.setCursor(0, 1);
      lcd.print("CHICKEN DINNER");
      delay(300);
    }
    AGP = 0;
    BGP = 0;
    ASP = 0;
    BSP = 0;
  }
}