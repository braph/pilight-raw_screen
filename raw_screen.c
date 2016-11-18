/*
   Copyright (C) 2016 Benjamin Abendroth

   raw_screen module for pilight.

   This file contains code from the pilight project.
*/

/*
	Copyright (C) 2014 CurlyMo

	This file is part of pilight.

	pilight is free software: you can redistribute it and/or modify it under the
	terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later
	version.

	pilight is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with pilight. If not, see	<http://www.gnu.org/licenses/>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../core/pilight.h"
#include "../../core/common.h"
#include "../../core/dso.h"
#include "../../core/log.h"
#include "../protocol.h"
#include "raw_screen.h"

static void createMessage(int id, int state) {
	raw_screen->message = json_mkobject();
	json_append_member(raw_screen->message, "id", json_mknumber(id, 1));
	if(state == 1) {
		json_append_member(raw_screen->message, "state", json_mkstring("up"));
	} else {
		json_append_member(raw_screen->message, "state", json_mkstring("down"));
	}
}

static int createCode(JsonNode *code) {
	int id = -1;
	int state = -1;
	double itmp = 0;

	if(json_find_number(code, "id", &itmp) == 0)
		id = (int)round(itmp);

	if(json_find_number(code, "down", &itmp) == 0)
		state=0;
	else if(json_find_number(code, "up", &itmp) == 0)
		state=1;

	if(id == -1 || state == -1) {
		logprintf(LOG_ERR, "raw_screen: insufficient number of arguments");
		return EXIT_FAILURE;
	} else {
		createMessage(id, state);
	}

	char *rcode = NULL;
	double repeats = 10;
	char **array = NULL;
	unsigned int i = 0, n = 0;

   if (state == 0) {
      if(json_find_string(code, "down-code", &rcode) != 0) {
         logprintf(LOG_ERR, "raw: insufficient number of arguments");
         return EXIT_FAILURE;
      }
   }
   else {
      if(json_find_string(code, "up-code", &rcode) != 0) {
         logprintf(LOG_ERR, "raw: insufficient number of arguments");
         return EXIT_FAILURE;
      }
   }

	json_find_number(code, "repeats", &repeats);

	n = explode(rcode, " ", &array);
	for(i=0;i<n;i++) {
		raw_screen->raw[i]=atoi(array[i]);
	}
	array_free(&array, n);

	raw_screen->rawlen= (int)i;
	raw_screen->txrpt = repeats;

	return EXIT_SUCCESS;
}

static void printHelp(void) {
	printf("\t -t --up\t\t\tsend an up signal\n");
	printf("\t -f --down\t\t\tsend an down signal\n");
	printf("\t -u --up-code\t\t\tup signal code\n");
	printf("\t -d --down-code\t\t\tdown signal code\n");
	printf("\t -r --repeats\t\t\trepeat signal n times\n");
	printf("\t -i --id=id\t\t\tcontrol a device with this id\n");
}

#if !defined(MODULE) && !defined(_WIN32)
__attribute__((weak))
#endif
void rawScreenInit(void) {

	protocol_register(&raw_screen);
	protocol_set_id(raw_screen, "raw_screen");
	protocol_device_add(raw_screen, "raw_screen", "raw screens");
	raw_screen->devtype = SCREEN;
	raw_screen->hwtype = RF433;

	options_add(&raw_screen->options, 't', "up", OPTION_NO_VALUE, DEVICES_STATE, JSON_STRING, NULL, NULL);
	options_add(&raw_screen->options, 'f', "down", OPTION_NO_VALUE, DEVICES_STATE, JSON_STRING, NULL, NULL);
	options_add(&raw_screen->options, 'u', "up-code", OPTION_HAS_VALUE, DEVICES_SETTING, JSON_STRING, NULL, NULL);
	options_add(&raw_screen->options, 'd', "down-code", OPTION_HAS_VALUE, DEVICES_SETTING, JSON_STRING, NULL, NULL);
	options_add(&raw_screen->options, 'r', "repeats", OPTION_OPT_VALUE, DEVICES_SETTING, JSON_NUMBER, NULL, NULL);
	options_add(&raw_screen->options, 'i', "id", OPTION_HAS_VALUE, DEVICES_ID, JSON_NUMBER, NULL, "^([0-9]{1,})$");

	options_add(&raw_screen->options, 0, "readonly", OPTION_HAS_VALUE, GUI_SETTING, JSON_NUMBER, (void *)0, "^[10]{1}$");
	options_add(&raw_screen->options, 0, "confirm", OPTION_HAS_VALUE, GUI_SETTING, JSON_NUMBER, (void *)0, "^[10]{1}$");

	raw_screen->printHelp=&printHelp;
	raw_screen->createCode=&createCode;
}

#if defined(MODULE) && !defined(_WIN32)
void compatibility(struct module_t *module) {
	module->name = "raw_screen";
	module->version = "1.0";
	module->reqversion = "6.0";
	module->reqcommit = "1";
}

void init(void) {
	rawScreenInit();
}
#endif
