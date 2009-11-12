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
	php index.php --static > index.html

meetings.html: meetings.php head.php
	php meetings.php --static > meetings.html

intraframe.html: intraframe.php head.php
	php intraframe.php --static > intraframe.html

interframe.html: interframe.php head.php
	php interframe.php --static > interframe.html

visualisation.html: visualisation.php head.php
	php visualisation.php --static > visualisation.html

downloads.html: downloads.php head.php
	php downloads.php --static > downloads.html

about.html: about.php head.php
	php about.php --static > about.html

clean:
	rm ${FILES}
