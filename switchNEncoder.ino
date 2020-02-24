/*  Defines for encoder  and button(sw)*/

#define CLK A0
#define DATA A1
#define sw A2
static uint8_t prevNextCode = 0;
static uint16_t store = 0;
static int8_t rot_enc_table[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};

int lastButtonState = HIGH;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
bool buttonState = HIGH; //saving state of the switch
volatile byte tapCounter; //for saving no. of times the switch is pressed
int timediff; //for saving the time in between each press and release of the switch
bool flag1, flag2; //just two variables
long double presstime, releasetime; //for saving millis at press and millis at release



/*  Defines for Seven segment display */

#define digit1Sel 8
#define digit2Sel 9
#define digit3Sel 10
#define digit4Sel 11

//volatile uint8_t digit[4];
volatile uint8_t segmentsfa[] = {0b11111100, 0b00011000, 0b01101100, 0b00111100, 0b10011000, 0b10110100, 0b11110100, 0b00011100, 0b11111100, 0b10111100};
volatile uint8_t segmentsg[] = {0b00000000, 0b00000000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00000000, 0b00100000, 0b00100000};
volatile int k, j, i, h, val = 0 ;
volatile int temp, num;

void setup()  {
  pinMode(CLK, INPUT);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DATA, INPUT);
  pinMode(DATA, INPUT_PULLUP);
  pinMode(sw, INPUT_PULLUP);
  pinMode(sw, INPUT_PULLUP); //setting pin A2 as input with internal pull up resistor
  pinMode(digit1Sel, OUTPUT); digitalWrite(digit1Sel, LOW);
  pinMode(digit2Sel, OUTPUT); digitalWrite(digit2Sel, LOW);
  pinMode(digit3Sel, OUTPUT); digitalWrite(digit3Sel, LOW);
  pinMode(digit4Sel, OUTPUT); digitalWrite(digit4Sel, LOW);


  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  TIMSK1 = (1 << OCIE1A); /* Enable Timer1 overflow interrupts */
  TCNT1 = 0;  /* load TCNT0, count for 10ms*/
  TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
  TCCR1B |= ((1 << CS12) | (1 << WGM12)); /* start timer1 with /256 prescaler in CTC Mode*/

  TCCR1B &= ~((1 << CS11) | (1 << CS10) | (1 << WGM13));
  OCR1A = 150;  // for refresh of > 80Hz per digit
  noDigit();
  Serial.begin(9600);
  sei();
}
void loop () {
  adjustValue();
  switchRead();
}

void switchRead () {
  int reading = digitalRead(sw);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
    }
  }

  //when switch is pressed
  if (buttonState == 0 && flag2 == 0)
  {
    presstime = millis(); //time from millis fn will save to presstime variable
    flag1 = 0;
    flag2 = 1;
    tapCounter++; //tap counter will increase by 1

  }
  //when sw is released
  if (buttonState == 1 && flag1 == 0)
  {
    releasetime = millis(); //time from millis fn will be saved to releasetime var
    flag1 = 1;
    flag2 = 0;

    timediff = releasetime - presstime; //here we find the time gap between press and release and stored to timediff var

  }

  if ((millis() - presstime) > 400 && buttonState == 1) //wait for some time and if sw is in release position
  {
    if (tapCounter == 1) //if tap counter is 1
    {
      if (timediff >= 400) //if time diff is larger than 400 then its a hold
      {
        Serial.println("Hold");
        hold(); //fn to call when the button is hold
        Serial.println(k);
        Serial.println(j);
        Serial.println(i);
        Serial.println(h);

      }
      else //if timediff is less than 400 then its a single tap
      {
        Serial.println("single tap");
        singleTap(); //fn to call when the button is single taped
        Serial.println(k);

      }
    }
    else if (tapCounter == 2 ) //if tapcounter is 2
    {
      if (timediff >= 400) // if timediff is greater than  400 then its single tap and hold
      {
        Serial.println("single tap and hold");
        tapAndHold(); //fn to call when the button is single tap and hold
        Serial.println(num);
      }
      else // if timediff is less than 400 then its just double tap
      {
        Serial.println("double tap");
        doubleTap(); //fn to call when doubletap
        Serial.println(j);
      }
    }
    else if (tapCounter == 3) //if tapcounter is 3 //then its triple tap
    {
      Serial.println("triple tap");
      tripleTap(); //fn to call when triple tap
      Serial.println (i);
    }
    else if (tapCounter == 4) //if tapcounter is 4 then its 4 tap
    {
      Serial.println("four tap");
      fourTap();//fn to call when four tap
      Serial.println(h);
    }
    tapCounter = 0;
  }
  lastButtonState = reading;
}


void noDigit()
{
  PORTB &= ~((1 << digit4Sel) | (1 << digit3Sel) | (1 << digit2Sel) | (1 << digit1Sel));
}

void singleTap()
{
  noDigit();
  adjustValue();
  segmentsEnable();
  digitalWrite(digit1Sel, HIGH);
  k = temp;
  temp = 0;
}

void doubleTap()
{
  noDigit();
  adjustValue();
  segmentsEnable();
  digitalWrite(digit2Sel, HIGH);
  j = temp;
  temp = 0;
}
void tripleTap()
{
  noDigit();
  segmentsEnable();
  digitalWrite(digit3Sel, HIGH);
  adjustValue();
  i = temp;
  temp = 0;
}
void fourTap()
{
  noDigit();
  segmentsEnable();
  digitalWrite(digit4Sel, HIGH);
  adjustValue();
  h = temp;
  temp = 0;
}
void hold()
{
  noDigit();
}

void tapAndHold()
{
  noDigit();
  segmentsEnable();
  num = k * 1000 + (j * 100) + (i * 10) + h;
  digitalWrite(digit1Sel, HIGH);
  digitalWrite(digit2Sel, HIGH);
  digitalWrite(digit3Sel, HIGH);
  digitalWrite(digit4Sel, HIGH);

}

// A valid CW or CCW move returns 1, invalid returns 0

int read_rotary()  {
  prevNextCode <<= 2;
  if (digitalRead(DATA)) prevNextCode |= 0x02; //digitalRead the DATA pin
  if (digitalRead(CLK)) prevNextCode |= 0x01; //digitalRead the CLK pin
  prevNextCode &= 0x0f;

  //if valid store as 16 bit data
  if (rot_enc_table[prevNextCode])  {
    store <<= 4;
    store |= prevNextCode;
    if ((store & 0xff) == 0x2b) return -1;
    if ((store & 0xff) == 0x17) return 1;
  }
  return 0;
}

int adjustValue () {
  if (val = read_rotary())  {
    temp += val;
    if (temp >= 10) {
      temp  = 0;
    }
    if (temp <= -1) {
      temp = 9;
    }
  }
}

void segmentsEnable()  {
  PORTD = segmentsfa[temp];
  PORTB = segmentsg[temp];
}


ISR(TIMER1_COMPA_vect)
{
  static byte digit = 0;
  switch (digit)
  {
    case 0:
      PORTB = segmentsg[k];
      PORTD = segmentsfa[k];
      PORTB |= (1 << PB0);
      break;
    case 1:
      PORTB = segmentsg[j];
      PORTD = segmentsfa[j];
      PORTB |= (1 << PB1);
      break;
    case 2:
      PORTB = segmentsg[i];
      PORTD = segmentsfa[i];
      PORTB |= (1 << PB2);
      break;
    case 3:
      PORTB = segmentsg[h];
      PORTD = segmentsfa[h];
      PORTB |= (1 << PB3);
      break;
  }
  digit++;
  if (digit > 3)
    digit = 0;
}
