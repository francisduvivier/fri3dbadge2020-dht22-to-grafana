systemctl enable docker
mkdir /root/node-red-data -p 
cp -r -n my-setup/node-red-data /root/
chmod 777 /root/node-red-data
mkdir /root/prometheus_data -p
chmod 777 /root/prometheus_data
cp -r -n my-setup/grafana-data /root/
chmod 777 /root/grafana-data
docker-compose up -d

