#!/bin/bash

#(1)read all the sim
#(2)compute the avg of the sim, and then output

awk '
FILENAME==ARGV[1]{
    if (NF > 1) {
        record[FNR] = $2;
    }
}
FILENAME==ARGV[2]{
    if (NF >1) {
        if ($2 < record[FNR]){
            record[FNR] = $2;
        };
    }
}
FILENAME==ARGV[3]{
    if (NF >1) {
        if ($2 < record[FNR]){
            record[FNR] = $2;
        };
    }
}
FILENAME==ARGV[4]{
    if (NF >1) {
        if ($2 < record[FNR]){
            record[FNR] = $2;
        };
    }
}
FILENAME==ARGV[5]{
    if (NF >1) {
        if ($2 < record[FNR]) {
            record[FNR] = $2;
        }
        print $1, record[FNR];
    }
    else {
        print $0;
    }
}
' 1.sim 2.sim 3.sim 4.sim 5.sim > min.sim
