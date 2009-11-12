<?php
$selected = 'Meetings';
include 'head.php';

$meetings = array(
    mktime(0, 0, 0,  4, 29, 2009),
    mktime(0, 0, 0,  5, 14, 2009),
    mktime(0, 0, 0,  6, 23, 2009),
    mktime(0, 0, 0,  7, 10, 2009),
    mktime(0, 0, 0,  9,  9, 2009),
    mktime(0, 0, 0,  9, 15, 2009),
    mktime(0, 0, 0,  9, 22, 2009),
    mktime(0, 0, 0,  9, 29, 2009),
    mktime(0, 0, 0, 10,  6, 2009),
    mktime(0, 0, 0, 10, 13, 2009),
    mktime(0, 0, 0, 10, 20, 2009),
    mktime(0, 0, 0, 10, 27, 2009),
);

?>

<h1>Meeting minutes</h1>

<select onchange="document.getElementById('meeting_frame').src = this.value;">
<?php foreach ($meetings as $meeting): ?>
<option value="meetings/<?php echo strftime('%Y-%m-%d', $meeting); ?>.html"><?php echo strftime('%d %B %Y', $meeting); ?></option>
<?php endforeach ?>
</select>

<!--
<ul id="minutes">
<?php foreach ($meetings as $meeting): ?>
<li>
<a href="meetings/<?php echo strftime('%Y-%m-%d', $meeting); ?>.html" target="meeting"><?php echo strftime('%d %B %Y', $meeting); ?></a>
</li>
<?php endforeach ?>
</ul>
-->

<br />
<script type="text/javascript">
function resizeIFrame()
{
    var iframe = document.getElementById('meeting_frame');
    var innerDoc = iframe.contentDocument ? iframe.contentDocument : iframe.contentWindow.document;
    iframe.height = innerDoc.body.scrollHeight + 50;
}//resizeIFrame
</script>

<iframe frameborder="0" id="meeting_frame" name="meeting" width="100%" onload="resizeIFrame();">
</iframe>

<?php
include 'tail.php';
?>
