#!/usr/bin/make -k

pdfs = $(shell ls *.tex | sh -x ../utils/rsed "s/tex$$/pdf/")
#SHARED = daybo.sty
SHARED=
IDENT=.ident

.PHONY: all
all: ident $(pdfs)
	@echo Done.

# nb. multiple calls to pdflatex required, this is not a mistake.
%.pdf: %.tex $(SHARED)
	@echo Building $@
	# Need to set the environment variable TEXINPUTS to contain the shared folder.
	(TEXINPUTS=.:$$TEXINPUTS && export TEXINPUTS && \
		pdflatex $<  && \
		pdflatex $<)
	rm -f $(basename $<).toc $(basename $<).aux $(basename $<).log  $(basename $<).out

.PHONY: ident
ident:
	hg ident > $(IDENT)

.PHONY: clean
clean:
	rm -f *.pdf *.toc *.aux *.log *.out $(IDENT)
