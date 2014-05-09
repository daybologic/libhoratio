#!/usr/bin/make -k

pdfs = $(shell ls *.tex | sh -x ../utils/rsed "s/tex$$/pdf/")
#SHARED = daybo.sty
SHARED=
IDENT=.ident
VERSION=.version
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
version: $(CONFIGURE_AC)
	../utils/version -c $(CONFIGURE_AC) > $(VERSION)

.PHONY: ident
ident: ../utils/hgident
	../utils/hgident > $(IDENT)

.PHONY: clean
clean:
	rm -f *.pdf *.toc *.aux *.log *.out $(IDENT) $(VERSION)
