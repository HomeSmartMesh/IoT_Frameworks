# Install on the Raspberry pi
 - https://github.com/fg2it/grafana-on-raspberry/wiki

please refer to the original source of info above or to the latest refrence.
Notes below are just to speedup installs and not considered as complete tutorials

### Dashboards
- "dashboards" subdirectory provides json files that can be uploaded to a new Grafana install

### Preparation
```
sudo apt-get install apt-transport-https curl
curl https://bintray.com/user/downloadSubjectPublicKey?username=bintray | sudo apt-key add -
echo "deb https://dl.bintray.com/fg2it/deb stretch main" | sudo tee -a /etc/apt/sources.list.d/grafana.list
```
### Install
```
sudo apt-get update
sudo apt-get install grafana
```
### start service
```
systemctl daemon-reload
systemctl start grafana-server
systemctl status grafana-server
```
### service start issues
had to start the service from /etc/rc.local
```
sudo systemctl start grafana-server.service
```
### config
```
sudo nano /etc/grafana/grafana.ini
```
### default users
admin / admin
### snapshots
 - https://snapshot.raintank.io/dashboard/snapshot/XUeMIkt3T936k036wl8pFE7acaLqONvt?orgId=2&from=1513890000000&to=1513976400000
