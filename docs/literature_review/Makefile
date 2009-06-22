all: pointcloud_keegan.pdf

pointcloud_keegan.pdf: pointcloud_keegan.tex pointcloud_keegan.bbl
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex

pointcloud_keegan.bbl: pointcloud_keegan.bib
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex
	bibtex pointcloud_keegan
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex
	pdflatex \\nonstopmode\\input pointcloud_keegan.tex

clean-all: clean
	rm -f pointcloud_keegan.pdf

clean:
	rm -f pointcloud_keegan.aux pointcloud_keegan.log pointcloud_keegan.out pointcloud_keegan.bbl pointcloud_keegan.blg pointcloud_keegan.toc texput.log
