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

<ul id="minutes">
<?php foreach ($meetings as $meeting): ?>
<li>
  <a href="meetings/<?php echo strftime('%Y-%m-%d', $meeting); ?>.html" target="meeting"><?php echo strftime('%d %B %Y', $meeting); ?></a>
</li>
<?php endforeach ?>
</ul>

<br />
<script type="text/javascript">
function resizeIFrame()
{
    var iframe = document.getElementById('meeting_frame');
    var innerDoc = iframe.contentDocument ? iframe.contentDocument : iframe.contentWindow.document;
    iframe.height = innerDoc.body.scrollHeight + 50;
}//resizeIFrame

function createOption(value, text)
{
    var o = document.createElement('option');
    o.value = value;
    o.innerHTML = text;
    return o;
}//createOption

function convertULAToSelect(id)
{
    var ul = document.getElementById(id);
    var select = document.createElement('select');
    var opts = ul.getElementsByTagName('a');
    var opt = undefined;

    //loop through the links and add a corresponding option
    //to the select
    select.appendChild(createOption('#', '-- Select a date --'));
    for (var i = 0; i < opts.length; i++)
        select.appendChild(createOption(opts[i], opts[i].innerHTML));
    select.onchange = function()
    {
        document.getElementById('meeting_frame').src = this.value;
    }//select.onchange

    //insert the select and remove the old ul
    ul.parentNode.insertBefore(select, ul);
    ul.parentNode.removeChild(ul);
    return select;
}//convertULAToSelect

window.onload = function()
{
    convertULAToSelect('minutes');
    document.getElementById('meeting_frame').height = 1;
}//window.onload
</script>

<iframe frameborder="0" height="400" id="meeting_frame" name="meeting" width="100%" onload="resizeIFrame();">
</iframe>

<?php
include 'tail.php';
?>
