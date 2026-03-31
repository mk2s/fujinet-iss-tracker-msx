#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <video/tms99x8.h>
#include "fujinet-network.h"
#include "map.h"

// MSX tick counter.
__at (0xFC9E) unsigned int tikcnt;

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

const unsigned char sprite[8] =
  {
    0x20,0x50,0xA4,0x58,0x1A,0x05,0x0A,0x04
  };
const unsigned char xpos[360] = 
  { 0,0,1,2,2,3,4,4,5,6,7,7,8,9,9,10,11,12,12,13,14,14,15,16,17,17,18,19,19,20,21,22,22,23,24,24,25,26,27,27,28,29,29,30,31,32,32,33,34,34,35,36,36,37,38,39,39,40,41,41,42,43,44,44,45,46,46,47,48,49,49,50,51,51,52,53,54,54,55,56,56,57,58,59,59,60,61,61,62,63,64,64,65,66,66,67,68,68,69,70,71,71,72,73,73,74,75,76,76,77,78,78,79,80,81,81,82,83,83,84,85,86,86,87,88,88,89,90,91,91,92,93,93,94,95,96,96,97,98,98,99,100,100,101,102,103,103,104,105,105,106,107,108,108,109,110,110,111,112,113,113,114,115,115,116,117,118,118,119,120,120,121,122,123,123,124,125,125,126,127,128,128,129,130,130,131,132,132,133,134,135,135,136,137,137,138,139,140,140,141,142,142,143,144,145,145,146,147,147,148,149,150,150,151,152,152,153,154,155,155,156,157,157,158,159,160,160,161,162,162,163,164,164,165,166,167,167,168,169,169,170,171,172,172,173,174,174,175,176,177,177,178,179,179,180,181,182,182,183,184,184,185,186,187,187,188,189,189,190,191,192,192,193,194,194,195,196,196,197,198,199,199,200,201,201,202,203,204,204,205,206,206,207,208,209,209,210,211,211,212,213,214,214,215,216,216,217,218,219,219,220,221,221,222,223,224,224,225,226,226,227,228,228,229,230,231,231,232,233,233,234,235,236,236,237,238,238,239,240,241,241,242,243,243,244,245,246,246,247,248,248,249,250,251,251,252,253,253,254,255 };

const unsigned char ypos[360] = 
  { 160,159,159,158,158,157,157,156,156,156,155,155,154,154,153,153,152,152,152,151,151,150,150,149,149,148,148,148,147,147,146,146,145,145,144,144,144,143,143,142,142,141,141,140,140,140,139,139,138,138,137,137,136,136,136,135,135,134,134,133,133,132,132,132,131,131,130,130,129,129,128,128,128,127,127,126,126,125,125,124,124,124,123,123,122,122,121,121,120,120,120,119,119,118,118,117,117,116,116,116,115,115,114,114,113,113,112,112,112,111,111,110,110,109,109,108,108,108,107,107,106,106,105,105,104,104,104,103,103,102,102,101,101,100,100,100,99,99,98,98,97,97,96,96,96,95,95,94,94,93,93,92,92,92,91,91,90,90,89,89,88,88,88,87,87,86,86,85,85,84,84,84,83,83,82,82,81,81,80,80,80,79,79,78,78,77,77,76,76,76,75,75,74,74,73,73,72,72,72,71,71,70,70,69,69,68,68,68,67,67,66,66,65,65,64,64,64,63,63,62,62,61,61,60,60,60,59,59,58,58,57,57,56,56,56,55,55,54,54,53,53,52,52,52,51,51,50,50,49,49,48,48,48,47,47,46,46,45,45,44,44,44,43,43,42,42,41,41,40,40,40,39,39,38,38,37,37,36,36,36,35,35,34,34,33,33,32,32,32,31,31,30,30,29,29,28,28,28,27,27,26,26,25,25,24,24,24,23,23,22,22,21,21,20,20,20,19,19,18,18,17,17,16,16,16,15,15,14,14,13,13,12,12,12,11,11,10,10,9,9,8,8,8,7,7,6,6,5,5,4,4,4,3,3,2,2,1,1,0,0 };

unsigned char fetch_latitude_to_y(char *latitude)
{
  int lat = atoi(latitude)+180;

  return ypos[lat]+4;
}

unsigned char fetch_longitude_to_x(char *longitude)
{
  int lon = atoi(longitude)+180;

  return xpos[lon]-4;
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

  vdp_set_sprite_mode(sprite_scaled);
  vdp_set_sprite_8(0,sprite);

  while(1){
    gotoxy(0,21); 
    cprintf("                           ");
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

      //parse_json("/timestamp");

      err = network_close("N:HTTP://api.open-notify.org/iss-now.json"); // FIXME: Detect and deal with errors (moot?)

      gotoxy(0,21);
      cprintf("lat:%s lon:%s", lat_str, lon_str);
      vdp_put_sprite_8(0,fetch_longitude_to_x(lon_str), fetch_latitude_to_y(lat_str),0, VDP_INK_DARK_RED);

    }
    tikcnt = 0;
    while(tikcnt<4096);
    /* 4096 / 60 = 68.27 seconds*/
  }
}