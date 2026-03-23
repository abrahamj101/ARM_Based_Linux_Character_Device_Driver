#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "xparameters.h"

// Constants: Device specific details
#define DEVICE_NAME "multiplier"
#define BUFFER_SIZE 80

// Function prototypes for device operations
static int device_init(void);
static void device_exit(void);
static int device_open(struct inode *, struct file *);
static int device_close(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

// Major number assigned to our device driver
static int Major;

// Memory Mapping Sizes
#define PHYSICAL_ADDRESS XPAR_MULTIPLY2_0_S00_AXI_BASEADDR
#define MEMORY_SIZE XPAR_MULTIPLY2_0_S00_AXI_HIGHADDR - XPAR_MULTIPLY2_0_S00_AXI_BASEADDR + 1

// Virtual address pointer for mapped memory
static void *mapped_address;

// Structure that defines the operations available to the device
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_close
};

// Initialize the module - Register the character device
static int __init device_init(void) {
    printk(KERN_INFO "Initializing the Character Device.\n");
    // Mapping the physical address to virtual address
    mapped_address = ioremap(PHYSICAL_ADDRESS, MEMORY_SIZE);
    printk(KERN_INFO "Physical Address: %x -> Virtual Address: %p\n", PHYSICAL_ADDRESS, mapped_address);

    // Register the device
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Failed to register the character device: %d\n", Major);
        return Major;
    }

    printk(KERN_INFO "Device registered: %d\n", Major);
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, Major);
    return 0;
}

// Clean-up function: Unregister and unmap the device
static void __exit device_exit(void) {
    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_INFO "Character device unregistered.\n");

    if (mapped_address) {
        iounmap(mapped_address);
        printk(KERN_INFO "Unmapped virtual address.\n");
    }
}

// Handle the opening of the device
static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened.\n");
    return 0;
}

// Handle the closing of the device
static int device_close(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed.\n");
    return 0;
}

// Handle reading from the device
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
    printk(KERN_INFO "Reading from device.\n");
    int bytes_read = 0;
    int *kernel_buffer = (int *)kmalloc(length * sizeof(int), GFP_KERNEL);

    // Directly read data from hardware into kernel buffer
    kernel_buffer[0] = ioread32(mapped_address);
    kernel_buffer[1] = ioread32(mapped_address + 4);
    kernel_buffer[2] = ioread32(mapped_address + 8);

    // Transfer data to user space
    for (int i = 0; i < length; i++) {
        put_user(((char *)kernel_buffer)[i], &buffer[i]);
        bytes_read++;
    }

    kfree(kernel_buffer);
    return bytes_read;
}

// Handle writing to the device
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *off) {
    printk(KERN_INFO "Writing to device.\n");
    char *kernel_buffer = kmalloc(len + 1, GFP_KERNEL);

    // Copy data from user space
    for (int i = 0; i < len; i++) {
        get_user(kernel_buffer[i], &buff[i]);
    }
    kernel_buffer[len] = '\0'; // Null-terminate the string

    // Writing data to hardware using mapped virtual address
     iowrite32(*(int *)kernel_buffer, mapped_address);
     iowrite32(*((int *)kernel_buffer + 1), mapped_address + 4);

    kfree(kernel_buffer);
    return len; // Return the number of bytes written
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ECEN 449 Student");
MODULE_DESCRIPTION("Character Device Driver for Multiplication Device");

module_init(device_init);
module_exit(device_exit);