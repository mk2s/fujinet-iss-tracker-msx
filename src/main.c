#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <video/tms99x8.h>
#include "fujinet-network.h"
#include "map.h"

char txt[80], url[128];
#define CH_EOL 13

/* Open and parse JSON from an URL
 * @param url
 * @return 0 on success, else fujinet-network error code (See FN_ERR_* values)
 * copied from https://github.com/FujiNetWIFI/fujinet-iss-tracker/blob/main/atari/src/atari/iss.c#L283
 */
uint8_t open_json(char * url) {
  uint8_t err;

  err = network_open(url, OPEN_MODE_HTTP_GET, OPEN_TRANS_NONE);

  if (err)
    return err; // ERROR! Return it & bail

  err = network_json_parse(url);

  return err; // SUCCESS or ERROR
}

char json_part[256];
char query[256];
char tmp[256];

/* Parse an element of json
 * @param element
 * @return 0 on success, or negative value (see FN_ERR_*) on error
 */
int8_t parse_json(char * element) {
  int16_t stat;

  json_part[0] = '\0';

  sprintf(query, "%s%c", element, CH_EOL);
  stat = network_json_query(url, query, json_part);

  if (stat < 0)
    return stat; // ERROR, return it!

  if (strlen(json_part) == 0) {
    return 0; // FIXME: An error value, instead?
  }

  return 0; // SUCCESS, apparently!
}

#define MSG_CENTER 99

void message(int x, int y, char * txt) {
  int i;
  unsigned char ch;
  unsigned int ybase;

  if (x == MSG_CENTER) {
    x = (20 - (strlen(txt) >> 1));
  }

  //ybase = (unsigned int) (txt_mem + (y * 40));

  for (i = 0; txt[i] != '\0'; i++) {
    ch = txt[i];

    if (ch < 32) {
      ch = ch + 64;
    } else if (ch < 96) {
      ch = ch - 32;
    }

    //POKE(ybase + x + i, ch);
  }
}

void main(void)
{
  uint8_t err;
  char lat_str[16];
  char lon_str[16];

    vdp_set_mode(2);
    vdp_color(VDP_INK_LIGHT_GREEN,VDP_INK_DARK_BLUE,VDP_INK_DARK_BLUE);
    vdp_vwrite(src_ISS_TIAP,0x0000,src_ISS_TIAP_len);
    vdp_vwrite(src_ISS_TIAC,0x2000,src_ISS_TIAC_len);

    gotoxy(0,20); cprintf("%24s","IIS Tracker yay!");
    err = open_json("N:HTTP://api.open-notify.org/iss-now.json");
    if (!err) {
      /* SUCCESS */
/* Parse and display position */
      parse_json("/iss_position/latitude");
      strncpy(lat_str, json_part, sizeof(lat_str) - 1);
      lat_str[sizeof(lat_str) - 1] = '\0';

      parse_json("/iss_position/longitude");
      strncpy(lon_str, json_part, sizeof(lon_str) - 1);
      lon_str[sizeof(lon_str) - 1] = '\0';

      parse_json("/timestamp");

      err = network_close("N:HTTP://api.open-notify.org/iss-now.json"); // FIXME: Detect and deal with errors (moot?)

      gotoxy(0,21); cprintf("lat:%s lon:%s", lat_str, lon_str);

      
    }

    while(1);
}