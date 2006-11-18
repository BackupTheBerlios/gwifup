/*
 * wifup.c
 *
 * Bring up wireless interfaces using ifup
 *
 * Author: Dominik Strecker <dstrecker@web.de>
 *
 * Date: 01 Nov 2006
 *
 */

#include <string.h>
#include <stdio.h>

#define CONFIG_DIR    "config"
#define IFACE_NAME    "iface"
#define IFACE_DELIM   " "
#define IFDOWN        "ifdown"
#define IFUP          "ifup"
#define IFUP_SWITCH   "-i"
#define IFDOWN_SWITCH "-i"
#define LINE_LENGTH   1024

int main(int argc, char **argv) {

  char line[LINE_LENGTH];
  char cmdline[LINE_LENGTH];
  char *iface;
  FILE *file;

  // get interface name from config file
  sprintf(line, "%s/%s", CONFIG_DIR, argv[1]);
  file = fopen(line, "r");
  for(;;) {
    fgets(line, 255, file);
    iface = strstr(line, IFACE_NAME);
    if(iface != NULL) {
      iface = strtok(iface, IFACE_DELIM);
      iface = strtok(NULL, IFACE_DELIM);
      break;
    }
  }
  fclose(file);

  // bring interface down
  sprintf(cmdline, "%s %s %s %s/%s", IFDOWN, iface, IFDOWN_SWITCH, CONFIG_DIR, argv[1]);
  system(cmdline);

  // bring interface up using config file
  sprintf(cmdline, "%s %s %s %s/%s", IFUP, iface, IFUP_SWITCH, CONFIG_DIR, argv[1]);
  system(cmdline);

  return 0;
}
