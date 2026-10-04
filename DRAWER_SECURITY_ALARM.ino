/*
  DRAWER SECURITY ALARM - FINAL VERSION (Class 7)
  ------------------------------------------------
  How it works:
  1. Turn the switch ON to arm the alarm.
  2. Wait 5 seconds (countdown) so the drawer can be closed.
  3. A laser beam shines on the LDR module.
  4. If the beam is broken OR the IR sensor sees something,
     the alarm rings (buzzer + red LED).
  5. The alarm keeps ringing until the switch is turned OFF.

  Components: Arduino Uno, laser diode, LDR module, IR sensor,
              ON/OFF switch, buzzer, built-in LED
*/

// -------- PIN NAMES --------
const int LDR_PIN    = 2;    // LDR module DO pin
const int IR_PIN     = 3;    // IR sensor OUT pin
const int SWITCH_PIN = 4;    // ON/OFF switch
const int BUZZER_PIN = 8;    // Buzzer + pin
const int LED_PIN    = 13;   // Built-in red LED

// -------- SETTINGS (values you told me) --------
// LDR gives 0 (LOW) when laser hits it, so beam BROKEN = 1 (HIGH)
const int BEAM_BROKEN = HIGH;

// IR gives 0 (LOW) when it senses something
const int IR_DETECTED = LOW;

// -------- VARIABLES (boxes that remember things) --------
bool alarmRinging = false;   // false = quiet, true = alarm is ringing
bool systemArmed  = false;   // false = disarmed, true = armed

// ---------------------------------------------------
// setup() runs ONCE when the Arduino starts
// ---------------------------------------------------
void setup() {
  Serial.begin(9600);    // Start the Serial Monitor
  Serial.println("=== DRAWER SECURITY ALARM STARTED ===");
  Serial.println("Turn the switch ON to arm the alarm.");

  pinMode(LDR_PIN, INPUT);            // LDR gives us information (input)
  pinMode(IR_PIN, INPUT);             // IR gives us information (input)
  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Switch input (ON = LOW)
  pinMode(BUZZER_PIN, OUTPUT);        // Buzzer is an output
  pinMode(LED_PIN, OUTPUT);           // LED is an output
}

// ---------------------------------------------------
// loop() runs again and again forever
// ---------------------------------------------------
void loop() {

  // ----- STEP 1: Read the switch -----
  // With INPUT_PULLUP: switch ON = LOW, switch OFF = HIGH
  bool switchIsOn = (digitalRead(SWITCH_PIN) == LOW);

  // ----- STEP 2: Switch is OFF -> disarm everything -----
  if (switchIsOn == false) {
    noTone(BUZZER_PIN);            // Buzzer silent
    digitalWrite(LED_PIN, LOW);    // LED off
    alarmRinging = false;          // Reset the alarm

    if (systemArmed == true) {     // Print only when something changes
      Serial.println("System DISARMED (switch OFF). Alarm stopped.");
      systemArmed = false;
    }

    delay(100);
    return;                        // Go back to the start of loop
  }

  // ----- STEP 3: Switch just turned ON -> countdown, then arm -----
  if (systemArmed == false) {
    Serial.println("Switch ON. Arming in 5 seconds...");

    for (int i = 5; i > 0; i--) {
      Serial.print("Arming in ");
      Serial.println(i);
      digitalWrite(LED_PIN, HIGH);   // Blink LED during countdown
      delay(250);
      digitalWrite(LED_PIN, LOW);
      delay(750);

      // If the switch is turned OFF during countdown, cancel
      if (digitalRead(SWITCH_PIN) == HIGH) {
        Serial.println("Countdown cancelled.");
        return;
      }
    }

    systemArmed = true;
    Serial.println("System ARMED. Watching the drawer...");
  }

  // ----- STEP 4: Read both sensors -----
  bool laserBroken = (digitalRead(LDR_PIN) == BEAM_BROKEN);
  bool irDetected  = (digitalRead(IR_PIN) == IR_DETECTED);

  // ----- STEP 5: Start the alarm if a sensor triggers -----
  // Once alarmRinging becomes true, it stays true until switch OFF.
  if (alarmRinging == false) {

    if (laserBroken == true) {
      Serial.println("ALERT! Laser beam is BROKEN!");
      alarmRinging = true;
    }

    if (irDetected == true) {
      Serial.println("ALERT! IR sensor detected something!");
      alarmRinging = true;
    }
  }

  // ----- STEP 6: If the alarm is ringing, make noise and light -----
  if (alarmRinging == true) {
    // Siren effect: high tone, then low tone
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 2000);        // High tone
    delay(200);
    digitalWrite(LED_PIN, LOW);
    tone(BUZZER_PIN, 1000);        // Low tone
    delay(200);
  }
}