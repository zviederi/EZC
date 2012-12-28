#!/bin/bash
# Created by Eriks
#
# simply GPY monitoring tool
# works only on Nvidia cards
# for using this tool need nvidia-smi tool
export GPU=0
export TEMP=0
export MEMORY=0
function get_gpu
{
  position=`nvidia-smi | grep '%' | grep -b -o % | awk 'BEGIN {FS=":"}{print $1}'`
  GPU=`nvidia-smi | grep '%' | cut -c $(($position-3))-$(($position+2))`
}
function get_temp
{
  position=`nvidia-smi | grep '%' | grep -b -o C | awk 'BEGIN {FS=":"}{print $1}'`
  TEMP=`nvidia-smi | grep '%' | cut -c $(($position-3))-$(($position+2))`
}
function get_memory
{
  position=`nvidia-smi | grep '%' | grep -b -o 'MB /' | awk 'BEGIN {FS=":"}{print $1}'`
  MEMORY=`nvidia-smi | grep '%' | cut -c $(($position-3))-$(($position+12))`
}
echo -e "\033[30;42mPress Ctrl-C to break ...\033[0m"
    while true; do
        get_gpu
        get_temp
        get_memory
        echo GPU: ${GPU} Temperature: ${TEMP} Memory-Usage: ${MEMORY}
        sleep 1
    done
