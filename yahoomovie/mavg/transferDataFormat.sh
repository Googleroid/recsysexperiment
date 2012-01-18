#!/bin/bash

#this file transfer the multi criteria data of yahoo to movielens format and then can use the 
#movilens's knn programme to compute the RMSE

#steps:
#(1)read every line 
#(2)just print the overall rate, and separater the field by tab. userId itemId  rating  timestamp
inputFile=$1
awk  '
BEGIN{
    FS=" ";
    OFS="\t";
}
{
    if ($7 !=0 && $7 != -1) { #only output the valid ratings
        print $2,$1,$7,"1234567";
    }
}' $inputFile
