<?php
$selected = 'Overview';
$title = 'Water compression';
include 'head.php';
?>

<h1>Overview</h1>

<p>
Molecular simulations run for days on end, generating lots and lots of data.
The current state of the art simulations (in the year 2009) generate data
consisting of 2 million atoms, with 100 thousand frames. This is approximately
1 terabyte of data, 50~80% of which is water. Transferring and storing so much
data is a problem, a solution to this is to compress the data.
</p>

<p>
Water has certain structural properties, which can be exploited to perform
prediction, and hence compression. This project: <span class="bold">Water
Compression</span>, thus aims to exploit certain structural properties of
water to effectively compress molecular simulations.
</p>

<p>
To more effectively compress the data, the data is first quantised.
Quantisation is the conversion from floating point data to integer values.
Integer values have fewer distinct values in comparison to floating point
data, thus requiring fewer bits to represent and reproduce the input data.
</p>

<p>
The project as a whole has been separated out into 3 components: Intraframe
compression (compression within a specific frame), Interframe compression
(compression across frames), and Visualisation (which also tests the effects
of quantisation).
</p>

<h2>System components</h2>

<h3>Intraframe compression</h3>
<p>
Intraframe compression is where each frame of the simulation is compressed
independantly of everything other frame. We created a compression scheme which
uses a model of the structure of water to predict where each water molecule
is. The water and non-water atoms are compressed seperately.
</p>

<p>
This section of the project explores intraframe techniques for compressing
molecular dynamics simulations. Our scheme is compared to other schemes, using
data which was generated with different percentage amounts of water.
</p>

<h3>Interframe compression</h3>
<p>
Interframe compression performs compression across frames of the molecular
simulations. The temporal properties of the simulation as a whole, are used to
compress the data. The water and non-water atoms are treated the same.
</p>

<p>
Five interframe compression schemes were implemented and tested on various datasets.
The datasets were chosen based on properties such as size and the coherence of
the motion in the simulation.
</p>

<h3>Visualisation</h3>
<p>
The visualisation component of the system is broken up into two main parts, a
quantisation experiment and developing visualisations to effectively visualise
the simulation data.
</p>

<p>
The results from the quantisation experiment is used to recommend an
appropriate level of quantisation to use. A number of visualisations were
developed to support the system as a whole.
</p>


<?php
include 'tail.php';
?>
