# FED3 Hublink

This repository contains the code for the FED3 Hublink project. The main sketch can be found in [FED3_Hublink_Kepecs/FED3_Hublink_Kepecs.ino](FED3_Hublink_Kepecs/FED3_Hublink_Kepecs.ino). This sketch was converted from the original in [DanielFED082824/DanielFED082824.ino](DanielFED082824/DanielFED082824.ino) to simplify and reduce complexity.

## Updating Firmware

You **must** use the [FED3 Library Fork](https://github.com/Neurotech-Hub/FED3_library) managed by the Neurotech Hub. This ensures compatibility with the new ESP32-based chip. See the note about RTC updates in the sketch: results may vary between operating systems. When connecting to Hublink, the RTC will be set to the current time via the `onTimestampReceived` callback.

1. Download the [FED3 Library Fork](https://github.com/Neurotech-Hub/FED3_library) and the Hublink-Node Library by Matt Gaidica from Arduino IDE (see documentation here: [Hublink-Node](https://github.com/Neurotech-Hub/Hublink-Node)).
2. For downloaded libraries, go to Sketch -> Include Library -> Add .ZIP Library, or place the library in the `libraries` folder in the Arduino IDE.
3. If the device has been previously flashed, it may be in a light sleep state that prevents it from connecting to the serial port. To enter boot mode, hold the `Boot` button and toggle the `Reset` button (then release the `Boot` button).
4. If the Arduino IDE does not indicate that it is connected to "Adafruit Feather ESP32-S3 2MB PSRAM", click Tools -> Board -> esp32 -> Adafruit Feather ESP32-S3 2MB PSRAM (you will need to download the esp32 board package (by espressif) from the Arduino IDE).

If you wish to modify the sketch itself, it is highly recommended to become a collaborator on this repository and use proper git workflows. This will reduce the risk of delpoying out of date code and makes identifying errors easier across multilpe authors.

## Hublink

[Hublink.cloud](https://hublink.cloud) is meant to transfer SD card content to the cloud. Your lab will have a dashboard link that should be for internal use only. The [Hublink Docs](https://hublink.cloud/docs) contain information about how format the `meta.json` file on the SD card, which is critical for:

1. Setting up a path structure for the cloud storage.
2. Setting subject data that can be used to track the experiments.
3. Adjust settings of the Hublink sync process.

## Troubleshooting

While in beta testing, please get in touch with the Neurotech Hub or email [hi@hublink.cloud](mailto:hi@hublink.cloud) if you have any questions or issues.