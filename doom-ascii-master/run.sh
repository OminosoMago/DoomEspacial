#!/bin/bash
echo "escoge opcion"
echo "1 unicode"
echo "2 ascii"
echo "3 ascii sin color"
read -p "Escriba su respuesta" opcion
if [ $opcion -eq 1 ]; then
	echo "escogiste la opcion 1"
	#ttyd --writable -p 8080 ./doom-ascii -iwad DOOM1.WAD -chars block
	cd _unix/game
	./doom-ascii -iwad /../../DOOM1.WAD -chars block -scaling 3
elif [ $opcion -eq 2 ]; then
	echo "escogiste la opcion 2"
	#ttyd --writable -p 8080 ../doom-ascii -iwad DOOM1.WAD -chars ascii
	cd _unix/game
	./doom-ascii -iwad /../../DOOM1.WAD -chars ascii -scaling 3
elif [ $opcion -eq 3 ]; then
	echo "escogiste la opcion 3"
	#ttyd --writable -p 8080 ./doom-ascii -iwad DOOM1.WAD -chars ascii -nocolor
	cd _unix/game
	./doom-ascii -iwad /../../DOOM1.WAD -chars ascii -nocolor -scaling 3
else
	echo "No entiendo nada adios"
fi
