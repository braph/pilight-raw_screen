MODULE_NAME = raw_screen

build:
	cmake pilight
	gcc -fPIC -shared $(MODULE_NAME).c -iquote pilight/libs/pilight/protocols/433.92/ -iquote pilight/inc  -o $(MODULE_NAME).so -DMODULE=1

install:
	install -m 664 $(MODULE_NAME).so /usr/local/lib/pilight/protocols/$(MODULE_NAME).so

clean:
	rm -f $(MODULE_NAME).so
