#!/bin/bash
filename="$1"
startLine=$2
endLine=$3

awk -v start=$2 -v end=$3 '
BEGIN{
#    print start
#    print end
}
{
    if (FNR>=start && FNR<=end) {
        print $0;
    }
    else if (FNR > end) {
        exit;
    }
}
END{
}' "$1"
