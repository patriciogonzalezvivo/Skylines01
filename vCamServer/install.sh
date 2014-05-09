#!/bin/sh

# Install Server
chmod 755 vCamServer.sh
sudo cp vCamServer.sh /etc/init.d
sudo update-rc.d vCamServer.sh defaults
