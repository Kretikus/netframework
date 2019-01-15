TEMPLATE = subdirs


SUBDIRS += \
	dblib \
	lib \
	test \


dblib.subdirs = dblib
lib.subdirs = lib

test.subdirs = test
test.depends = dblib lib
