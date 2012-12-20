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

#libhours-locations-open {
  float: left;
  font-size: 1.2em;
  font-weight: bold;
  padding: 4px;
}
#libhours-locations-open li.libhours-location-open {}
#libhours-locations-open li.libhours-location-open.child {
  margin-left: 5px;
}
</style>

<div id="libhours-content">
    <ul id="libhours-locations-open">
      <?php foreach($variables as $location): ?>
        <li class="libhours-location-open<?php echo (($location['child']) ? ' child' : '') ?>"><span><a href="/hours/<?php echo $location['id']; ?>"><?php echo $location['location'] ?></a></span><span><?php echo $location['hours'] ?></span></li>
      <?php endforeach; ?>
    </ul>
</div>

<!--
<?php
echo "<div style='clear:both;'><pre>";
  print_r($variables);
  echo date("Hi");
echo "</pre></div>";
?>
-->