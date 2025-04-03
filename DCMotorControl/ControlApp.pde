import controlP5.*;
import processing.serial.*;

// Declare ControlP5 and Serial objects
ControlP5 cp5;
Serial myPort;

// Variables to hold input and received data
String inputText = "";
boolean enterPressed = false;
String motorSpeed = "0";
String motorPosition = "0";
String motorRotations = "0";

void setup() {
  size(600, 400);
  
  // Initialize serial communication with Arduino (adjust port index if needed)
  String portName = Serial.list()[0];  
  myPort = new Serial(this, portName, 9600);

  // Initialize ControlP5 for GUI elements
  cp5 = new ControlP5(this);

  // Input Textfield for rotation reference
  cp5.addTextfield("rotationRef")
     .setPosition(50, 140)
     .setSize(200, 40)
     .setFocus(true)
     .setFont(createFont("Arial", 16))
     .setColor(color(0, 0, 0))
     .setColorBackground(color(200, 200, 200))
     .setColorActive(color(255, 255, 255))
     .setLabel("");

  // Button to send rotation reference to Arduino
  cp5.addButton("Send Rotation Ref")
     .setPosition(300, 140)
     .setSize(120, 40)
     .setFont(createFont("Arial", 16))
     .setColorBackground(color(0, 150, 136))
     .setColorForeground(color(0, 188, 212))
     .setColorActive(color(0, 200, 83))
     .setLabel("Send")
     .onClick(new CallbackListener() {
       public void controlEvent(CallbackEvent theEvent) {
         sendRotationRef(); // Call function to send rotation reference
       }
     });

  // Display for motor speed
  cp5.addTextfield("SpeedDisplay")
     .setPosition(210, 240)
     .setSize(200, 30)
     .setFont(createFont("Arial", 16))
     .setColor(color(0, 0, 0))
     .setColorBackground(color(200, 200, 200))
     .setColorActive(color(255, 255, 255))
     .setLabel("")
     .setText(motorSpeed);

  // Display for motor position
  cp5.addTextfield("PositionDisplay")
     .setPosition(210, 280)
     .setSize(200, 30)
     .setFont(createFont("Arial", 16))
     .setColor(color(0, 0, 0))
     .setColorBackground(color(200, 200, 200))
     .setColorActive(color(255, 255, 255))
     .setLabel("")
     .setText(motorPosition);
     
  // Display for number of rotations made
  cp5.addTextfield("RotationsDisplay")
     .setPosition(210, 320)
     .setSize(200, 30)
     .setFont(createFont("Arial", 16))
     .setColor(color(0, 0, 0))
     .setColorBackground(color(200, 200, 200))
     .setColorActive(color(255, 255, 255))
     .setLabel("")
     .setText(motorRotations);
}

void draw() {
  background(33, 33, 33);
  
  // Displaying text titles for the interface
  fill(255);
  textSize(30);
  text("DC Motor Control", 195, 50);
  
  textSize(18);
  textAlign(LEFT);
  text("Enter Rotation Reference:", 50, 125);
  text("Speed (RPM): ", 50, 260);
  text("Position (degrees): ", 50, 300);
  text("Rotations made: ",50, 340);
  
}

// Function to send rotation reference from the GUI to Arduino
void sendRotationRef() {
  try {
    // Retrieve the user input from the text field
    inputText = cp5.get(Textfield.class, "rotationRef").getText();
    int rotationRef = Integer.parseInt(inputText); // Convert input to integer
    String message = rotationRef + "\n"; // Format message for serial transmission
    myPort.write(message); // Send message to Arduino
    println("Sent to Arduino: " + message); // Display message sent in console
  } catch (Exception e) {
    println("Invalid input for Rotation Reference"); // Error message for invalid input
  }
}

// Function to handle incoming data from Arduino
void serialEvent(Serial myPort) {
  String received = myPort.readStringUntil('\n'); // Read data until newline character
  if (received != null) {
    received = received.trim(); // Remove unwanted spaces or newlines
    println("Received from Arduino: " + received); // Display received data in console

    // If the received data is related to motor speed
    if (received.startsWith("Speed:")) {
      motorSpeed = received.substring(6).trim();  // Extract speed value
      motorSpeed = motorSpeed.replaceAll("[^0-9.]", ""); // Clean up any unwanted characters
      cp5.get(Textfield.class, "SpeedDisplay").setText(motorSpeed); // Display speed on GUI
    } 
    
    // If the received data is related to motor position
    else if (received.startsWith("Position:")) {
      motorPosition = received.substring(9).trim();  // Extract position value
      motorPosition = motorPosition.replaceAll("[^0-9.]", ""); // Clean up any unwanted characters
      cp5.get(Textfield.class, "PositionDisplay").setText(motorPosition); // Display position on GUI
    }
    
    // If the received data is related to rotations made
    else if (received.startsWith("Rotations Made:")) {
      motorRotations = received.substring(15).trim();  // Extract rotations count
      motorRotations = motorRotations.replaceAll("[^0-9.]", ""); // Clean up any unwanted characters
      cp5.get(Textfield.class, "RotationsDisplay").setText(motorRotations); // Display rotations on GUI
    }
  }
}  
