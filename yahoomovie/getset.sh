#!/bin/bash
if [ -f temp ];
then
    rm temp
fi
if [ -f firstOnly ];
then
    rm firstOnly
fi
if [ -f secondOnly ];
then
    rm secondOnly
fi
if [ -f common ];
then
    rm common 
fi

cat $1 > temp
cat $2 >> temp
cat $2 >> temp
sort -T ./ temp | uniq -c | awk '{
if (1 == $1) {
    print $2 >> "firstOnly"
}
else if(2 == $1) {
	print $2 >> "secondOnly"
}
else if(3 == $1) {
	print $2 >> "common"
}
}'
