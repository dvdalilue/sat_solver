compile: all clean
	echo "Done"

all:
	pdflatex tfm
	bibtex tfm
	pdflatex tfm
	pdflatex tfm

clean:
	rm *.aux *.log *.toc *.lof *.lot *.out