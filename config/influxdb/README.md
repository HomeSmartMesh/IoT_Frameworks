# Install on the Raspberry pi
sources:
- https://gist.github.com/boseji/bb71910d43283a1b84ab200bcce43c26
- https://docs.influxdata.com/influxdb/v1.4/introduction/getting_started/

```
sudo apt install apt-transport-https
echo "deb https://repos.influxdata.com/debian stretch stable" | sudo tee /etc/apt/sources.list.d/influxdb.list
sudo apt update

sudo apt-get install influxdb

```

### config
```
sudo nano /etc/influxdb/influxdb.conf
```

### start the service
```
sudo systemctl start influxdb
```

### help reminder, see getting started link for more
```
influx -precision rfc3339
CREATE DATABASE mydb,
SHOD DATABASES
USE mydb
cpu,host=serverA,region=us_west value=0.64
```
