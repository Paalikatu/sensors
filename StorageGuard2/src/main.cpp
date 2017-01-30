/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * Simple binary switch example
 * Connect button or door/window reed switch between
 * digitial I/O pin 3 (BUTTON_PIN below) and GND.
 * http://www.mysensors.org/build/binary
 */

#define MY_RF24_PA_LEVEL RF24_PA_MAX
#define MY_DEBUG
#define MY_RADIO_NRF24
#define BOUNCE_LOCK_OUT


#include <SPI.h>
#include <MySensors.h>
#include <Bounce2.h>

#define CHILD_ID 3
#define SWITCH_PIN 3

#define SLEEP_MAX_MS 3600000

#define DEBOUNCE_WAIT 100

int BATTERY_SENSE_PIN = A0;
int oldBatteryPcnt = 0;


Bounce debouncer = Bounce();
int oldValue=-1;

// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(CHILD_ID,V_TRIPPED);

void setup()
{
    // Setup the switch
    pinMode(SWITCH_PIN, INPUT);
    // Activate internal pull-up
    digitalWrite(SWITCH_PIN, HIGH);

    // After setting up the button, setup debouncer
    //debouncer.attach(SWITCH_PIN);
    //debouncer.interval(4);

    // use the 1.1 V internal reference
#if defined(__AVR_ATmega2560__)
    analogReference(INTERNAL1V1);
#else
    analogReference(DEFAULT);
#endif


}

void presentation() {
    // Register binary input sensor to gw (they will be created as child devices)
    // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
    // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
    sendSketchInfo("StorageGuard", "1.0");
    present(CHILD_ID, S_DOOR);
}


//  Check if digital input has changed and send in new value
void loop()
{
    //sleep(5, false);
    // Check the switch state and report if something has changed
    //debouncer.update();
    // Get the update value
    //int value = debouncer.read();

    // Wait for a certain amount before reading the pin
    while (true) {
        wait(DEBOUNCE_WAIT);

        int value = digitalRead(SWITCH_PIN);
        if (value != oldValue) {
            // Send in the new value
            Serial.print(value);
            send(msg.set(value==HIGH ? 1 : 0));
            oldValue = value;
        } else {
            break;
        }
    }

    // Deliver battery level
    int sensorValue = analogRead(BATTERY_SENSE_PIN);
    int batteryPcnt = sensorValue / 10;
    if (oldBatteryPcnt != batteryPcnt) {
        // Power up radio after sleep
        sendBatteryLevel(batteryPcnt);
        oldBatteryPcnt = batteryPcnt;
    }



    // And get back to sleep!
    sleep(digitalPinToInterrupt(SWITCH_PIN), CHANGE, SLEEP_MAX_MS, false);

}
