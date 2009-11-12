FILES=\
	index.html \
	meetings.html \
	intraframe.html \
	interframe.html \
	visualisation.html \
	downloads.html \
	about.html

ALL: ${FILES}


index.html: index.php head.php
	php index.php -s > index.html

meetings.html: meetings.php head.php
	php meetings.php -s > meetings.html

intraframe.html: intraframe.php head.php
	php intraframe.php -s > intraframe.html

interframe.html: interframe.php head.php
	php interframe.php -s > interframe.html

visualisation.html: visualisation.php head.php
	php visualisation.php -s > visualisation.html

downloads.html: downloads.php head.php
	php downloads.php -s > downloads.html

about.html: about.php head.php
	php about.php -s > about.html

clean:
	rm ${FILES}
