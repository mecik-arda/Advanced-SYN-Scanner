#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: ./auto_scan.sh <Target_IP> <Port_Range_End>"
    exit 1
fi

TARGET="$1"
PORT_END="$2"

# Automatically extract the local IP address of the active network interface
SOURCE_IP=$(hostname -I | awk '{print $1}')

echo "[*] Your Local IP detected as: $SOURCE_IP"
echo "[*] Starting Hades Syn Scanner on $TARGET..."

# Execute the C-based scanner and display results on terminal while saving to a report file
sudo ./syn_scanner -s "$SOURCE_IP" -t "$TARGET" -p 1 -e "$PORT_END" | tee scan_report.txt

echo "[+] Scan finished. Results saved to scan_report.txt"
