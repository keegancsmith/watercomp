all: report_min.pdf

report_min.pdf: report_min.tex \
		abstract.tex \
		introduction.tex \
		background.tex \
		design.tex \
		implementation.tex \
		experiment.tex \
		conclusion.tex \
		appendix.tex \
		report_min.bbl
	pdflatex report_min.tex

report_min.bbl: report_min.bib
	pdflatex report_min.tex
	bibtex report_min
	pdflatex report_min.tex

clean:
	rm -f report_min.aux report_min.bbl report_min.blg report_min.log \
		report_min.lof report_min.lot report_min.out report_min.toc \
		abstract.aux \
		introduction.aux \
		background.aux \
		design.aux \
		implementation.aux \
		experiment.aux \
		conclusion.aux \
		appendix.aux \

