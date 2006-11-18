/*
 * gwifup.c
 *
 * Gtk-UI for wifup
 *
 * Author: Dominik Strecker <dstrecker@web.de>
 *
 * Date: 01 Nov 2006
 *
 */

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <gtk/gtk.h>
#include "eggtrayicon.h"

#define WIRELESS_PREFIX "wireless-"
#define IFACE_NAME      "iface"
#define IFACE_DELIM     " "
#define CONFIG_DIR      "config"
#define WIFUP           "./wifup"
#define LINE_LENGTH     1024

#define NUM_RATES       8
char *rate[NUM_RATES] = {
  "n/a",
  "auto",
  "fixed",
  "5.5M",
  "11M",
  "22M",
  "54M",
  "108M",
};

#define NUM_CHANNELS    17
char *channel[NUM_CHANNELS] = {
  "n/a",
  "auto",
  "off",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "10",
  "11",
  "12",
  "13",
  "14",
};

#define NUM_MODES       8
char *mode[NUM_MODES] = {
  "n/a",
  "auto",
  "Ad-Hoc",
  "Managed",
  "Master",
  "Repeater",
  "Secondary",
  "Monitor",
};

GtkWidget *window1;
GtkWidget *vbox17;
GtkWidget *hbox17;
GtkWidget *vbox1;
GtkWidget *frame1;
GtkWidget *alignment1;
GtkWidget *profile_box;
GtkWidget *label1;
GtkWidget *frame3;
GtkWidget *alignment3;
GtkWidget *table1;
GtkWidget *hbox2;
GtkWidget *label5;
GtkWidget *essid_entry;
GtkWidget *hbox3;
GtkWidget *label6;
GtkWidget *channel_box;
GtkWidget *hbox4;
GtkWidget *label7;
GtkWidget *mode_box;
GtkWidget *hbox5;
GtkWidget *label8;
GtkWidget *rate_box;
GtkWidget *label3;
GtkWidget *frame7;
GtkWidget *alignment7;
GtkWidget *vbox5;
GtkWidget *hbox18;
GtkWidget *label19;
GtkWidget *iface_entry;
GtkWidget *dhcp_cb;
GtkWidget *frame8;
GtkWidget *alignment8;
GtkWidget *vbox6;
GtkWidget *scrolledwindow2;
GtkWidget *static_text;
GtkWidget *label20;
GtkWidget *label18;
GtkWidget *vbox7;
GtkWidget *frame2;
GtkWidget *alignment2;
GtkWidget *vbox2;
GtkWidget *hbox6;
GtkWidget *label9;
GtkWidget *keymode_box;
GtkWidget *hbox10;
GtkWidget *frame5;
GtkWidget *alignment5;
GtkWidget *vbox3;
GtkWidget *hbox12;
GtkWidget *label11;
GtkWidget *key1_entry;
GtkWidget *key1_rb;
GSList *key1_rb_group = NULL;
GtkWidget *hbox13;
GtkWidget *label12;
GtkWidget *key2_entry;
GtkWidget *key2_rb;
GtkWidget *hbox14;
GtkWidget *label13;
GtkWidget *key3_entry;
GtkWidget *key3_rb;
GtkWidget *hbox15;
GtkWidget *label14;
GtkWidget *key4_entry;
GtkWidget *key4_rb;
GtkWidget *label10;
GtkWidget *label2;
GtkWidget *frame6;
GtkWidget *alignment6;
GtkWidget *scrolledwindow1;
GtkWidget *advanced_text;
GtkWidget *label15;
GtkWidget *hseparator1;
GtkWidget *hbuttonbox1;
GtkWidget *cancel_button;
GtkWidget *save_button;
GtkTooltips *tooltips;

// bring up popup menu
static void tray_popup(GtkWidget *widget, GdkEventButton *event, gpointer data) {
  if(event->type == GDK_BUTTON_PRESS) {
    if(event->button == 3) {
      gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL, event->button, event->time);
    }
  }
}

// hide config window
static gboolean config_hide(GtkWidget *widget, GdkEvent *event, gpointer data) {
  gtk_widget_hide(widget);
  return TRUE; 
}

static void load_profile(GtkComboBox *widget, GdkEventButton *event, gpointer data) {

  FILE *file;
  char line[LINE_LENGTH];
  char temp[LINE_LENGTH];
  char *token;
  char *value;
  int i;

  // clear text
  gtk_text_buffer_set_text(gtk_text_view_get_buffer((GtkTextView*)static_text), "", -1);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer((GtkTextView*)advanced_text), "", -1);
  gtk_entry_set_text((GtkEntry*)essid_entry, "");
  gtk_combo_box_set_active((GtkComboBox*)rate_box, 0);
  gtk_combo_box_set_active((GtkComboBox*)channel_box, 0);

  sprintf(line, "%s/%s", CONFIG_DIR, gtk_combo_box_get_active_text(widget));
  file = fopen(line, "r");
  printf("%s\n", line);

  // find line containing "iface"
  for(;;) {
    fgets(line, LINE_LENGTH, file);
    if(feof(file)) {
      // popup
      fclose(file);
      return;
    }
    token = strstr(line, IFACE_NAME);
    if(token != NULL) {
      token = strtok(token, IFACE_DELIM);
      token = strtok(NULL, IFACE_DELIM);
      gtk_entry_set_text((GtkEntry*)iface_entry, token);
      token = strtok(NULL, IFACE_DELIM);
      if(strcmp(token, "inet") != 0) {
	// popup
	fclose(file);
	return;
      }
      token = strtok(NULL, "\n");
      if(strcmp(token, "dhcp") == 0) {
	gtk_toggle_button_set_active((GtkToggleButton*)dhcp_cb, TRUE);
      } else {
	gtk_toggle_button_set_active((GtkToggleButton*)dhcp_cb, FALSE);
      }
      break;
    }
  }

  // lines following "iface"-line
  while(!feof(file)) {
    fgets(line, LINE_LENGTH, file);
    strncpy(temp, line, LINE_LENGTH);
    token = strstr(temp, WIRELESS_PREFIX);
    if(token != NULL) {
      token = strtok(token, "-");
      token = strtok(NULL, " ");
      value = strtok(NULL, "\n");
      if(strcmp(token, "essid") == 0) {
	gtk_entry_set_text((GtkEntry*)essid_entry, value);
      } else if(strcmp(token, "channel") == 0) {
	for(i = 0; i < NUM_CHANNELS; i++) {
	  if(strcmp(value, channel[i]) == 0) {
	    gtk_combo_box_set_active((GtkComboBox*)channel_box, i);
	  }
	}
      } else if(strcmp(token, "mode") == 0) {
	for(i = 0; i < NUM_MODES; i++) {
	  if(strcmp(value, mode[i]) == 0) {
	    gtk_combo_box_set_active((GtkComboBox*)mode_box, i);
	  }
	}
      } else if(strcmp(token, "rate") == 0) {
	for(i = 0; i < NUM_RATES; i++) {
	  if(strcmp(value, rate[i]) == 0) {
	    gtk_combo_box_set_active((GtkComboBox*)rate_box, i);
	  }
	}
      } else if(strcmp(token, "key") == 0) {
	// TODO
      } else {
	gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer((GtkTextView*)advanced_text), line, strlen(line));
      }
    } else {
      gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer((GtkTextView*)static_text), line, strlen(line));
    }
  }
  fclose(file);
}

static void save_profile(GtkComboBox *widget, GdkEventButton *event, gpointer data) {

  FILE *file;
  char line[LINE_LENGTH];
  GtkTextIter *start;
  GtkTextIter *end;

  sprintf(line, "%s/_%s", CONFIG_DIR, gtk_combo_box_get_active_text(widget));
  file = fopen(line, "w");
  if(gtk_toggle_button_get_active((GtkToggleButton*)dhcp_cb)) {
    sprintf(line, "iface %s inet dhcp\n", gtk_entry_get_text((GtkEntry*)iface_entry));
    fputs(line, file);
  } else {
    sprintf(line, "iface %s inet static\n", gtk_entry_get_text((GtkEntry*)iface_entry));
    fputs(line, file);
    gtk_text_buffer_get_start_iter(gtk_text_view_get_buffer((GtkTextView*)static_text), start);
    gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer((GtkTextView*)static_text), end);
    fputs(gtk_text_buffer_get_text(gtk_text_view_get_buffer((GtkTextView*)static_text), start, end, FALSE), file);
  }
  if(strcmp(gtk_entry_get_text((GtkEntry*)essid_entry), "") != 0) {
    sprintf(line, "wireless-essid %s\n", gtk_entry_get_text((GtkEntry*)essid_entry));
    fputs(line, file);
  }
  if(gtk_combo_box_get_active((GtkComboBox*)channel_box) != 0) {
    sprintf(line, "wireless-channel %s\n", channel[gtk_combo_box_get_active((GtkComboBox*)channel_box)]);
    fputs(line, file);
  }
  if(gtk_combo_box_get_active((GtkComboBox*)mode_box) != 0) {
    sprintf(line, "wireless-mode %s\n", mode[gtk_combo_box_get_active((GtkComboBox*)mode_box)]);
    fputs(line, file);
  }
  if(gtk_combo_box_get_active((GtkComboBox*)rate_box) != 0) {
    sprintf(line, "wireless-rate %s\n", rate[gtk_combo_box_get_active((GtkComboBox*)rate_box)]);
    fputs(line, file);
  }
  fclose(file);
}

// show config window
static void config_show(GtkWidget *widget, GdkEventButton *event, gpointer data) {

  FILE *file;
  char line[LINE_LENGTH];
  char *iface;
  struct dirent *dent;

  // clear combo box TODO (too complicated)
  while(gtk_combo_box_get_active_text((GtkComboBox*)profile_box) != NULL) {
    gtk_combo_box_remove_text((GtkComboBox*)profile_box, 0);
    gtk_combo_box_set_active((GtkComboBox*)profile_box, 0);
  }
  
  DIR *dir = opendir(CONFIG_DIR);
  dent = readdir(dir);
  while(dent) {
    if((strcmp(dent->d_name, ".") != 0) &&(strcmp(dent->d_name, "..") != 0)) {
      gtk_combo_box_append_text((GtkComboBox*)profile_box, dent->d_name);
    }
    dent = readdir(dir);
  }
  closedir(dir);
  gtk_combo_box_set_active((GtkComboBox*)profile_box, 0);
  gtk_widget_show(widget);
}

// quit program
static void gtk_quit(GtkWidget *widget, GdkEventButton *event, gpointer data) {
  if(event->type == GDK_BUTTON_PRESS) {
    if(event->button == 1) {
      gtk_main_quit();
    }
  }
}

// run ifup
static void run_ifup(GtkBin *widget, GdkEventButton *event, gpointer data) {

  char cmdline[LINE_LENGTH];

  if(event->type == GDK_BUTTON_PRESS) {
    if(event->button == 1) {
      sprintf(cmdline, "%s %s", WIFUP, gtk_label_get_text((GtkLabel*)(gtk_bin_get_child(widget))));
      printf(cmdline);
      system(cmdline);
    }
  }
}

int main(int argc, char **argv) {

  int i;

  gtk_set_locale();
  gtk_init(&argc, &argv);

  // config window
  tooltips = gtk_tooltips_new();

  window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(window1, 600, 400);
  gtk_window_set_title(GTK_WINDOW(window1), "Preferences - gwifup");
  gtk_window_set_position(GTK_WINDOW(window1), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(window1), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(window1), FALSE);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window1), TRUE);
  g_signal_connect(G_OBJECT(window1), "delete_event", G_CALLBACK(config_hide), NULL);

  vbox17 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox17);
  gtk_container_add(GTK_CONTAINER(window1), vbox17);

  hbox17 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox17);
  gtk_box_pack_start(GTK_BOX(vbox17), hbox17, TRUE, TRUE, 0);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(hbox17), vbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 16);

  frame1 = gtk_frame_new(NULL);
  gtk_widget_show(frame1);
  gtk_box_pack_start(GTK_BOX(vbox1), frame1, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame1), GTK_SHADOW_NONE);

  alignment1 = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(frame1), alignment1);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment1), 0, 0, 12, 0);

  profile_box = gtk_combo_box_new_text();
  gtk_widget_show(profile_box);
  gtk_container_add(GTK_CONTAINER(alignment1), profile_box);
  gtk_widget_set_size_request(profile_box, -1, 37);
  g_signal_connect(G_OBJECT(profile_box), "changed", G_CALLBACK(load_profile), NULL);

  label1 = gtk_label_new("<b>Profile</b>");
  gtk_widget_show(label1);
  gtk_frame_set_label_widget(GTK_FRAME(frame1), label1);
  gtk_label_set_use_markup(GTK_LABEL(label1), TRUE);

  frame3 = gtk_frame_new(NULL);
  gtk_widget_show(frame3);
  gtk_box_pack_start(GTK_BOX(vbox1), frame3, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame3), GTK_SHADOW_NONE);

  alignment3 = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment3);
  gtk_container_add(GTK_CONTAINER(frame3), alignment3);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment3), 0, 0, 12, 0);

  table1 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table1);
  gtk_container_add(GTK_CONTAINER(alignment3), table1);

  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox2);
  gtk_table_attach(GTK_TABLE(table1), hbox2, 0, 1, 0, 1,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);

  label5 = gtk_label_new("ESSID:");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(hbox2), label5, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label5), 1, 0);

  essid_entry = gtk_entry_new();
  gtk_widget_show(essid_entry);
  gtk_box_pack_start(GTK_BOX(hbox2), essid_entry, TRUE, TRUE, 0);
  gtk_widget_set_size_request(essid_entry, 118, -1);
  gtk_entry_set_invisible_char(GTK_ENTRY(essid_entry), 9679);

  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox3);
  gtk_table_attach(GTK_TABLE(table1), hbox3, 1, 2, 0, 1,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(GTK_FILL), 0, 0);

  label6 = gtk_label_new("Channel:");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(hbox3), label6, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label6), 1, 0);

  channel_box = gtk_combo_box_new_text();
  gtk_widget_show(channel_box);
  gtk_box_pack_start(GTK_BOX(hbox3), channel_box, TRUE, TRUE, 0);
  gtk_widget_set_size_request(channel_box, 78, -1);
  for(i = 0; i < NUM_CHANNELS; i++) {
    gtk_combo_box_append_text((GtkComboBox*)channel_box, channel[i]);
  }

  hbox4 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox4);
  gtk_table_attach(GTK_TABLE(table1), hbox4, 0, 1, 1, 2,
                   (GtkAttachOptions)(GTK_FILL),
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);

  label7 = gtk_label_new("Mode:");
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX(hbox4), label7, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label7), 3, 0);

  mode_box = gtk_combo_box_new_text();
  gtk_widget_show(mode_box);
  gtk_box_pack_start(GTK_BOX(hbox4), mode_box, TRUE, TRUE, 0);
  for(i = 0; i < NUM_MODES; i++) {
    gtk_combo_box_append_text((GtkComboBox*)mode_box, mode[i]);
  }

  hbox5 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox5);
  gtk_table_attach(GTK_TABLE(table1), hbox5, 1, 2, 1, 2,
                   (GtkAttachOptions)(GTK_FILL),
                   (GtkAttachOptions)(GTK_FILL), 0, 0);

  label8 = gtk_label_new("Rate:");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox5), label8, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label8), 13, 0);

  rate_box = gtk_combo_box_new_text();
  gtk_widget_show(rate_box);
  gtk_box_pack_start(GTK_BOX(hbox5), rate_box, TRUE, TRUE, 0);
  for(i = 0; i < NUM_RATES; i++) {
    gtk_combo_box_append_text((GtkComboBox*)rate_box, rate[i]);
  }

  label3 = gtk_label_new("<b>Network</b>");
  gtk_widget_show(label3);
  gtk_frame_set_label_widget(GTK_FRAME(frame3), label3);
  gtk_label_set_use_markup(GTK_LABEL(label3), TRUE);

  frame7 = gtk_frame_new(NULL);
  gtk_widget_show(frame7);
  gtk_box_pack_start(GTK_BOX(vbox1), frame7, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame7), GTK_SHADOW_NONE);

  alignment7 = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment7);
  gtk_container_add(GTK_CONTAINER(frame7), alignment7);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment7), 0, 0, 12, 0);

  vbox5 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox5);
  gtk_container_add(GTK_CONTAINER(alignment7), vbox5);

  hbox18 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox18);
  gtk_box_pack_start(GTK_BOX(vbox5), hbox18, TRUE, TRUE, 0);

  label19 = gtk_label_new("Name:");
  gtk_widget_show(label19);
  gtk_box_pack_start(GTK_BOX(hbox18), label19, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label19), 1, 0);

  iface_entry = gtk_entry_new();
  gtk_widget_show(iface_entry);
  gtk_box_pack_start(GTK_BOX(hbox18), iface_entry, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY(iface_entry), 9679);

  dhcp_cb = gtk_check_button_new_with_mnemonic("Use DHCP");
  gtk_widget_show(dhcp_cb);
  gtk_box_pack_start(GTK_BOX(vbox5), dhcp_cb, FALSE, FALSE, 0);

  frame8 = gtk_frame_new(NULL);
  gtk_widget_show(frame8);
  gtk_box_pack_start(GTK_BOX(vbox5), frame8, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame8), GTK_SHADOW_NONE);

  alignment8 = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment8);
  gtk_container_add(GTK_CONTAINER(frame8), alignment8);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment8), 0, 0, 12, 0);

  vbox6 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox6);
  gtk_container_add(GTK_CONTAINER(alignment8), vbox6);

  scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow2);
  gtk_box_pack_start(GTK_BOX(vbox6), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_SHADOW_IN);

  static_text = gtk_text_view_new();
  gtk_widget_show(static_text);
  gtk_container_add(GTK_CONTAINER(scrolledwindow2), static_text);

  label20 = gtk_label_new("<b>Static Configuration</b>");
  gtk_widget_show(label20);
  gtk_frame_set_label_widget(GTK_FRAME(frame8), label20);
  gtk_label_set_use_markup(GTK_LABEL(label20), TRUE);

  label18 = gtk_label_new("<b>Interface</b>");
  gtk_widget_show(label18);
  gtk_frame_set_label_widget(GTK_FRAME(frame7), label18);
  gtk_label_set_use_markup(GTK_LABEL(label18), TRUE);

  vbox7 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox7);
  gtk_box_pack_start(GTK_BOX(hbox17), vbox7, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox7), 16);

  frame2 = gtk_frame_new(NULL);
  gtk_widget_show(frame2);
  gtk_box_pack_start(GTK_BOX(vbox7), frame2, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_NONE);

  alignment2 = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(frame2), alignment2);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment2), 0, 0, 12, 0);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_container_add(GTK_CONTAINER(alignment2), vbox2);

  hbox6 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox6);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox6, TRUE, TRUE, 0);

  label9 = gtk_label_new("Keymode:");
  gtk_widget_show(label9);
  gtk_box_pack_start(GTK_BOX(hbox6), label9, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label9), 1, 0);

  keymode_box = gtk_combo_box_new_text();
  gtk_widget_show(keymode_box);
  gtk_box_pack_start(GTK_BOX(hbox6), keymode_box, TRUE, TRUE, 0);
  gtk_widget_set_size_request(keymode_box, -1, 27);

  hbox10 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox10);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox10, TRUE, TRUE, 0);

  frame5 = gtk_frame_new(NULL);
  gtk_widget_show(frame5);
  gtk_box_pack_start(GTK_BOX(hbox10), frame5, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame5), GTK_SHADOW_NONE);

  alignment5 = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment5);
  gtk_container_add(GTK_CONTAINER(frame5), alignment5);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment5), 0, 0, 12, 0);

  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox3);
  gtk_container_add(GTK_CONTAINER(alignment5), vbox3);

  hbox12 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox12);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox12, TRUE, TRUE, 0);

  label11 = gtk_label_new("Key 1:");
  gtk_widget_show(label11);
  gtk_box_pack_start(GTK_BOX(hbox12), label11, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label11), 1, 0);

  key1_entry = gtk_entry_new();
  gtk_widget_show(key1_entry);
  gtk_box_pack_start(GTK_BOX(hbox12), key1_entry, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY(key1_entry), 9679);

  key1_rb = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_widget_show(key1_rb);
  gtk_box_pack_start(GTK_BOX(hbox12), key1_rb, FALSE, FALSE, 0);
  gtk_tooltips_set_tip(tooltips, key1_rb, "Set as default key", NULL);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(key1_rb), key1_rb_group);
  key1_rb_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(key1_rb));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(key1_rb), TRUE);

  hbox13 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox13);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox13, TRUE, TRUE, 0);

  label12 = gtk_label_new("Key 2:");
  gtk_widget_show(label12);
  gtk_box_pack_start(GTK_BOX(hbox13), label12, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label12), 1, 0);

  key2_entry = gtk_entry_new();
  gtk_widget_show(key2_entry);
  gtk_box_pack_start(GTK_BOX(hbox13), key2_entry, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY(key2_entry), 9679);

  key2_rb = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_widget_show(key2_rb);
  gtk_box_pack_start(GTK_BOX(hbox13), key2_rb, FALSE, FALSE, 0);
  gtk_tooltips_set_tip(tooltips, key2_rb, "Set as default key", NULL);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(key2_rb), key1_rb_group);
  key1_rb_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(key2_rb));

  hbox14 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox14);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox14, TRUE, TRUE, 0);

  label13 = gtk_label_new("Key 3:");
  gtk_widget_show(label13);
  gtk_box_pack_start(GTK_BOX(hbox14), label13, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label13), 1, 0);

  key3_entry = gtk_entry_new();
  gtk_widget_show(key3_entry);
  gtk_box_pack_start(GTK_BOX(hbox14), key3_entry, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY(key3_entry), 9679);

  key3_rb = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_widget_show(key3_rb);
  gtk_box_pack_start(GTK_BOX(hbox14), key3_rb, FALSE, FALSE, 0);
  gtk_tooltips_set_tip(tooltips, key3_rb, "Set as default key", NULL);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(key3_rb), key1_rb_group);
  key1_rb_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(key3_rb));

  hbox15 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox15);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox15, TRUE, TRUE, 0);

  label14 = gtk_label_new("Key 4:");
  gtk_widget_show(label14);
  gtk_box_pack_start(GTK_BOX(hbox15), label14, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label14), 1, 0);

  key4_entry = gtk_entry_new();
  gtk_widget_show(key4_entry);
  gtk_box_pack_start(GTK_BOX(hbox15), key4_entry, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY(key4_entry), 9679);

  key4_rb = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_widget_show(key4_rb);
  gtk_box_pack_start(GTK_BOX(hbox15), key4_rb, FALSE, FALSE, 0);
  gtk_tooltips_set_tip(tooltips, key4_rb, "Set as default key", NULL);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(key4_rb), key1_rb_group);
  key1_rb_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(key4_rb));

  label10 = gtk_label_new("<b>Keys</b>");
  gtk_widget_show(label10);
  gtk_frame_set_label_widget(GTK_FRAME(frame5), label10);
  gtk_label_set_use_markup(GTK_LABEL(label10), TRUE);

  label2 = gtk_label_new("<b>Security</b>");
  gtk_widget_show(label2);
  gtk_frame_set_label_widget(GTK_FRAME(frame2), label2);
  gtk_label_set_use_markup(GTK_LABEL(label2), TRUE);

  frame6 = gtk_frame_new(NULL);
  gtk_widget_show(frame6);
  gtk_box_pack_start(GTK_BOX(vbox7), frame6, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame6), GTK_SHADOW_NONE);

  alignment6 = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment6);
  gtk_container_add(GTK_CONTAINER(frame6), alignment6);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment6), 0, 0, 12, 0);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_container_add(GTK_CONTAINER(alignment6), scrolledwindow1);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  advanced_text = gtk_text_view_new();
  gtk_widget_show(advanced_text);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), advanced_text);

  label15 = gtk_label_new("<b>Advanced</b>");
  gtk_widget_show(label15);
  gtk_frame_set_label_widget(GTK_FRAME(frame6), label15);
  gtk_label_set_use_markup(GTK_LABEL(label15), TRUE);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox17), hseparator1, TRUE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new();
  gtk_widget_show(hbuttonbox1);
  gtk_box_pack_start(GTK_BOX(vbox17), hbuttonbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbuttonbox1), 4);

  cancel_button = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancel_button);
  gtk_container_add(GTK_CONTAINER(hbuttonbox1), cancel_button);
  GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
  g_signal_connect_swapped(G_OBJECT(cancel_button), "button-release-event", G_CALLBACK(config_hide), window1);

  save_button = gtk_button_new_from_stock("gtk-save");
  gtk_widget_show(save_button);
  gtk_container_add(GTK_CONTAINER(hbuttonbox1), save_button);
  GTK_WIDGET_SET_FLAGS(save_button, GTK_CAN_DEFAULT);
  g_signal_connect_swapped(G_OBJECT(save_button), "button-release-event", G_CALLBACK(save_profile), profile_box);

  // popup menu
  GtkWidget *menu;
  menu = gtk_menu_new();

  GtkWidget *separator;
  separator = gtk_separator_menu_item_new();
  gtk_widget_show(separator);

  GtkWidget *quit_item;
  quit_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
  g_signal_connect_swapped(G_OBJECT(quit_item), "button-press-event", G_CALLBACK(gtk_quit), NULL);
  gtk_widget_show(quit_item);

  GtkWidget *config_item;
  config_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL);
  g_signal_connect_swapped(G_OBJECT(config_item), "button-press-event", G_CALLBACK(config_show), window1);
  gtk_widget_show(config_item);

  DIR *dir = opendir(CONFIG_DIR);
  struct dirent *dent;
  GtkWidget *item[5];
  for(i = 0; i < 5; i++) {
    dent = readdir(dir);
    if(!dent) {
      break;
    }
    if((strcmp(dent->d_name, ".") != 0) &&(strcmp(dent->d_name, "..") != 0)) {
      item[i] = gtk_menu_item_new_with_label(dent->d_name);
      g_signal_connect(G_OBJECT(item[i]), "button-press-event", G_CALLBACK(run_ifup), NULL);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), item[i]);
      gtk_widget_show(item[i]);
    }
  }
  closedir(dir);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), config_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_item);

  // tray icon
  GtkWidget *box;
  GtkWidget *icon;
  GtkWidget *tray_icon;
  box = gtk_event_box_new();
  icon = gtk_image_new();
  tray_icon = (GtkWidget*)egg_tray_icon_new("");
  gtk_container_add(GTK_CONTAINER(box), icon);
  gtk_container_add(GTK_CONTAINER(tray_icon), box);
  gtk_widget_show_all(GTK_WIDGET(tray_icon));
  g_signal_connect_swapped(G_OBJECT(box), "button-press-event", G_CALLBACK(tray_popup), menu);

  GdkPixbuf *pixbuf;
  pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(icon));
  if(pixbuf) {
    g_object_unref(G_OBJECT(pixbuf));
  }
  pixbuf = gdk_pixbuf_new_from_file("gwifup_menu.png", NULL);
  gtk_image_set_from_pixbuf(GTK_IMAGE(icon), pixbuf);

  gtk_main();
  gtk_exit(0);

  return 0;
}
