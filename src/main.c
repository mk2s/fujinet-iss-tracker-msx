#include <stdint.h>
#include <conio.h>
#include <video/tms99x8.h>
#include "map.h"

void main(void)
{
    vdp_set_mode(2);
    vdp_color(VDP_INK_LIGHT_GREEN,VDP_INK_DARK_BLUE,VDP_INK_DARK_BLUE);
    vdp_vwrite(src_ISS_TIAP,0x0000,src_ISS_TIAP_len);
    vdp_vwrite(src_ISS_TIAC,0x2000,src_ISS_TIAC_len);

    gotoxy(0,20); cprintf("%24s","IIS Tracker yay!");

    while(1);
}