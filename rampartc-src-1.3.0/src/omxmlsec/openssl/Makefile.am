noinst_LTLIBRARIES =libomopenssl.la
libomopenssl_la_SOURCES = cipher_ctx.c crypt.c rsa.c pkey.c util.c cipher_property.c digest.c x509.c pkcs12.c pem.c sign.c hmac.c pkcs12_keystore.c

INCLUDES = -I$(top_builddir)/include \
			-I ../../../../util/include \
			-I ../../../../include \
			-I ../../../../axiom/include \
			@OPENSSLINC@ \
			@UTILINC@ \
			@AXIOMINC@ \
			@AXIS2INC@
