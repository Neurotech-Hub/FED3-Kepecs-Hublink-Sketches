# FED3 Hublink

This repository contains the code for the FED3 Hublink project. The main sketch can be found in [FED3_Hublink_Kepecs/FED3_Hublink_Kepecs.ino](FED3_Hublink_Kepecs/FED3_Hublink_Kepecs.ino). This sketch was converted from the original in [DanielFED082824/DanielFED082824.ino](DanielFED082824/DanielFED082824.ino) to simplify and reduce complexity.

## Updating Firmware

You **must** use the [FED3 Library Fork](https://github.com/Neurotech-Hub/FED3_library) managed by the Neurotech Hub. This ensures compatibility with the new ESP32-based chip. See the note about RTC updates in the sketch: results may vary between operating systems. When connecting to Hublink, the RTC will be set to the current time via the `onTimestampReceived` callback.

If you wish to modify the sketch itself, it is highly recommended to become a collaborator on this repository and use proper git workflows. This will reduce the risk of delpoying out of date code and makes identifying errors easier across multilpe authors.

## Hublink

[Hublink.cloud](https://hublink.cloud) is meant to transfer SD card content to the cloud. Your lab will have a dashboard link that should be for internal use only. The [Hublink Docs](https://hublink.cloud/docs) contain information about how format the `meta.json` file on the SD card, which is critical for:

1. Setting up a path structure for the cloud storage.
2. Setting subject data that can be used to track the experiments.
3. Adjust settings of the Hublink sync process.

## Troubleshooting

While in beta testing, please get in touch with the Neurotech Hub or email [hi@hublink.cloud](mailto:hi@hublink.cloud) if you have any questions or issues.