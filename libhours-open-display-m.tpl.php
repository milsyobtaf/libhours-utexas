<?php

/**
 * @file
 * Theme for patron hours display of currently open branches for the libhours module
 *
 * @author Sean Watkins <slwatkins@uh.edu>
 * @copyright 2011 University of Houston Libraries (http://info.lib.uh.edu)
 *
 * Available variables:
 * - $openvariables: Array of open locations and times
 * - $location: 
 *
 * @see template_preprocess()
 * @see template_preprocess_hours_open_display()
 */
?>
<!-- These styles are necessary to help style the whole page outside of the module's content area, otherwise they would be in libhours.css -->
<style type="text/css">
@media print {
#contentbox {
	border: none;
}}
h1.title {margin: 0;}
</style>

<div id="libhours-content">
    <table id="libhours-locations-open">
      <?php foreach($variables as $location): ?>
        <tr>
          <td class="libhours-open-location<?php echo (($location['child']) ? ' child' : '') ?>">
            <a href="/hours/<?php echo $location['id']; ?>"><?php echo $location['location'] ?></a>
          </td>
          <td class="libhours-open-location-hours"><?php echo $location['hours'] ?></td>
        </tr>
      <?php endforeach; ?>
    </table>
</div>