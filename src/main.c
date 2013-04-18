#include "multiboot.h"
#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "paging.h"
#include "fs.h"
extern u32int placement_address;

void kmain(struct multiboot *mboot_ptr, unsigned int magic)
{
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		PANIC("magic != MULTIBOOT_BOOTLOADER_MAGIC");
		__asm__ __volatile("cli; hlt;");
	}
    init_descriptor_tables();

	monitor_clear();

	ASSERT(mboot_ptr->mods_count > 0);
	u32int initrd_location = *((u32int*)mboot_ptr->mods_addr);
	u32int initrd_end = *(u32int*)(mboot_ptr->mods_addr+4);
	placement_address = initrd_end;

	monitor_write("IDT Initialized!\n");
	u32int a = kmalloc(8);
	initialise_paging();
	monitor_write("Hello, paging world!\n");

	u32int b = kmalloc(8);
	u32int c = kmalloc(8);
	monitor_write("a: ");
	monitor_write_hex(a);
	monitor_write(", b: ");
	monitor_write_hex(b);
	monitor_write("\nc: ");
	monitor_write_hex(c);

	kfree(c);
	kfree(b);
	u32int d = kmalloc(12);
	monitor_write(", d: ");
	monitor_write_hex(d);

	fs_root = initialise_initrd(initrd_location);

	int i = 0;
	struct dirent *node = 0;
	while ( (node = readdir_fs(fs_root, i)) != 0)
	{
		monitor_write("Found file ");
		monitor_write(node->name);
		fs_node_t *fsnode = finddir_fs(fs_root, node->name);

		if ((fsnode->flags&0x7) == FS_DIRECTORY)
			monitor_write("\n\t(directory)\n");
		else
		{
			monitor_write("\n\t contents: \"");
			char buf[256];
			u32int sz = read_fs(fsnode, 0, 256, buf);
			int j;
			for (j = 0; j < sz; j++)
				monitor_put(buf[j]);

			monitor_write("\"\n");
		}
		i++;
	}
	
	while (1) __asm__ __volatile("hlt");
}
