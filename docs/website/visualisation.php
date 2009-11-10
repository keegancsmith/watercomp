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
<p>
A simple visualisation using the effects of transparency to convey an idea of 
the volume.
</p>

<h3>Ball-and-stick visualisation</h3>
<p>
The standard approach to visualising molecular data.
</p>

<h3>Metaballs visualisation</h3>
<p>
Determines and renders the surface between the water and non-water regions of 
the volume.
</p>

<h3>Water cluster visualisation</h3>
<p>
Renders the extracted water clusters from the frames.
</p>

<h3>Quantisation error visualisation</h3>
<p>
Shows the errors introduced from quantising the molecular data.
</p>


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
With more quantisation, i.e. quantisation using fewer bits, the perceived
differences are greater. The median rating for 4 bit quantisation is a 6
("Very different"), 6 bit quantisation is a 4 ("Moderately different"), 8 and
10 bit quantisations are rated "Somewhat different" (median rating of 3).
</p>

<p>
Since the perceived differences between 8 and 10 bit quantisation levels are
not significant, 8 bit quantisation is the recommended quantisation level for
visual data.
</p>

<br /><br />
For further details:
<ul class="downloads">
<?php
echo_download_item('min-report', 'Visualisation Report');
echo_download_item('min-implementation', 'Visualisation Implementation');
?>
</ul>

<?php
include 'tail.php';
?>
