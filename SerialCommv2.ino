/////////////////////////////////////////////////////////////////////////
//  3D-Photobooth Arduino Code
//  Author: David Riedell
//          dariedel@ncsu.edu
//
//  This code governs the behavior of the Arduino Uno for the 3D-Photobooth.
/////////////////////////////////////////////////////////////////////////


// Variable Declarations

const int ls1 = 2;        // Limit Switch 1
const int ls2 = 3;        // Limit Switch 2
const int lc = 4;         // Load Cell
const int vstep = 5;      // Vertical Motor Step
const int rstep = 6;      // Rotational Motor Step
const int vdir = 7;       // Vertical Motor Direction
const int rdir = 8;       // Rotational Motor Direction
const int vfan = 9;       // Vertical Motor Fan Enable
const int rfan = 10;      // Rotational Motor Fan Enable

String serialString = "";  // incoming serial string
int ls1state = 0;
int ls2state = 0;
long vtt = 0;             // Vertical travel time
long dist = 0;            // Avoid tripping limit switches
int vOnDelay = 1;         // msecs that vert motor is set high
int vOffDelay = 5;        // msecs that vert motor is set low
int rOnDelay = 1;         // msecs that rotational motor is set high
int rOffDelay = 5;        // msecs that rotational motor is set low

/////////////////////////////////////////////////////////////////////////
//  setup
/////////////////////////////////////////////////////////////////////////
void setup() {
    pinMode(ls1, INPUT);       // LOW is not closed, HIGH is closed, top switch
    pinMode(ls2, INPUT);       // LOW is not closed, HIGH is closed, bottom switch
    pinMode(lc, INPUT);        // LOW is not tripped, HIGH is tripped
    pinMode(vstep, OUTPUT);    // LOW is no pulses, HIGH is a pulse
    pinMode(rstep, OUTPUT);    // LOW is no pulses, HIGH is a pulse
    pinMode(vdir, OUTPUT);     // LOW is move down, HIGH is move up
    pinMode(rdir, OUTPUT);     // LOW is counterclockwise, HIGH is clockwise
    pinMode(vfan, OUTPUT);     // LOW is off, HIGH is on
    pinMode(rfan, OUTPUT);     // LOW is off, HIGH is on
    
    Serial.begin(9600);        // start serial port at 9600 bps:
    findLimits();
}

/////////////////////////////////////////////////////////////////////////
//  loop
/////////////////////////////////////////////////////////////////////////
void loop() {
    Serial.println("\nEntered loop function");
    
    while(true) {
        checkSerial();
        delay(1000);
        if(serialString == "start scan\n") {
            Serial.println("serialString == " + serialString);
            break;
        } else if (serialString == "find limits\n") {
            findLimits();
        }
    }
    Serial.println("Starting Scan!");
    delay(3000);
    while(true) {
        dist = 0.96*vtt;
        while(dist > 0) {
            Serial.println(dist);
            dist--;
            vup();
            rotate();
            
            checkSerial();
            if(serialString == "stop\n") {
                digitalWrite(vstep, LOW);
                digitalWrite(rstep, LOW);
                loop();
            }
        }
        dist = 0.96*vtt;
        while(dist>0) {
            Serial.println(dist);
            dist--;
            vdown();
            rotate();
            
            checkSerial();
            if(serialString == "stop\n") {
                digitalWrite(vstep, LOW);
                digitalWrite(rstep, LOW);
                loop();
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////
//  establishContact
/////////////////////////////////////////////////////////////////////////
void establishContact() {
    while (Serial.available() <= 0) {
        Serial.write("A\n");   // send a capital A
        delay(1000);
    }
}

/////////////////////////////////////////////////////////////////////////
//  checkSerial
/////////////////////////////////////////////////////////////////////////
int checkSerial() {
//    Serial.println("Entered checkSerial function");
    serialString = ""; // Clear recieved buffer
    while (Serial.available() > 0) {
        char recieved = Serial.read();
        serialString += recieved;
        
        // Process message when new line character is recieved
        if (recieved == '\n') {
            Serial.println("serialString: " + serialString);
        }
    }
    
//    Serial.println("Exiting checkSerial function");
    return 0;
}

/////////////////////////////////////////////////////////////////////////
//  findLimits
/////////////////////////////////////////////////////////////////////////
void findLimits() {
    Serial.println("Entered findLimits function");
    
    Serial.println("Movin' on up!");
    int limitSwitch1 = 0;
    while(!limitSwitch1) {
        limitSwitch1 = vup();
//        Serial.println(limitSwitch1);
    }
    
    Serial.println("Movin' on down!");
    int limitSwitch2 = 0;
    while(!limitSwitch2) {
        limitSwitch2 = vdown();
        vtt++;
//        Serial.println(limitSwitch2);
    }
    
    digitalWrite(vdir, HIGH);
    
    String vttString = String(vtt);
    Serial.println("\nvtt = " + vttString);
    
    int buffer = 0.02*vtt;
    String bufferString = String(buffer);
    Serial.println("buffer = " + bufferString);
    
    dist = 0.96*vtt;
    String distString = String(dist);
    Serial.println("dist = " + distString);
    
    while(buffer > 0) {      // Maybe can change this to vup()?
        buffer--;
        digitalWrite(vstep, HIGH);
        delay(vOnDelay);
        digitalWrite(vstep, LOW);
        delay(vOffDelay);
    }
    digitalWrite(vstep,LOW);
    Serial.println("At the starting position!\n");
//    Serial.println("Exiting findLimits function");
}

/////////////////////////////////////////////////////////////////////////
//  Vertical Motor Up
/////////////////////////////////////////////////////////////////////////
int vup() {
//    Serial.println("Entered vup function");
    
    digitalWrite(vdir, HIGH);            // Set vertical direction up
    
    int ls1state = digitalRead(ls1);
    if(ls1state == HIGH) {
        digitalWrite(vstep, LOW);
        Serial.println("Hit top LS!!!");
        return 1;
    }
    
    digitalWrite(vstep, HIGH);
    delay(vOnDelay);
    digitalWrite(vstep, LOW);
    delay(vOffDelay);
    
//    Serial.println("Exiting vup function");
    return 0;
}

/////////////////////////////////////////////////////////////////////////
//  Vertical Motor Down
/////////////////////////////////////////////////////////////////////////
int vdown() {
//    Serial.println("Entered vdown function");
    
    digitalWrite(vdir, LOW);            // Set vertical direction down
    int ls2state = digitalRead(ls2);
    
    if(ls2state == HIGH) {
        digitalWrite(vstep, LOW);
        Serial.println("Hit bottom LS!!!");
        return 1;
    }
    
    digitalWrite(vstep, HIGH);
    delay(vOnDelay);
    digitalWrite(vstep, LOW);
    delay(vOffDelay);
    
//    Serial.println("Exiting vdown function");
    return 0;
}

/////////////////////////////////////////////////////////////////////////
//  Rotational Motor
/////////////////////////////////////////////////////////////////////////
int rotate() {
//    Serial.println("Entered rotate function");
    
    digitalWrite(rdir, HIGH);
    
    digitalWrite(rstep, HIGH);
    delay(rOnDelay);
    digitalWrite(rstep, LOW);
    delay(rOffDelay);
    
//    Serial.println("Exiting rotate function");
    return 0;
}
