# **Hades-Scanner: Low-Level TCP SYN Suite**

This repository features a high-performance network reconnaissance engine developed to bypass standard OS network stacks. It focuses on raw packet injection and real-time TCP state analysis for stealthy service discovery.

---

### **Project Reflection & Technical Q&A**

### **1. Why did I write the code this way? (The "Low-Level" Logic)**
* **Mastering the Stack**: My objective was to master the TCP/IP stack by eliminating reliance on high-level libraries.
* **Stealth Operations**: I accomplished a custom SYN scanning engine as measured by its ability to perform "half-open" scans, which significantly reduces the log footprint on target systems compared to full connect scans.
* **Direct Control**: By utilizing **Raw Sockets** in C, I gained 100% control over the IP and TCP headers, ensuring that every byte sent is exactly as defined in the source code.

### **2. What challenges did I face?**
* **Manual Checksum Calculation**: A major hurdle was the TCP Checksum. Since I bypassed the OS stack, the kernel no longer calculated the checksum for me.
* **Algorithmic Implementation**: I had to implement a 1s complement sum algorithm and a pseudo-header structure to satisfy the receiving host's verification logic.
* **Packet Filtering**: During multi-port scans, the raw socket catches *all* incoming TCP traffic. I had to implement a filtering logic that verifies the source IP and source port of the incoming packet.
* **Handling Timeouts**: To prevent the scanner from hanging on filtered ports, I integrated `SO_RCVTIMEO` socket options to manage microsecond-level timeouts for each port probe.

### **3. How did I manage the Network Logic?**
* **The Three-Way Handshake Manipulation**: The scripts send a `SYN` packet and listen for a `SYN-ACK`.
* **State Control**: Once a service is confirmed, the scanner immediately sends a `RST` (Reset) packet instead of an `ACK`, effectively tearing down the connection before it is fully established.
* **ICMP Error Interpretation**: Beyond TCP, the project integrates logic to interpret ICMP `Type 3, Code 3` messages, allowing the tool to distinguish between a "Closed Port" and a "Filtered/Firewalled Port" based on the Wireshark analysis sessions conducted during the lab.

---

### **Technical Specifications**
* **Language**: C (Core Engine), Bash (Automation & Logging).
* **Networking**: Raw Sockets (AF_INET, SOCK_RAW), TCP/IP, ICMP.
* **Architecture**: Modular design with a dedicated C-sender/receiver and shell-based reporting wrappers.

---

### **License**
This project is licensed under the MIT License - see the LICENSE file for details.

---
**Author:** Arda Meçik
**Student ID:** 1241602620