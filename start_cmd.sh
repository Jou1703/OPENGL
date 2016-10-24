#!/bin/bash
echo "start_cmd run"
if [ -f "/home/steam/.shimmer/mango_beta_20160816.tar.gz" ];then
	echo "tar"
	tar -xf /home/steam/.shimmer/mango_beta_20160816.tar.gz -C /home/steam
fi

if [ -f "/home/steam/.shimmer/mango_beta_20160816.tar.gz" ];then
        rm /home/steam/.shimmer/mango_beta_20160816.tar.gz -rf
fi
