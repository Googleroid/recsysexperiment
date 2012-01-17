#!/bin/sh
#usage: ./mapId.sh mapFile sourcFile destFile
#./mapId.sh ../yahooMovie/itemId_old_new.map ../yahooMovie/selectRatings.maped ../yahooMovie/yahooMovie.data

#this script map the old userId to new userId to keep the sequential character of the userIds
#and at the same time select
destFile=$3
mapFile=$1
srcFile=$2
if [ -f $destFile ];
then
    rm $3
fi
awk '
NR==FNR{
    { #first use the dict file to get the select User dict
        selectUser[$1] = $3;
    }
}
NR>FNR{
    { #select and map the ratings
        if (selectUser[$1]) { #if is select
            print selectUser[$1], $2, $3, $4, $5, $6, $7
#            print $1,$2,$3,$4,$5,$6,$7 >> "../yahooMovie/selectRatings.txt"
#            print $0 >> "../yahooMovie/selectRatings.txt"


        }
    }
}
END {
    for (user in selectUser) {
#    print selectUser[user];
    }
}
' $mapFile $srcFile > $destFile
