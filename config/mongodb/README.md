# Install on the Raspberry pi
sources:
- http://yannickloriot.com/2016/04/install-mongodb-and-node-js-on-a-raspberry-pi/

### dependencies
- node

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install mongodb-server
sudo service mongod start
mongo
```

### config
```
sudo nano /etc/mongodb.conf
```
