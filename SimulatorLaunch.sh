#!/bin/bash

export LD_LIBRARY_PATH="/opt/Simulator/lib/"
export SPATZSIM_RESOURCE_PATH="/opt/Simulator/"

RET=$(/opt/Simulator/SpatzSim "$@")
