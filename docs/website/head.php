<?php
$ext = '.php';
$args = getopt("", array("static::"));
if (isset($args['static']))
    $ext = $args['static'] === false ? '.html' : ".{$args['static']}";

$links = array(
    'Overview' => 'index'.$ext,
    'Meetings' => 'meetings'.$ext,
    'Intraframe' => 'intraframe'.$ext,
    'Interframe' => 'interframe'.$ext,
    'Visualisation' => 'visualisation'.$ext,
    'Downloads' => 'downloads'.$ext,
    'About' => 'about'.$ext,
);
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <title><?php
        if (!isset($title))
            $title = isset($selected) ? $selected : 'Water compression';
        echo $title;
    ?></title>
  <link href="css/reset.css"  rel="stylesheet" type="text/css" />
  <link href="css/basic.css"  rel="stylesheet" type="text/css" />
  <link href="css/structure.css" rel="stylesheet" type="text/css" />
  <link href="css/navbar.css" rel="stylesheet" type="text/css" />
<?php if (!isset($css)) $css = array(); ?>
<?php foreach ($css as $c): ?>
    <link href="css/<?php echo $c; ?>" rel="stylesheet" type="text/css" />
<?php endforeach ?>
</head>
<body>
    <div id="main">
        <a href="<?php echo $links['Overview']; ?>"><img alt="banner" src="images/banner.png" /></a>
        <div id="sidebar">


<h4 style="padding-left: 1em;">Navigation</h4>
<ul id="navbar">
<?php foreach ($links as $name => $link): ?>
    <li<?php if ($selected == $name) echo ' id="selected"'; ?>><a href="<?php echo $link; ?>"><?php echo $name; ?></a></li>
<? endforeach ?>
</ul> <!-- ul#navbar -->


        </div> <!-- div#sidebar -->

        <div id="content">

