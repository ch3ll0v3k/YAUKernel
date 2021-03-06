#include <boot/multiboot.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/asm.h>
#include <sys/syscall.h>
#include <sys/pagemem.h>
#include <sys/memory.h>
#include <sys/task.h>
#include <sys/kmalloc.h>
#include <driver/pic.h>
#include <driver/video.h>
#include <driver/ide.h>
#include <libc/string.h>

void task1(void);
void task2(void);
void kmain_continue(struct multiboot_info *mbi);
void kmain(unsigned long magic, struct multiboot_info *mbi);

void task1()
{
    int i;
    char *msg = (char*) 0x40000100;
    msg[0] = 't';
    msg[1] = 'a';
    msg[2] = 's';
    msg[3] = 'k';
    msg[4] = '1';
    msg[5] = '\n';

    while(1)
    {
        asm volatile("mov %%ecx, 6     ;"
                 "mov %%ebx, %0     ;"
                 "mov %%eax, 0x01   ;"
                 "int 0x30          ;"
            :
            : "m" (msg)
            :
        );
        for(i=0; i<10000000; i++);
    }
}

void task2()
{
    int i;
    char *msg = (char*) 0x40000100;
    msg[0] = 't';
    msg[1] = 'a';
    msg[2] = 's';
    msg[3] = 'k';
    msg[4] = '2';
    msg[5] = '\n';

    while(1)
    {
        asm volatile("mov %%ecx, 6      ;"
                 "mov %%ebx, %0     ;"
                 "mov %%eax, 0x01   ;"
                 "int 0x30          ;"
            :
            : "m" (msg)
            :
        );
        for(i=0; i<10000000; i++);
    }
}

void kmain_continue(struct multiboot_info *mbi)
{
    char *buf;
    char const *msg = "Bonjourrrrrrr\n";
    idt_init();
    pic_init();

    memory_init(mbi);
    pagemem_init();
    kmalloc_init();

    syscall_init();

    task_load_and_schedule(&task1, 0x2000);
    task_load_and_schedule(&task2, 0x4000);

    // disk test
    buf = kmalloc(512);
    memcpy(buf, msg, strlen((char *)msg) + 1);
    ide_write(0, 2, 1, buf);

    video_print_color("Enable interrupts !\n", COLOR(WHITE, GREEN));
    sti(); // enable interrupts

    while(1);
}

void kmain(unsigned long magic, struct multiboot_info *mbi)
{
    video_screen_clean();
    video_print("YAUK !\n");

    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        video_printf("Bad magic - unable to boot :( (0x%x)\n", magic);
        asm volatile("hlt");
    }

    multiboot_display(mbi);
    
    gdt_init();
    set_ss_esp(SEGMENT_KERNEL_STACK);

    kmain_continue(mbi);
}
