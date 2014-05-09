#!/bin/bash
# /etc/init.d/vCamServer.sh

### BEGIN INIT INFO
# Provides:          vCamServer
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: runs vCam
# Description:       Feeds a vPlotterServer with images from the camera
### END INIT INFO

SERVER_PATH=/home/pi/openFrameworks/apps/Skylines01/vCamServer
SERVER_JS=server.js
USER=pi
OUT=nodejs.log

case "$1" in

start)
  echo "Starting Node.js"
  cd $SERVER_PATH
  sudo -u $USER node $SERVER_JS >> $OUT 2>>$OUT&
	;;

stop)
	killall node
	;;

*)
	echo "usage: $0 (start|stop)"
esac

exit 0
