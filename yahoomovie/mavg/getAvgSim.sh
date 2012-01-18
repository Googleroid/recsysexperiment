#!/bin/bash

#(1)read all the sim
#(2)compute the avg of the sim, and then output

awk '
{
    if(1==ARGIND) {
        print $0;
    }
    else if(2==ARGIN) {
        print $0;
    }
}' 1.sim 2.sim 3.sim 4.sim 5.sim
