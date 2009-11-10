<?php
if (!isset($css)) $css = array();
$css[] = 'downloads.css';

$downloads = array(
    'proposal' => array(
        'name' => 'Project Proposal',
        'css' => 'document',
        'link' => 'proposal.pdf',
        'filetype' => 'PDF'),

    'presentation' => array(
        'name' => 'Proposal Presentation',
        'css' => 'presentation',
        'link' => 'presentation.pdf',
        'filetype' => 'PDF'),

    'keegan-report' => array(
        'name' => 'Keegan\'s Report',
        'css' => 'document',
        'link' => 'report_keegan.pdf',
        'filetype' => 'PDF'),

    'julian-report' => array(
        'name' => 'Julian\'s Report',
        'css' => 'document',
        'link' => 'report_julian.pdf',
        'filetype' => 'PDF'),

    'min-report' => array(
        'name' => 'Min-Young\'s Report',
        'css' => 'document',
        'link' => 'report_min.pdf',
        'filetype' => 'PDF'),

    'poster-pdf' => array(
        'name' => 'Project Poster (PDF)',
        'css' => 'presentation',
        'link' => 'poster.pdf',
        'filetype' => 'PDF'),

    'poster-png' => array(
        'name' => 'Project Poster (PNG)',
        'css' => 'presentation',
        'link' => 'poster.png',
        'filetype' => 'PNG'),

    'keegan-implementation' => array(
        'name' => 'Keegan\'s Implementation',
        'css' => 'archive',
        'link' => 'keegan.tar.gz',
        'filetype' => 'TGZ'),

    'julian-implementation' => array(
        'name' => 'Julian\'s Implementation',
        'css' => 'archive',
        'link' => 'julian.tar.gz',
        'filetype' => 'TGZ'),

    'min-implementation' => array(
        'name' => 'Min-Young\'s Implementation',
        'css' => 'archive',
        'link' => 'min.tar.gz',
        'filetype' => 'TGZ'),

    'experiment-instructions' => array(
        'name' => 'Experiment Instructions',
        'css' => 'document',
        'link' => 'experiment_instructions.pdf',
        'filetype' => 'PDF'),

    'experiment-form' => array(
        'name' => 'Experiment Form',
        'css' => 'document',
        'link' => 'experiment_form.pdf',
        'filetype' => 'PDF'),
);

function get_filesize($filename)
{
    $size = filesize($filename);
    $sizelen = strlen($size);

    if ($sizelen <= 9 && $sizelen >= 7)
        $size = sprintf("%.2f MB", $size / (1024 * 1024));
    elseif ($sizelen >= 10)
        $size = sprintf("%.2f GB", $size / (1024 * 1024 * 1024));
    else
        $size = sprintf("%.2f KB", $size / 1024);
    return $size;
}//get_filesize

function echo_download_item($key, $name=null)
{
    global $downloads;
    $details = $downloads[$key];
    $filename = "downloads/{$details['link']}";
    if (empty($name)) $name = $details['name'];
?>

<li class="<?php echo $details['css']; ?>">
<a href="<?php echo $filename; ?>"><?php echo $name; ?></a>
<br />
<span class="fileinfo"><?php echo $details['filetype']; ?> - <?php echo get_filesize($filename); ?></span>
</li>

<?php
}//get_download_item

?>

