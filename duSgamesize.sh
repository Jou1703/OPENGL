#!/bin/bash

if [ -d /home/steam/.local/share/Steam/steamapps/common/"$1" ];then
	du -sm /home/steam/.local/share/Steam/steamapps/common/"$1"
fi
