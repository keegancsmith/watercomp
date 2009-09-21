all: report_min.pdf

report_min.pdf: report_min.tex report_min.bib introduction.tex background.tex \
		design.tex implementation.tex evaluation.tex conclusion.tex \
		report_min.bbl
	pdflatex report_min.tex

report_min.bbl: report_min.bib
	pdflatex report_min.tex
	bibtex report_min
	pdflatex report_min.tex

clean:
	rm -f report_min.aux report_min.bbl report_min.blg report_min.log \
	    report_min.lof report_min.toc \
	    introduction.aux background.aux design.aux implementation.aux \
	    evaluation.aux conclusion.aux \

