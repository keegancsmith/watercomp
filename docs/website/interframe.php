<?php
$selected = 'Interframe';
include 'files.php';
include 'head.php';
?>

<h1>Interframe Compression</h1>
<p>
The Interframe section of the Water Compression system performs half of compression task. Interframe Compression is the technique of using information about previous frames in a simulation for compression.
</p>

<h2>Interframe Compression Schemes</h2>
<p>
Five compression schemes were implemented that exploit the temporal nature of the simulations in question. 
</p>

<h3>Polynomial Extrapolation</h3>
<p>
This is a simple scheme which predicts that atom's positions follow a simple Polynomial curve. Using the previous atoms position, a polynomial is constructed using Lagrange Polynomial Interpolation. The error from the prediction is encoded to reconstruct the positions when decoding.
</p>

<h3>Spline Extrapolation</h3>
<p>
This scheme is an adaptation of the Polynomial Extrapolation where the curve used is derived using Spline Interpolation, in our case Bezier Curves.
</p>

<h3>Smallest Error Encoding</h3>
<p>
This scheme encodes a difference to the closest atom from its current position. The benefit of this scheme is that smaller errors are encoded, however, there is an additional cost of encoding extra information in order to recover the original data.
</p>

<h3>Common Error Encoding</h3>
<p>
This is an adaptation of the Smallest Error Encoding scheme. Instead of relying on small errors to increase compression rate, it attempts to compress based on which errors are very common. Errors that occur frequently are likely to be compressed very well. 
</p>

<h3>k-Nearest Neighbour Encoding</h3>
<p>
k-Nearest Neighbour Encoding is a scheme based on the machine learning technique called k-Nearest Neighbour. In our implementation of the scheme, it attempts to learn from a limited selection of previous atom positions in an order to predict the next position.
</p>


<h2>Compression Experiment</h2>
<p>
The Compression schemes were tested on several datasets. These datasets
were chosen based on their water content, size in atoms and frames and
coherency of motion. Coherency of motion is an indicator of how random
the motion in the simulation is. The factors that affect this is the
temperature of the simulation as well as the output stride of the 
simulation, the number of frames which are simulated before one is output.
</p>

<p>
A total of sixteen datasets were used in the testing. A range of parameters 
were tested for each compression scheme. These parameters affect the number
of previous frames that can be used for prediction, or window size (which has an impact on
the degree of the polynomials for the Polynomial Extrapolation Scheme and 
Spline Extrapolation Scheme). In the case of k-Nearest Neighbour Scheme, 
there is an additional parameter to control learning settings.
</p>

<h2>Results</h2>
<p>
Our results found that the best Interframe Compression Scheme corresponds to Delta Encoding. Essentially this scheme corresponds to Polynomial Extrapolation with a window size of 1. Instead of better predictions being made with larger window sizes the opposite effect is seen. This is mainly due to the poor coherency in most molecular simulations, frames can often jump and bear little only a small resemblance to previous frames. 
</p>

<p>
At 8-bit quantisation we compress the file down to 10% on average using Delta Encoding. This is much better, by approximately 20%, than the other schemes tested for molecular simulations. 
</p>

<br /><br />
For further details:
<ul class="downloads">
<?php
echo_download_item('julian-report', 'Interframe Report');
echo_download_item('project-implementation');
?>
</ul>

<?php
include 'tail.php';
?>
