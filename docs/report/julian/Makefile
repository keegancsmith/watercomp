report_min.pdf: report_julian.tex report_julian.bib
	pdflatex report_julian.tex
	bibtex report_julian
	pdflatex report_julian.tex
	pdflatex report_julian.tex

clean:
	rm -f report_julian.aux report_julian.bbl report_julian.blg report_julian.log report_julian.lof report_julian.toc
