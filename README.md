# Sanascope
SanascopeProject

## Vertical prototypes

### CStorageTest

Currently unable to write into the internal storage of the device.
Also the app crashes when trying to write onto the SD card.
Try requesting the writing permissions manually.

### OboeRecord
Needs the oboe library https://github.com/dromescu/oboe to be cloned
into the same parent directory as /Sanascope to work properly.

Audio input stream can be created, started, stopped and closed.
However, the app crashes after reading a few frames.
Possibly there are issues with buffers or memory allocation.
