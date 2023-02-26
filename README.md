# Weekend Project: Fri3d Badge 2020 + DHT22 Humidity Sensor + Orange Pi 5 => Grafana Dashboard

This repo contains code for a weekend project for using my fri3d badge as an mqtt-connected dht22 sensor that sends data to an orange pi 5 with Orange Pi Os which runs a combination of node red with an mqtt broker and prometheus exporter, prometheus and grafana in docker containers. 

## Note about Flashing the Fri3d badge
In order to flash the fri3d badge, you should know that the esp32 first needs to be put into bootloader mode.

What works for me to do this is: 
- I took out the battery
- turned the on-off switch to on (display switch doesn't matter), 
- held the boot button on the badge while plugging in the usb cable in
- then pressed the reset button and release the boot button.

Probably it's simpler than these steps though, but this works.
Flashing and arduino development is possible from web with a plugin: https://create.arduino.cc/editor, or for the fri3d firmware zip files on https://fri3d-flasher.vercel.app/#/.

Note that to make this work on linux mint I had to change some group stuff for being able to access the dev/ttyUSB0 as non-root.
Following https://askubuntu.com/questions/133235/how-do-i-allow-non-root-access-to-ttyusb0, I did:
- `sudo usermod -a -G dialout $USER`
- `reboot`
on my linux mint development pc


## Note about Orange pi OS
Orange PI os looks to be an armbian fork with some tweaks for adding their UI and some extra software. Notably, you can easily install some media server software and a pi hole from the orangepi-config tool.
And advantage is also that Docker is already preinstalled and working well.
Default username and password there is root: orangepi.
default hostname is orangepi5

## About Orange Pi docker setup.
to start the required docker containers, we need to create a directory for nod red data with correct access rights for docker and then run docker-compose up -d. You can do this by running:
- `./go.sh`

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
- https://flows.nodered.org/node/node-red-contrib-aedes (MQTT Broker for node red)
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
