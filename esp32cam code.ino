#define MAX_RESOLUTION_VGA 1
#include <new_detection_object_inferencing.h>
#include "esp32cam.h"
#include "esp32cam/tinyml/edgeimpulse/FOMO.h"
#include <HardwareSerial.h>

using namespace Eloquent::Esp32cam;

Cam cam;
TinyML::EdgeImpulse::FOMO fomo;
// const int ledPin = 4;

void setup() {
    Serial.begin(115200);
    delay(1000);
    // pinMode(ledPin, OUTPUT);

    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.highestSaturation();
    cam.vga();

    while (!cam.begin()) {
        Serial.println(cam.getErrorMessage());
        delay(1000);
    }
}

void loop() {
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        delay(1000);
        return;
    }

    // Run FOMO model
    if (!fomo.detectObjects(cam)) {
        Serial.println(fomo.getErrorMessage());
        delay(1000);
        return;
    }

    // Print found bounding boxes
    if (fomo.hasObjects()) {
        ei_impulse_result_bounding_box_t bboxX;
        Serial.printf("Found %d objects in %d millis\n", fomo.count(), fomo.getExecutionTimeInMillis());

        fomo.forEach([&bboxX](size_t ix, ei_impulse_result_bounding_box_t bbox) {
            if (bbox.value > 0.7) {
                if (bbox.label != bboxX.label) {
                    
                    String dataToSend = bbox.label;
                    Serial.println(dataToSend);
                    delay(1000);
                    bboxX = bbox;
                }
            }
        });

        Serial.println();
        // digitalWrite(ledPin, HIGH);
        delay(2000);
        // digitalWrite(ledPin, LOW);
    } else {
        Serial.println("No objects detected");
        // digitalWrite(ledPin, LOW);
    }
}
