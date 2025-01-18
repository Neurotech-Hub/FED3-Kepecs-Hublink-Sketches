#include <Wire.h>

// Different I2C frequencies to test
const uint32_t SPEEDS[] = {100000, 50000, 10000}; // 100kHz, 50kHz, 10kHz
const int NUM_SPEEDS = 3;
int currentSpeed = 0;
unsigned long lastSpeedChange = 0;
const unsigned long SPEED_CHANGE_INTERVAL = 5000; // Try each speed for 5 seconds

void setup()
{
    Serial.begin(115200);
    Serial.println("\nI2C Diagnostic Tool for Address 0x68");
    startI2C();
}

void startI2C()
{
    Wire.end(); // Stop any existing I2C communication
    delay(100); // Give it time to settle
    Wire.begin();
    Wire.setClock(SPEEDS[currentSpeed]);
    Serial.print("\nTesting I2C at ");
    Serial.print(SPEEDS[currentSpeed] / 1000.0);
    Serial.println(" kHz");
}

void loop()
{
    // Change speed every SPEED_CHANGE_INTERVAL milliseconds
    if (millis() - lastSpeedChange > SPEED_CHANGE_INTERVAL)
    {
        currentSpeed = (currentSpeed + 1) % NUM_SPEEDS;
        lastSpeedChange = millis();
        startI2C();
    }

    // First, try to detect device
    Wire.beginTransmission(0x68);
    byte error = Wire.endTransmission();

    Serial.print("Status at ");
    Serial.print(millis() / 1000.0, 1);
    Serial.print("s: ");

    if (error == 0)
    {
        Serial.println("Device responded!");

        // Try to read a byte to verify communication
        Wire.requestFrom(0x68, 1);
        if (Wire.available())
        {
            byte data = Wire.read();
            Serial.print("Read byte: 0x");
            Serial.println(data, HEX);
        }
        else
        {
            Serial.println("No data available on read");
        }
    }
    else
    {
        Serial.print("Error ");
        switch (error)
        {
        case 1:
            Serial.println("(data too long)");
            break;
        case 2:
            Serial.println("(NACK on address - device not responding)");
            break;
        case 3:
            Serial.println("(NACK on data)");
            break;
        case 4:
            Serial.println("(other error)");
            break;
        default:
            Serial.println("(unknown error)");
        }
    }

    // Print I2C status for scope debugging
    Serial.print("SCL State: ");
    Serial.print(digitalRead(SCL));
    Serial.print(", SDA State: ");
    Serial.println(digitalRead(SDA));

    delay(1000);
}