lib_LTLIBRARIES = libaxis2_http_receiver.la
libaxis2_http_receiver_la_LIBADD=$(top_builddir)/util/src/libaxutil.la\
                                 $(top_builddir)/src/core/transport/http/common/libaxis2_http_common.la \
                                 $(top_builddir)/src/core/engine/libaxis2_engine.la


libaxis2_http_receiver_la_SOURCES = http_receiver.c\
                                    http_svr_thread.c


libaxis2_http_receiver_la_LDFLAGS = -version-info $(VERSION_NO)

INCLUDES = -I$(top_builddir)/include \
	       -I$(top_builddir)/src/core/transport\
	       -I$(top_builddir)/src/core/transport/http \
           -I$(top_builddir)/src/core/description \
           -I$(top_builddir)/src/core/context \
           -I$(top_builddir)/src/core/phaseresolver \
           -I$(top_builddir)/src/core/engine \
           -I$(top_builddir)/src/core/deployment \
           -I$(top_builddir)/util/include \
           -I$(top_builddir)/axiom/include
