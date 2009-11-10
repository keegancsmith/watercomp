<?php
$selected = 'Downloads';
include 'files.php';
include 'head.php';

function echo_downloads($keys)
{
?>
<ul class="downloads">
<?php foreach ($keys as $key) echo_download_item($key); ?>
</ul>
<?php
}//echo_downloads
?>

<h1>Downloads</h1>

<h2>Project Proposal</h2>
<?php echo_downloads(array('proposal', 'presentation')); ?>

<h2>Project Reports</h2>
<?php echo_downloads(array('keegan-report', 'julian-report', 'min-report')); ?>

<h2>Project Poster</h2>
<?php echo_downloads(array('poster-pdf', 'poster-png')); ?>

<h2>Project Implementations</h2>
<?php echo_downloads(array('keegan-implementation', 'julian-implementation', 'min-implementation')); ?>

<h2>Quantisation Experiment</h2>
<?php echo_downloads(array('experiment-instructions', 'experiment-form')); ?>

<?php
include 'tail.php';
?>
