all: pointcloud_keegan.pdf visualisation_min.pdf compression_julian.pdf

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

compression_julian.pdf: compression_julian.tex compression_julian.bbl
	pdflatex \\nonstopmode\\input compression_julian.tex

compression_julian.bbl: compression_julian.bib
	pdflatex \\nonstopmode\\input compression_julian.tex
	bibtex compression_julian
	pdflatex \\nonstopmode\\input compression_julian.tex
	pdflatex \\nonstopmode\\input compression_julian.tex

clean-all: clean
	rm -f pointcloud_keegan.pdf visualisation_min.pdf compression_julian.pdf

clean:
	rm -f pointcloud_keegan.aux pointcloud_keegan.log pointcloud_keegan.out pointcloud_keegan.bbl pointcloud_keegan.blg pointcloud_keegan.toc texput.log
	rm -f visualisation_min.aux visualisation_min.log visualisation_min.out visualisation_min.bbl visualisation_min.blg visualisation_min.toc texput.log
	rm -f compression_julian.aux compression_julian.log compression_julian.out compression_julian.bbl compression_julian.blg compression_julian.toc texput.log
