<?php
$selected = 'About';
include 'head.php';

function echo_email($email)
{
    echo "<a href=\"mailto:{$email}\">{$email}</a>";
}//echo_email

?>

<style>
ul.people li
{
    margin-bottom: 1.5em;
}

div#cs-department
{
    width: 160px;
    font-size: 0.8em;
}

</style>

<h1>About</h1>

<div class="right-float" id="cs-department">
<a href="http://www.cs.uct.ac.za/"><img src="images/cslogo.png" />
Department of Computer Science</a><br />
<a href="http://www.uct.ac.za/">University of Cape Town</a><br />

Private Bag X3<br />
Rondebosch<br />
7701<br />
South Africa<br />
</div>

<p>
This project was completed for the Department of Computer Science of the
University of Cape Town.
</p>

<p>
This project was in part proposed by John Stone of the Theoretical and 
Computational Biophysics Group at the University of Illinois. It was 
supervised by Dr. Patrick Marais and co-supervised by Dr. James Gain.
</p>

<p>
Parts of the projects may be used in a molecular simulation visualisation
program called VMD. 
</p>

<div class="clearfix"> </div>

<h2>Project Team</h2>
<ul class="people">
    <li>
        <span class="bold">Intraframe Compression</span><br />
        Keegan Carruthers-Smith<br />
        <?php echo_email('ksmith@cs.uct.ac.za'); ?>
    </li>
    <li>
        <span class="bold">Interframe Compression</span><br />
        Julian Kenwood<br />
        <?php echo_email('jkenwood@cs.uct.ac.za'); ?>
    </li>
    <li>
        <span class="bold">Visualisation</span><br />
        Min-Young Wu<br />
        <?php echo_email('mwu@cs.uct.ac.za'); ?>
    </li>
</ul>

<h2>Supervisors</h2>
<ul class="people">
    <li>
        <a href="http://www.cs.uct.ac.za/~patrick/">Dr. Patrick Marais</a><br />
        Department of Computer Science, University of Cape Town<br />
        <?php echo_email('patrick@cs.uct.ac.za'); ?>
    </li>
    <li>
        <a href="http://www.cs.uct.ac.za/~jgain/">Dr. James Gain</a><br />
        Department of Computer Science, University of Cape Town<br />
        <?php echo_email('jgain@cs.uct.ac.za'); ?>
    </li>
</ul>

<?php
include 'tail.php';
?>
