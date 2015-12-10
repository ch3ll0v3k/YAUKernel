#include <sys/pic.h>
#include <driver/video.h>

#define outb(port,value) \
    asm volatile ("outb %%al, %%dx" :: "d" (port), "a" (value));

void init_pic()
{
    video_print("Init PIC ... ");

    /* Initialisation de ICW1 */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /* Initialisation de ICW2 */
    outb(0x21, 0x20);   /* vecteur de depart = 32 */
    outb(0xA1, 0x70);   /* vecteur de depart = 96 */

    /* Initialisation de ICW3 */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    /* Initialisation de ICW4 */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /* masquage des interruptions */
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    video_print_ok();
}
