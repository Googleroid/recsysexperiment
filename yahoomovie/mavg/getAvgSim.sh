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
        record[FNR] +=$2;
    }
}
FILENAME==ARGV[3]{
    if (NF >1) {
        record[FNR] +=$2;
    }
}
FILENAME==ARGV[4]{
    if (NF >1) {
        record[FNR] +=$2;
    }
}
FILENAME==ARGV[5]{
    if (NF >1) {
        record[FNR] +=$2;
        printf "%d %.6f\n",($1,record[FNR]/5);
    }
    print $0;
}
' 1.sim 2.sim 3.sim 4.sim 5.sim > avg.sim
