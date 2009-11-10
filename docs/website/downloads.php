<?php
$selected = 'Downloads';
$css = array('downloads.css');
include 'head.php';

function get_filesize($filename)
{
    $size = filesize($filename);
    $sizelen = strlen($size);

    if ($sizelen <= 9 && $sizelen >= 7)
        $size = sprintf("%.2f MB", $size / (1024 * 1024));
    elseif ($sizelen >= 10)
        $size = sprintf("%.2f GB", $size / (1024 * 1024 * 1024));
    else
        $size = sprintf("%.2f KB", $size / 1024);
    return $size;
}//get_filesize

$downloads = array(
    'Project Proposal' => array('document', 'proposal.pdf', 'PDF'),
    'Proposal Presentation' => array('presentation', 'presentation.pdf', 'PDF'),
    'Keegan\'s Report' => array('document gap', 'report_keegan.pdf', 'PDF'),
    'Julian\'s Report' => array('document', 'report_julian.pdf', 'PDF'),
    'Min-Young\'s Report' => array('document', 'report_min.pdf', 'PDF'),
    'Project Poster (PDF)' => array('presentation gap', 'poster.pdf', 'PDF'),
    'Project Poster (PNG)' => array('presentation', 'poster.png', 'PNG'),
    'Keegan\'s Implementation' => array('archive gap', 'keegan.tar.gz', 'TGZ'),
    'Julian\'s Implementation' => array('archive', 'julian.tar.gz', 'TGZ'),
    'Min-Young\'s Implementation' => array('archive', 'min.tar.gz', 'TGZ'),
);
?>

<h1>Downloads</h1>

<ul id="downloads">

<?php foreach ($downloads as $name => $details): ?>

<?php $filename = "downloads/{$details[1]}"; ?>
<li class="<?php echo $details[0]; ?>">
<a href="<?php echo $filename; ?>"><?php echo $name; ?></a>
<br />
<span class="fileinfo"><?php echo $details[2]; ?> - <?php echo get_filesize($filename); ?></span>
</li>
<?php endforeach ?>

</ul>

<?php
include 'tail.php';
?>
