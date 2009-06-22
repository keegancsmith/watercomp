all: proposal.pdf slides.pdf

proposal.pdf: proposal.tex proposal.bib
	pdflatex -halt-on-error proposal.tex
	bibtex proposal
	pdflatex -halt-on-error proposal.tex
	pdflatex -halt-on-error proposal.tex

slides.pdf: slides.tex
	pdflatex -halt-on-error slides.tex
