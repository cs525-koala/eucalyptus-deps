TESTS =

prglibdir=$(prefix)/modules/rahas

prglib_LTLIBRARIES = libmod_rahas.la

prglib_DATA= ../data/rahas_module.xml

libmod_rahas_la_SOURCES = mod_rahas.c rahas_in_handler.c rahas_request_processor.c
libmod_rahas_la_LDFLAGS = -version-info $(VERSION_NO)

libmod_rahas_la_LIBADD  = ../util/librampart.la \
				@OPENSSLLIB@ \
				@AXIS2LIB@ \
				-lcrypto 
				

INCLUDES = -I$(top_builddir)/include \
			-I ../../include \
			@OPENSSLINC@ \
			@AXIS2INC@ \
			@UTILINC@ \
			@NEETHIINC@ \
			@AXIOMINC@

install-data-hook:
	mv $(prefix)/modules/rahas/rahas_module.xml $(prefix)/modules/rahas/module.xml
	
