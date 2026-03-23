# ARM_Based_Linux_Character_Device_Driver
ARM Based Linux Character Device Driver to Multiply two numbers and return result. Translates user application to FPGA hardware peripherals. 

# Zynq SoC: Hardware-Software Co-Design & Linux Driver Development

This repository documents the end-to-end development of a custom hardware-accelerated system on a **Xilinx ZYBO Z7-10 (Zynq-7000 SoC)**. The project involves designing custom FPGA logic and building the Linux kernel infrastructure required to interface with it.

## 🛠 Project Overview
The primary goal of this project was to offload integer multiplication to a custom-designed Verilog peripheral and create a high-performance communication path between the **ARM Cortex-A9** processor and the FPGA fabric.

## 📂 Core Files
* **`multiplier.c`**: A Linux Character Device Driver that implements the `file_operations` structure . It manages memory mapping (`ioremap`) and provides a `/dev/multiplier` interface for user applications.
* **`multiply.c`**: A simpler Linux Kernel Module used for initial hardware validation and testing AXI-Lite register access.
* **`devtest.c`**: A user-space C application that opens the device driver, writes operands to the hardware, and reads back the result to verify accuracy .

## ⚙️ Technical Implementation

### 1. Memory-Mapped I/O (MMIO)
The Linux kernel operates in virtual memory. To communicate with the hardware's physical AXI address (`0x41200000`), the driver uses `ioremap` to create a virtual mapping that the kernel can safely access .

### 2. Character Device Driver Architecture
The driver acts as a bridge between User Space and Kernel Space:
* **`device_write`**: Uses `get_user` to safely pull operands from the user application and sends them to the hardware via `iowrite32` .
* **`device_read`**: Uses `ioread32` to grab the hardware's product and `put_user` to return it to the application ].
* **Device Registration**: The driver is registered with a dynamic Major Number to identify it within the Linux VFS (Virtual File System).

### 3. Hardware-Software Trade-offs
Performance analysis across the project showed that:
* ]**Bare-Metal (Direct Access)**: Offers the lowest latency and fastest "wall-clock" time because there is no OS overhead .
* **Linux Driver (Abstraction)**: While slightly slower due to system calls and memory translation, it provides a secure, multi-user environment where hardware can be shared and managed by the OS.

## 🚀 How to Run
1. **Compile the driver**: Use the PetaLinux cross-compilation toolchain.
2. **Load the module**: `insmod multiplier.ko`.
3. **Create the device node**: Check `dmesg` for the Major Number and run `mknod /dev/multiplier c [Major] 0` .
4. **Run the test**: `./devtest`.

---
*This work was completed for ECEN 449: Microprocessor Systems Design at Texas A&M University.*
