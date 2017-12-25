#!/bin/bash
echo "Restoring the influx Databse"
sudo systemctl stop influxdb

backup_path=$1
influxd restore -metadir /var/lib/influxdb/meta $backup_path
influxd restore -database meshNodes -datadir /var/lib/influxdb/data $backup_path
influxd restore -database raspiStatus -datadir /var/lib/influxdb/data $backup_path

sudo chown -R influxdb:influxdb /var/lib/influxdb

sudo systemctl start influxdb
