# Sanascope
SanascopeProject

## Vertical prototypes

### CStorageTest

Writing one file from Java code and one from C++ code into the public directory.
Contains a workaround for an MTP bug preventing the created files and directories
to show up in the explorer when the device is connected as mass storage.

### OboeRecord
Records sound from an audio source (headset, if connected, phone mic otherwise)
simultaneously outputting it on the speakers or headphones
and writing it into RAM.

Needs the oboe library https://github.com/dromescu/oboe
and the FIR filter implementation by CardinalPeak
https://cardinalpeak.com/blog/a-c-class-to-implement-low-pass-high-pass-and-band-pass-filters/
in the same parent directory as /Sanascope to work properly.
