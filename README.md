# **Advanced-SYN-Scanner: Low-Level TCP Suite**

This repository features a high-performance network reconnaissance engine developed to bypass standard OS network stacks. It focuses on raw packet injection and real-time TCP state analysis for stealthy service discovery.

---

### **Project Reflection & Technical Q&A**

### **1. Why did I write the code this way? (The "Low-Level" Logic)**
* **Mastering the Stack**: My objective was to master the TCP/IP stack by eliminating reliance on high-level libraries.
* **Stealth Operations**: I implemented a custom SYN scanning engine that performs "half-open" scans, significantly reducing the log footprint on target systems.
* **Direct Control**: By utilizing **Raw Sockets** in C, I gained 100% control over the IP and TCP headers, ensuring every byte is manually defined.

### **2. What challenges did I face?**
* **Manual Checksum Calculation**: Since I bypassed the OS stack, the kernel no longer calculated the checksums; I had to implement a 1s complement sum algorithm manually.
* **Packet Filtering**: I developed filtering logic to ensure the raw socket only processes incoming packets relevant to the specific scan session.
* **Timeout Management**: I integrated `SO_RCVTIMEO` socket options to manage microsecond-level timeouts for robust port probing.

---

### **Installation & Usage**

### **1. Compilation**
The core engine is written in C and must be compiled using GCC.
```bash
gcc syn_scanner.c -o syn_scanner
```

### **2. Running the Scanner (Manual)**

* Since the tool uses Raw Sockets, it requires root privileges.
```bash
sudo ./syn_scanner -s <Your_Source_IP> -t <Target_IP> -p <Start_Port> -e <End_Port>

-s: Your local interface IP (used for packet headers).

-t: Destination target IP.

-p: Starting port number.

-e: Ending port number.
```
### **3. Automated Operations (Recommended)**

*For faster deployment and automated logging, use the provided Bash wrappers:
```bash
    Auto Scan: Automatically detects local IP and initiates the scan.
    Bash

    chmod +x auto_scan.sh
    ./auto_scan.sh <Target_IP> <Max_Port>

    Session Logging: Captures scan output and saves it to a timestamped file.
    Bash

    chmod +x scan_logger.sh
    ./scan_logger.sh <Target_IP>
```
### **Technical Specifications**
```bash
    Language: C (Core Engine), Bash (Automation & Logging).

    Networking: Raw Sockets (AF_INET, SOCK_RAW), TCP/IP, ICMP.

    Architecture: Modular design with a dedicated C-sender/receiver and shell-based reporting wrappers.
```
### **License**

This project is licensed under the MIT License - see the LICENSE file for details.

Author: Arda Meçik

Student ID: 1241602620
