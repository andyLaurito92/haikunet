#!/bin/bash

currentPath="`pwd`"   # current directory
MY_PATH="`dirname \"$0\"`"              # relative
MY_PATH="`( cd \"$MY_PATH\" && pwd )`"  # absolutized and normalized

# Sclilab backdoor need to be started from the powerdevs directory
cd $MY_PATH/../output/

# start scilab normaly with backdoor
$MY_PATH/../../scilab-5.5.1/bin/scilab  -f $MY_PATH/loadBackDoor.sce

# return to previous directory
cd $currentPath
