
Required hardware: HC-05 bluetooth module "ZS-040" [JY-MCU]

Linux:
------

Code upload with Bluetooth is a bit flaky. Auto-reset timing is messy, but it does work often enough.
You may need to use the avrdude-wrapper script. See the arduino_setup foler.

* Go to "<arduino-folder>/hardware/tools/" and rename "avrdude" to "avrdude-bin"
* Copy the wrapper-script to "<arduino-folder>/hardware/tools/"

