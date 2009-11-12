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
<a href="http://bitbucket.org/keegan_csmith/watercomp/" target="_blank">Project source code
<img alt="new window" src="images/arrow.png" /></a>
<br />
<span class="fileinfo">Project source code</span>
</li>

<li class="link">
<a href="http://www.cs.umd.edu/~mount/ANN/" target="_blank">ANN - Approximate Nearest Neighbour
<img alt="new window" src="images/arrow.png" /></a>
<br />
<span class="fileinfo">Library required for compression</span>
</li>

<li class="link">
<a href="http://qt.nokia.com/" target="_blank">Qt - A cross-platform application and UI framework
<img alt="new window" src="images/arrow.png" /></a>
<br />
<span class="fileinfo">Visualisation GUI toolkit</span>
</li>

<li class="link">
<a href="http://gts.sourceforge.net/" target="_blank">GTS - The GNU Triangulated Surfaced Library
<img alt="new window" src="images/arrow.png" /></a>
<br />
<span class="fileinfo">Library required for visualisation</span>
</li>

</ul>

<h2>Quantisation Experiment</h2>
<?php echo_downloads(array('experiment-instructions', 'experiment-form')); ?>

<?php
include 'tail.php';
?>
