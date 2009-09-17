## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in
	  
PERL = perl

scriptsdir = $(top_srcdir)/scripts
scripts_DATA = etk_server_gen_funcs etk_server_gen_signals

etk_server_gen_funcs:
	$(PERL) etk_server_gen_funcs.pl
	
etk_server_gen_signals:
	$(PERL) etk_server_gen_signals.pl	

install:
