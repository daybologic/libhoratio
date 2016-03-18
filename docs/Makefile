#!/usr/bin/make -k

pdfs = $(shell ls *.tex | sh -x ../utils/bin/rsed "s/tex$$/pdf/")
#SHARED = daybo.sty
SHARED=
IDENT=.ident
VERSION=.version
DOXYGEN_VERSION=.doxygen_version
CONFIGURE_AC=../configure.ac

.PHONY: all
all: ident version $(pdfs)
	@echo Done.

# nb. multiple calls to pdflatex required, this is not a mistake.
%.pdf: %.tex $(SHARED)
	@echo Building $@
	# Need to set the environment variable TEXINPUTS to contain the shared folder.
	(TEXINPUTS=.:$$TEXINPUTS && export TEXINPUTS && \
		pdflatex $<  && \
		pdflatex $<)
	rm -f $(basename $<).toc $(basename $<).aux $(basename $<).log  $(basename $<).out

.PHONY: version
version: $(CONFIGURE_AC) ../utils/bin/version
	ver=`../utils/bin/version -c $(CONFIGURE_AC)`; \
	echo $$ver > $(VERSION); \
	echo "PROJECT_NUMBER=$$ver" > $(DOXYGEN_VERSION)

.PHONY: ident
ident: ../utils/bin/hgident
	../utils/bin/hgident > $(IDENT)

.PHONY: clean
clean:
	rm -f *.pdf *.toc *.aux *.log *.out $(IDENT) $(VERSION) $(DOXYGEN_VERSION)

.PHONY: check
check:
	@echo 'Dummy rule'

.PHONY: reallyinstall install
install:
	@echo 'Dummy rule'

reallyinstall:
	mkdir -p $(DESTDIR)/usr/share/doc/libhoratio
	install -m 444 api.pdf $(DESTDIR)/usr/share/doc/libhoratio/
