all: report_keegan.pdf

report_keegan.pdf: report_keegan.tex report_keegan.bbl
	pdflatex \\nonstopmode\\input report_keegan.tex

report_keegan.bbl: report_keegan.bib
	pdflatex \\nonstopmode\\input report_keegan.tex
	bibtex report_keegan
	pdflatex \\nonstopmode\\input report_keegan.tex
	pdflatex \\nonstopmode\\input report_keegan.tex

clean-all: clean
	rm -f report_keegan.pdf

clean:
	rm -f report_keegan.aux report_keegan.log report_keegan.out report_keegan.bbl report_keegan.blg report_keegan.toc texput.log
