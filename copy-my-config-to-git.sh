mkdir -p my-setup/grafana-data
cp /root/grafana-data/grafana.db my-setup/grafana-data/
mkdir -p my-setup/node-red-data
cp /root/node-red-data/flows.json my-setup/node-red-data/
cp /root/node-red-data/settings.js my-setup/node-red-data/
cp /root/node-red-data/package.json my-setup/node-red-data/
git add my-setup
