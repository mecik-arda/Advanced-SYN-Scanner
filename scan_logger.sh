#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: ./scan_logger.sh <Target_IP> <End_Port>"
    exit 1
fi

TARGET=$1
PORT_LIMIT=$2
MY_IP=$(hostname -I | awk '{print $1}')
LOG_FILE="scan_results_$(date +%Y%m%d_%H%M%S).log"

echo "--- Hades Scan Session Start: $(date) ---" > "$LOG_FILE"
echo "Target: $TARGET | Source: $MY_IP" >> "$LOG_FILE"
echo "------------------------------------------" >> "$LOG_FILE"

sudo ./syn_scanner -s "$MY_IP" -t "$TARGET" -p 1 -e "$PORT_LIMIT" | tee -a "$LOG_FILE"

echo "------------------------------------------" >> "$LOG_FILE"
echo "Scan saved to: $LOG_FILE"
