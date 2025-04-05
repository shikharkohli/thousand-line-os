#include "kernel.h"

extern char __bss[], __bss_end[], __stack_top[];

// Declare putchar before it's used
void putchar(char ch);

// Helper function to print a string
void print_string(const char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        putchar(s[i]);
    }
}

// Helper function to print a number in hexadecimal
void print_hex(unsigned long n) {
    char hex_digits[] = "0123456789abcdef";
    char buffer[17]; // 16 hex digits + null terminator
    buffer[16] = '\0';
    for (int i = 15; i >= 0; i--) {
        buffer[i] = hex_digits[n & 0xF];
        n >>= 4;
    }
    print_string("0x");
    print_string(buffer);
}

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid) {
    print_string("sbi_call: fid = ");
    print_hex(fid);
    print_string(", eid = ");
    print_hex(eid);
    print_string(", args = ");
    print_hex(arg0);
    print_string(", ");
    print_hex(arg1);
    print_string("\n");

    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");

    print_string("sbi_call: return error = ");
    print_hex(a0);
    print_string(", value = ");
    print_hex(a1);
    print_string("\n");

    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) {
    // Assuming SBI call ID 1 is for console output
    struct sbiret ret = sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

void clear_bss(void) {
    for (char *b = __bss; b < __bss_end; b++) {
        *b = 0;
    }
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "la sp, __stack_top\n"  // Load the address of __stack_top into sp
        "j kernel_main\n"    // Call kernel_main
        :
        : 
        : "sp", "ra"            // Indicate sp and ra are clobbered
    );
}

void kernel_main(void) {
    print_string("kernel_main: start\n"); // Add this line
    clear_bss();
    const char *s = "\n\nHello World!\n";
    for (int i = 0; s[i] != '\0'; i++) {
        putchar(s[i]);
    }

    for (;;) {
        __asm__ __volatile__("wfi");
    }
}