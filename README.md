# Sanascope
SanascopeProject

## Vertical prototypes

### CStorageTest

Currently unable to write into the internal storage of the device.
Also the app crashes when trying to write onto the SD card.
Try requesting the writing permissions manually.

### OboeRecord
Records sound from an audio source (headset, if connected, phone mic otherwise)
simultaneously outputting it on the speakers or headphones
and writing it into RAM.

Needs the oboe library https://github.com/dromescu/oboe
and the FIR filter implementation by CardinalPeak
https://cardinalpeak.com/blog/a-c-class-to-implement-low-pass-high-pass-and-band-pass-filters/
in the same parent directory as /Sanascope to work properly.
