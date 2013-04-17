#include <multiboot.h>
#include "monitor.h"
void kmain(multiboot_header *multiboot, unsigned int magic)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        /* Kernel was not loaded with a multiboot compliant bootloader, halt */
        __asm__ __volatile("cli; hlt;");
    }
    monitor_clear();
    monitor_write("Hello World");
    while (1) __asm__ __volatile("hlt");
}
