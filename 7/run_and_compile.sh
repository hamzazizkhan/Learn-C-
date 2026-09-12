#!/bin/zsh

./compile.sh

if [ $? -eq 0 ]
then
	echo "compiled succesfully"
else
	echo "compilation failed"
	exit
fi

./out
