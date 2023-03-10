FROM nodered/node-red

RUN npm install --save node-red-contrib-prometheus-exporter
RUN npm install --save node-red-contrib-aedes
RUN npm install --unsafe-perm --no-update-notifier --no-fund --only=production

# MQTT Broker
EXPOSE 1883/tcp

# Node Red server, includes prometheus exporter endpoint at /metrics
EXPOSE 1880/tcp
