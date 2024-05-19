IOTUSB - v0.8.2

IOTUSB - Control devices via USB
====== = ======= ======= === ===

IOTUSB is a service that runs on linux to communicate with USB IO boards to use for IOT purposes.
The USB  board has a communication channel to send and receive commands. The service takes case of commicating with
a smarthome platform.

4relays:
--------

At this moment only '4relays' (revision 1) hardware (IOTrelaysUSB) is added. This board can drive up to 4 relays and
4 buttons/ LEDs. The code is designed for an Arduino micro or LEonardo, but should work for other boards to after
some minor tweaks (like port assignment). Becuase of some issues with idle mode and sending data over USB, standard
Arduino code is tweaked to make the board more reliable. E.g. the millis and delay functions are reprogrammed to timer1
on which the interrupt is used to wake up from idling every 100 ms (10 Hz loop). When the CPU is busy handling IOs or
data, the idle mode is not entered for about 1 second. The serial interface is tweaked to wakeup USB on sending data
on idle.

With EEPROM settings, the following functions can be set (EEPROM settings are entered into settings file)
Enable0-3: Connect relay to switch. Relay 0 to switch 0, etc. This means that the relay will follow a button press by
toggling on or off. If not enabled, the relay can be switched on or off via the smarthome platform and the button state
can be read out separately.
Latch0-3: Use a latching relay that can be set or reset by a pulse. Now the relay coil is only energized for 1 second
when switching, saving power for relays that need to stay on for a long time. Take into account to use a latching relay for
this function. IF you only have a normal relay, set this to zero.
Inverse0-3: Inverse the relay outputs
Buttonsw0-3: If 1 then a switch is used instead of a button. Take care that a switch overrules the setting by the smarthome
platform.
LowPower: Use lowpower mode to keep the CPU idle for most of the time.
LowPowerLEDs: Do not keep the LEDs on all time, but just flash a few times when the state changes

Pressing the button for 1 second, will lock the relay in on position. This means the switching off by the smarthome platform
is not possible. The relay can be unlocked by pressing the button again.
Pressing button 0 for more than 3 seconds will reset the board when the button is released.

IOTUSB:
-------

IOTUSB is a service, so no user interaction is required. 

The USB system is hot pluggable (service doesn't need to be restarted) and will also survive unplugging, device reboot etc.

To communicate with the smarthome platform, 2 options are implemented:

restapi:
--------

Via http a command can be send to the service. The service then gives a response in JSON format.

Status requests are done by a http get in the following format:
http://username:password@localhost:port/device[/tag]
The status response is then given in JSON. If the service doesn't run on the localhost, the IP address of that system must 
be entered.
The get request can be entered in a browser or placed by a tool like curl:
curl -X get http://username:password@localhost:port/device[/tag]

command requests are done by a http put in the following format:
http://username:password@localhost:port/device
The payload is in JSON format: {"tag1": "value1", "tag2": "value2"}
The put request can be placed by curl:
curl -X PUT http://localhost:8080/device -d "{\"tag1\":\"value1\",\"tag2\":\"value2\"}"
To test/ use the system without the availability of curl (but only a browser), as faultback commands can also be entered by
a http get in the following format:
http://username:password@localhost:port/device?tag1=value1&tag2=value2

mqtt:
-----

As lot of smarthome platforms support MQTT, this is the prefered way to interact with the relays.
commands needs to be published and status needs to be received (subscribed) in the following format:
[maintopic]/[device]/tag1
For use with home assistant, device discovery can be used. 

Installation:
-------------
Manual installation can be done as follows:
- Browse to: https://github.com/Helly1206/iotusb
- Click the 'Clone or Download' button
- Click 'Download Zip'
- Unzip the zip file to a temporary location
- Open a terminal at this location
- Enter: 'sudo ./install.sh'
- Wait and answer the questions:
	Do you want to install an automatic startup service for iotusb (Y/n)?
   		Default = Y
   		If you want to automatically start iotusb during startup (or don't know), answer Y to this question.
   		If you do not want to install an automatic startup script, answer N to this question.

Installer options:
--------- --------
sudo ./install.sh    --> Installs iotusb
sudo ./install.sh -u --> Uninstalls iotusb
sudo ./install.sh -c --> Deletes compiled files in install folder (only required when copying or zipping the install folder)
sudo ./install.sh -d --> Builds debian packages

Package install:
------- --------
iotusb installs automatically from deb package/ apt repository (only for debian based distros like debian or ubuntu).
Just enter: 'sudo apt install iotusb' after adding the repository
see: https://github.com/Helly1206/hellyrepo for installing the repository

Configuration:
--------------
In /etc/iotusb.yml, the configuration file is found.
Example:
general:                 # general settings
  logging: debug         # logging: info, debug, error, use info as default

restapi:                 # restapi settings
  enable: true           # enable restapi
  port: 8080             # restapi port
  username:              # restapi username, no username if not entered or empty
  password:              # restapi password, no password if not entered or empty. Username is required for password.

mqtt:                    # mqtt settings
  enable: true           # enable mqtt
  broker: 192.168.x.y    # mqtt broker address or url
  port: 1883			 # mqtt port
  username: xxx			 # mqtt username, no username if not entered or empty
  password: xxxxx        # mqtt password, no password if not entered or empty. Username is required for password.
  maintopic: myhome      # mqtt main topic for all devices
  hatopic: homeassistant # home assistant topic for device discovery. No discovery if empty or not entered.
  qos: 0                 # qos for mqtt data
  retain: true           # retain published messages. True is suggested.

devices:				 # here devices are added
  device1:               # the name of the first device (used for detection of device hardware and in restapi and mqtt topic)
    clear: false         # if true, the device name of the hardware is cleared (only execute once to change device name)
    type: 4relays        # device type
    alias:               # aliases (names) for relays and buttons (if not entered defaults realy1-4 and button1-4 are used)
      cmd_suffix:        # suffix for commands over mqtt. default empty
      st_suffix: status  # suffix for stauts over mqtt. default st
      relay1: mylights
      relay2: myotherlights
      relay3: spare1
      relay4: spare2
      button1: btnmylights
      button2: btnmyotherlights
      button3: btnspare1
      button4: btnspare2
    eeprom:				 # eeprom settings. EEPROM is only overwritten if settings change.
      Enable0: 1         # enable relay and button connection 1
      Enable1: 1
      Enable2: 0
      Enable3: 0
      Latch0: 1          # use latching relay 1
      Latch1: 1
      Latch2: 1
      Latch3: 1
      Inverse0: 0        # inverse relay 1
      Inverse1: 0
      Inverse2: 0
      Inverse3: 0
      Buttonsw0: 0       # use button 1 as button (0) or switch (1)
      Buttonsw1: 0
      Buttonsw2: 0
      Buttonsw3: 0
      Lowpower: 1        # use in low power
      LowpowerLEDs: 1    # use low power LEDs

When a new device or device without name is inserted, the USB ports are looped through and the first device that is empty or
cleared (or the correct type), will be given this name. 

After changing the yaml configuration, the service needs to be restarted. Devices do not need to be reset.

That's all for now ...

Have fun

Please send Comments and Bugreports to hellyrulez@home.nl
