report_min.pdf: report_min.tex report_min.bib
	latex report_min.tex
	bibtex report_min
	latex report_min.tex
	pdflatex report_min.tex

clean:
	rm report_min.aux
	rm report_min.bbl
	rm report_min.blg
	rm report_min.dvi
	rm report_min.log
