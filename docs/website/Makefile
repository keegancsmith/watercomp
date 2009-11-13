PHP=php
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
	${PHP} index.php -s > index.html

meetings.html: meetings.php head.php
	${PHP} meetings.php -s > meetings.html

intraframe.html: intraframe.php head.php
	${PHP} intraframe.php -s > intraframe.html

interframe.html: interframe.php head.php
	${PHP} interframe.php -s > interframe.html

visualisation.html: visualisation.php head.php
	${PHP} visualisation.php -s > visualisation.html

downloads.html: downloads.php head.php
	${PHP} downloads.php -s > downloads.html

about.html: about.php head.php
	${PHP} about.php -s > about.html

clean:
	rm ${FILES}
