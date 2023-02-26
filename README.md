# Weekend project: Fri3d Badge 2020 + DHT22 Humidity Sensor + Orange Pi => Grafana Dashboard

This repo contains code for a weekend project for using my fri3d badge as an mqtt-connected dht22 sensor that sends data to an orange pi 5 with Orange Pi Os which runs a combination of node red with an mqtt broker and prometheus exporter, prometheus and grafana in docker containers. 

## Note about Flashing the Fri3d badge
In order to flash the fri3d badge,you should know that the esp32 first needs to be put into bootloader mode.
What works for me to do this is: 
- I took out the battery
- turned the on-off switch to on (display switch doesn't matter), 
- held the boot button on the badge while plugging in the usb cable in
- then pressed the reset button and release the boot button.

Probably it's simpler than these steps though, but this works.
