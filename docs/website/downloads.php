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
<?php echo_downloads(array('poster-pdf2', 'poster-pdf17', 'poster-png')); ?>

<h2>Project Implementation</h2>
<ul class="downloads">
<?php echo_download_item('project-implementation'); ?>

<li class="link">
<a href="http://www.cs.umd.edu/~mount/ANN/">ANN - Approximate Nearest Neighbour</a>
<br />
<span class="fileinfo">Library required for compression</span>
</li>

<li class="link">
<a href="http://qt.nokia.com/">Qt - A cross-platform application and UI framework</a>
<br />
<span class="fileinfo">Visualisation GUI toolkit</span>
</li>

<li class="link">
<a href="http://gts.sourceforge.net/">GTS - The GNU Triangulated Surfaced Library</a>
<br />
<span class="fileinfo">Library required for visualisation</span>
</li>

</ul>

<h2>Quantisation Experiment</h2>
<?php echo_downloads(array('experiment-instructions', 'experiment-form')); ?>

<?php
include 'tail.php';
?>
