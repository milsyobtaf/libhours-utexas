<?php

/**
 * @file
 * Theme for patron hours display of currently open branches for the libhours module
 *
 * @author Sean Watkins <slwatkins@uh.edu>
 * @copyright 2011 University of Houston Libraries (http://info.lib.uh.edu)
 *
 * Available variables:
 * - $lid: Location unique identifier
 * - $pid: Period unique identifier
 * - $locations: Array of available locations
 * - $periods: Array of period information for a selected location
 * - $description: Location description string
 *
 * @see template_preprocess()
 * @see template_preprocess_hours_open_display()
 */
firep("hello!");
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
  <div id="libhours-locations">
    <ul id="libhours-location-list">
    hello world!
    <?php echo $description ?>
      <?php foreach($return['locations'] as $location): ?>
        <li><?php echo $location ?></li>
      <?php endforeach; ?>
    </ul>
  </div>
</div>
<?php firep("hello!"); ?>