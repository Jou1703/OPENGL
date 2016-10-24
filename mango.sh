#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/steam/.mango/lib
if [ -f "/home/steam/.mango/upgrade/mangotv" ]
then
exec /home/steam/.mango/upgrade/mangotv
else
exec /home/steam/.mango/bin/mangotv
fi
