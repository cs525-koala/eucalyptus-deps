TESTS =

prglibdir=$(prefix)/modules/rampart

prglib_LTLIBRARIES = libmod_rampart.la

prglib_DATA= ../data/module.xml

libmod_rampart_la_SOURCES = mod_rampart.c
libmod_rampart_la_LDFLAGS = -version-info $(VERSION_NO)

libmod_rampart_la_LIBADD  = ../handlers/librampart_handlers.la \
			    ../util/librampart.la \
				@OPENSSLLIB@ \
				@AXIS2LIB@ \
				-lcrypto \
				-laxis2_engine
				

INCLUDES = -I$(top_builddir)/include \
			-I ../../include \
			@OPENSSLINC@ \
			@AXIS2INC@ \
			@UTILINC@ \
			@NEETHIINC@ \
			@AXIOMINC@

