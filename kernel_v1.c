typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];


//Sets a value in the memery to a defined character by taking the buffer address and memory size
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *)(buf);
    while (n--) {
        *p++ = c;
    }
    return buf;
}

void kernel_main(void) {
    memset(__bss, 0, (size_t)(__bss_end - __bss)); //initialize the bss section to all zeroes

    for(;;); // initialize the loop to keep kernel running
}

//place the boot function in the .text.boot section so the linker finds it
__attribute__((section(".text.boot"))) 
//don't generate unnecessary code before 
//and after the boot function to make sure inline assembly code is kept as is
__attribute__((naked)) 

void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // set the stack pointer to the top of the stack
        "j kernel_main\n" // jump to the kernel main function
        :
        : [stack_top] "r" (__stack_top) // pass the stack top to the [stack-top] operand
        //keep in mind that stack addresses grow towards zero as more data is pushed onto the stack, 
        //therefore we decrement the address when pushing onto the stack
    );
}