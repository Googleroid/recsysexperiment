#!/bin/sh

#this file get the new userId form the sequential of the user
#and then get the oldUserId-newUserId map

filename="../yahooMovie/selectUserId.txt"
newFilename="../yahooMovie/userId_old_new.map"
awk '{print $0,NR}' $filename > $newFilename
