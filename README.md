# Weekend Project: Fri3d Badge 2020 + DHT22 Humidity Sensor + Orange Pi 5 => Grafana Dashboard

This repo contains code for a weekend project for using my fri3d badge as an mqtt-connected dht22 sensor that sends data to an Orange Pi 5 with Orange Pi Os which runs a combination of Node-RED with an MQTT Broker and a Prometheus exporter, Prometheus and Grafana in Docker containers. 
## Result pictures 

Grafana Dashboard running from Docker containers on Orange Pi 5 |  Badge showing DHT22 readings
:-------------------------:|:-------------------------:
![Front Of Fri3d badge showing humidity, temperature and wifi and mqtt connection](doc/res/fri3d-dht22-badge-grafana-dashboard-pretty.png) |  ![Front Of Fri3d badge showing humidity, temperature and wifi and mqtt connection](doc/res/fri3d-dht22-badge-front.png)
## Notes about Flashing the Fri3d badge
### Bootloader mode needs to be activated
In order to flash the Fri3d badge, you should know that the esp32 first needs to be put into bootloader mode.

What works for me to do this is: 
- I took out the battery
- turned the on-off switch to on (display switch doesn't matter), 
- held the boot button on the badge while plugging in the usb cable in
- then pressed the reset button and release the boot button.

Probably it can be simpler than these steps though, but this works as well.

### Arduino IDE setup for development and flashing
To get the Arduino Ide working for reading and showing DHT values on the  Fri3d Badge, the following steps were done:
- Add the ESP32 board manager url
- install the ESP boards
- Install Adafruit GFX Library
- Install Adafruit's DHT Sensor Library
- Install Display library: https://github.com/adafruit/Adafruit-ST7735-Library
- Then select ESP32 Dev Module as the board
- Then select the upload port 
- Then flash after putting the device in bootloader mode.
Then for the mqtt, ota, wifi and async event code I added some more libraries:
- knolleary/PubSubClient@^2.8
- aasim-a/AsyncTimer@^2.4.0
- ArduinoOTA


Note that to make this work on linux mint I had to change some group stuff for being able to access the dev/ttyUSB0 as non-root.
Following https://askubuntu.com/questions/133235/how-do-i-allow-non-root-access-to-ttyusb0, I did:
- `sudo usermod -a -G dialout $USER`
- `reboot`
on my linux mint development pc

### Notes regarding Web Flashing options 
#### Arduino Web Editor
- **Conclusion: not really usable for development! See below**
- Flashing and arduino development is possible from web with the Arduino Web Editor : https://create.arduino.cc/editor. Notes here:
  - **Has a limit on the amount of times you can compile in the free version**
  - Requires a plugin to be installed
  - In my case, on linux mint, I also needed to install an extra package
  - Board to select is ESP32 Dev Module
  - The web interface runs stuff on the host pc via the plugin, and also allows to load zipped library files in the cloud compilation environment, so we can zip and load the Fri3d Badge 2020 Arduino Library zip. https://github.com/Fri3dCamp/Badge2020_arduino/archive/refs/heads/main.zip library.

#### Fried Flasher
- For the Fri3d firmware zip files, you can use https://fri3d-flasher.vercel.app/#/
- This might be useful if you want to go back to original firmware, or just wanna try that flashing works.


## Notes regarding Orange Pi OS
Orange Pi OS looks to be an armbian arm64 fork with some tweaks for adding their UI and some extra software. Notably:
- You can easily install some media server software and a pi hole from the orangepi-config tool
- Docker and docker-compose is already preinstalled and working well
- `armbian` in the commands are renamed to `orangepi`, so eg `orangepi-config` instead of `armbian-config`, but also `orangepimonitor` instead of `armbianmonitor` etc.

Default username and password is `root`: `orangepi`.
default hostname is orangepi5

## About Orange Pi Docker setup
to start the required Docker containers, we need to create some directories for Node-RED, Prometheus and Grafana data with correct access rights for Docker and then starting the containers with docker-compose. You can do this by running:
- `./go.sh`

## Hardware
### Back of Badge Picture with soldered wires
![Back of Fri3d badge showing soldered wires for dht22 connection](doc/res/fri3d-dht22-badge-back.png)

## TODO
- Clean up Arduino Code
- Make it more efficient to add and extra device or an extra sensor
- Improve documentation regarding Node-RED and Grafana configuration after deployment   
- Include Node-Red Config in Docker image or mounted files in deployment
- Include Grafana Config in new Docker image or mounted files in deployment
- Add Wiring diagram
- Make project Simulatable in WokWi (de display is an issue here)
- Test Arduino OTA updates

## Useful related links:
### Fri3d Badge
- https://github.com/Fri3dCamp/badge-2020
  - https://github.com/Fri3dCamp/badge-2020/tree/master/firmware
- https://fri3d-flasher.vercel.app/#/
- https://github.com/Fri3dCamp/Badge2020_arduino
- https://hackaday.io/project/169741-fri3d-2022-badge
- https://docs.espressif.com/projects/esptool/en/latest/esp32/advanced-topics/boot-mode-selection.html
- https://www.esp32.com/viewtopic.php?t=5682#p24710
- https://create.arduino.cc/editor
- https://askubuntu.com/questions/133235/how-do-i-allow-non-root-access-to-ttyusb0

### Node-RED + Plugins + Docker + Prometheus + Grafana
- https://nodered.org/docs/getting-started/docker
- https://flows.nodered.org/node/node-red-contrib-aedes (MQTT Broker for Node-RED)
  - https://github.com/martin-doyle/node-red-contrib-aedes/tree/v0.8.2
- https://flows.nodered.org/node/node-red-contrib-prometheus-exporter
  - https://github.com/Docoyo/node-red-contrib-prometheus-exporter/tree/1.0.5
- https://prometheus.io/
- https://grafana.com/
- https://www.catchpoint.com/blog/node-red-influxdb-grafana (Similar project)

### Orange Pi 5 with Orange Pi OS
- http://www.orangepi.org/html/hardWare/computerAndMicrocontrollers/details/Orange-Pi-5.html
- https://drive.google.com/drive/folders/1YxOLUAunufM6NzAfeEmt5tUnAFOu5bdA (Orange Pi OS drive)
- https://drive.google.com/drive/folders/1Bre2q0bGgXQuQlYaYDMvwstpvtHLmcgX (Orange Pi user manual drive)
- https://www.youtube.com/watch?v=cBqV4QWj0lE (Orange Pi 5 First install guide, not completely followed, we just use sd card)
- https://www.balena.io/etcher
- https://www.sdcard.org/downloads/formatter/ (formatter advised to be used before flashing image on sd card)

## My Node-RED docker image for this:
- https://hub.docker.com/r/francisduvivier/node-red-with-mqtt-and-prometheus

## Web Arduino And ESP32 simulator: project used for trying out text centering code:
- https://wokwi.com/projects/357757982958445569

## DHT22 Temperature and Humidity sensor
- https://lastminuteengineers.com/dht11-dht22-arduino-tutorial/
