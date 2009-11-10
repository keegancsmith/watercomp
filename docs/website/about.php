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
    float: right;
    width: 160px;
    margin-left: 1em;
    font-size: 0.8em;
}

span.bold
{
    font-weight: bold;
}

</style>

<h1>About</h1>

<div id="cs-department">
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
TODO: Add something about chemistry department.
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
