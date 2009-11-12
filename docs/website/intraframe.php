<?php
$selected = 'Intraframe';
include 'files.php';
include 'head.php';
?>

<h1>Intraframe</h1>

<p>
Intraframe compression is where each frame of the simulation is compressed
independently of everything other frame. This section of the project explored
intraframe techniques for compressing molecular dynamics simulations.
</p>

<p>
Specifically we created a Predictive Point Cloud Compressor which targets
water. A point cloud is a collection of coordinates in 3D space. So each frame
is a collection of atom position, as such it is a point cloud. Predictive
means we predict where the position are, and encode the error of the
prediction. Using the error we can recover the positions.
</p>

<h2>Water Compression</h2>

<p>
Our scheme treats water molecules and other atoms separately. The other atoms
are compressed using only quantisation. The water molecules are treated as a
point cloud (each molecule's position is the coordinates of the oxygen atom).
</p>

<p>
The water prediction scheme used is based on the water dimer model. The water
dimer model gives approximations on the bonds between pairs of water
molecules. Given a water molecule we can use this model to predict likely
positions of other water molecules.
</p>

<p>
The following is a high-level overview of how we compress the water molecules:
<ol style="list-style-type: decimal; list-style-position: inside; margin-left:
10px">
<li>Create a directed graph of the water molecules where each molecule is
connected to every other within 3 angstroms.</li>
<li>From the graph create a rooted spanning tree.</li>
<li>Do a breadth first walk of the tree, encoding the tree and prediction
errors using an arithmetic encoder.</li>
</ol>
</p>

<p>
The creation of the spanning tree in step two tries to minimise the prediction
errors encoded in step three. This is done with a breadth first search using a
priority queue on the prediction errors.
</p>


<h2>Compression Experiment</h2>

<p>
Our scheme was compared against two other point cloud compressors, as well as
gzip and another molecular dynamics compressor. Tests where done on MD data
files with varying amounts of water. Varying temperatures where also tested.
</p>

<h2>Results</h2>

<p>
Our results indicate that our water compression scheme performs best. At 8-bit
quantisation the mean and average compression rate for water compression is
17.7%. The next best compression rate was gzip with a mean and average
compression rate of 19.5% and 18.5% respectively. At 12-bit quantisation the
mean and average compression rate for water compression is 29.8%. The next
best scheme is gzip which has a mean and average compression rate of 37% and
36.5% respectively.
</p>

<br /><br />
For further details:
<ul class="downloads">
<?php
echo_download_item('keegan-report', 'Intraframe Report');
echo_download_item('project-implementation');
?>
</ul>

<?php
include 'tail.php';
?>
