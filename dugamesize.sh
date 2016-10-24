#!/bin/bash

if [ -d /media/usb0/shimmergames/"$1" ];then
	du -sm /media/usb0/shimmergames/"$1"
fi
