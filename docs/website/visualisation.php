<?php
$selected = 'Visualisation';
include 'files.php';
include 'head.php';
?>

<h1>Visualisation</h1>
<p>
The Visualisation component of the Water Compression system supports the main
goal of the project: compressing molecular simulations. To do so, the molecular
simulation data will need to be visualised, and a quantisation experiment
carried out.
</p>

<h2>Visualisations</h2>
<p>
A number of visualisations have been developed. The visualisations chosen 
either support the compression component of the system, or allow for more 
general interpretation of data.
</p>

<h3>Water point visualisation</h3>
<div class="right-float">
<img alt="water-point image" src="images/water-point.png" />
</div>
<p>
A simple visualisation using the effects of transparency to convey an idea of
the volume.
</p>

<p>
The figure on the right shows two distinct regions of water: the blue areas in
the image. There is a region of non-water in the middle, which can be clearly
seen.
</p>

<div class="clearfix"> </div>
<h3>Ball-and-stick visualisation</h3>
<div class="right-float">
<img alt="ball-and-stick image" src="images/ball-and-stick.png" />
</div>
<p>
The standard approach to visualising molecular data.
</p>

<p>
The red spheres are oxygen atoms, blue spheres are hydrogen atoms, and the
white cylinders are the bonds connecting the atoms together.
</p>

<div class="clearfix"> </div>
<h3>Metaballs visualisation</h3>
<div class="right-float">
<img alt="metaballs image" src="images/metaballs.png" />
</div>
<p>
Determines and renders the surface between the water and non-water regions of
the volume.
</p>

<p>
Again, there are two distinct regions of water on the left and right, with a
region of non-water in the middle.
</p>

<div class="clearfix"> </div>
<h3>Water cluster visualisation</h3>
<div class="right-float">
<img alt="water-cluster image" src="images/water-cluster.png" />
</div>
<p>
Renders the extracted water clusters from the frames.
</p>

<p>
The water molecules within a water cluster are connected using cylinders, with
spheres placed at the position of the water molecules.
</p>

<div class="clearfix"> </div>
<h3>Quantisation error visualisation</h3>
<div class="right-float">
<img alt="quantise-error image" src="images/quantise-error.png" />
</div>
<p>
Shows the errors introduced from quantising the molecular data.
</p>

<p>
The error is colour coded on a stepped colour scale. Green indicates low
quantisation error, while red indicates high quantisation error.
</p>


<div class="clearfix"> </div>
<h2>Quantisation Experiment</h2>
<p>
The position data to be compressed in our water compression system is
originally floating point data. To facilitate compression, the floating point
data is converted to integer values through quantisation. An appropriate number
of bits, the number of distinct integer values, needs to be decided. Fewer bits
will result in higher compression, but more quantisation error will be
incurred; while more bits will result in lower compression, but the data is
stored more accurately
</p>

<p>
To determine the appropriate level of quantisation, an experiment was
conducted to test the perceptually visible effects of quantisation. More simply
stated: how noticeable are the effects of quantisation? This will help
determine the appropriate level of quantisation. Use the least number of
bits, while maintaining visual similarity to the original data.
</p>

<p>
The quantisation levels tested were: 4, 6, 8 and 10 bit quantisations.
</p>

<h2>Results</h2>
<p>
User experimentation showed that people are able to notice the effects of
quantisation. The greater the quantisation (fewer distinct values), the more
noticeable the difference between the original and quantised data.
</p>

<p>
The effects of quantisation were most noticeable at 4 bit quantisation, less so
at 6 bit quantisation. The results from quantising the data using 8 and 10 bit
quantisation were rated only slightly different from the original data.
</p>

<p>
Since the perceived differences between 8 and 10 bit quantisation with the
original data are not significant, 8 bit quantisation is the recommended
quantisation level for visual data.
</p>

<br /><br />
For further details:
<ul class="downloads">
<?php
echo_download_item('min-report', 'Visualisation Report');
echo_download_item('project-implementation');
?>
</ul>

<?php
include 'tail.php';
?>
