all: pointcloud_keegan.pdf visualisation_min.pdf

pointcloud_keegan.pdf: pointcloud_keegan.tex pointcloud_keegan.bbl
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex

pointcloud_keegan.bbl: pointcloud_keegan.bib
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex
	bibtex pointcloud_keegan
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex

visualisation_min.pdf: visualisation_min.tex visualisation_min.bbl
	pdflatex \\nonstopmode\\input visualisation_min.tex

visualisation_min.bbl: visualisation_min.bib
	pdflatex \\nonstopmode\\input visualisation_min.tex
	bibtex visualisation_min
	pdflatex \\nonstopmode\\input visualisation_min.tex
	pdflatex \\nonstopmode\\input visualisation_min.tex

clean-all: clean
	rm -f pointcloud_keegan.pdf visualisation_min.pdf

clean:
	rm -f pointcloud_keegan.aux pointcloud_keegan.log pointcloud_keegan.out pointcloud_keegan.bbl pointcloud_keegan.blg pointcloud_keegan.toc texput.log
	rm -f visualisation_min.aux visualisation_min.log visualisation_min.out visualisation_min.bbl visualisation_min.blg visualisation_min.toc texput.log
