#!/bin/bash
echo "escoge opcion"
echo "1 unicode"
echo "2 ascii"
echo "3 ascii sin color"
read -p "Escriba su respuesta" opcion
if [ $opcion -eq 1 ]; then
	echo "escogiste la opcion 1"
	#ttyd --writable -p 8080 ./doom-ascii -iwad DOOM1.WAD -chars block
	./doom-ascii -iwad DOOM1.WAD -chars block
elif [ $opcion -eq 2 ]; then
	echo "escogiste la opcion 2"
	#ttyd --writable -p 8080 ../doom-ascii -iwad DOOM1.WAD -chars ascii
	./doom-ascii -iwad DOOM1.WAD -chars ascii 
elif [ $opcion -eq 3 ]; then
	echo "escogiste la opcion 3"
	#ttyd --writable -p 8080 ./doom-ascii -iwad DOOM1.WAD -chars ascii -nocolor
	./doom-ascii -iwad DOOM1.WAD -chars ascii -nocolor
else
	echo "No entiendo nada adios"
fi
