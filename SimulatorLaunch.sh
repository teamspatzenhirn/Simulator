#!/bin/bash
LD_LIBRARY_PATH="/opt/Simulator/lib/"
SPATZSIM_RESOURCE_PATH="/opt/Simulator/"
RET=$(/opt/Simulator/SpatzSim "$@")
