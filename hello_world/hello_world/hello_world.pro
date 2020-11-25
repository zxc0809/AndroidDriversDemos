TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += __KERNEL__
DEFINES += MODULE
INCLUDEPATH = /lib/modules/4.0.5-1-ARCH/build \
/lib/modules/5.0.2-050002-generic/build/include \
/lib/modules/5.0.2-050002-generic/build/arch/x86 \
/lib/modules/5.0.2-050002-generic/build/arch/x86/include

SOURCES += \
	khello.c

DISTFILES += \
	Makefile \
	Makefile
