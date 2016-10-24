#!/bin/bash
code="CODE`date +%y%m%d`.tgz"
source="SOURCE`date +%y%m%d`.tgz"  
ui="UI.tgz" 

case $1 in
	'c')
	tar zcvf $code *.cpp *.h makefile *.sh alsa_src/*.h alsa_src/*.cpp
	;;
	's')
	tar zcvf $source sound image font file.dat *.sh opt.conf
	;;
	'u')
	tar zcvf $ui SteamUI tar_font
	;;
esac

