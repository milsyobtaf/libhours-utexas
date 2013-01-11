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
<div id="libhours-content-m">
  <table id="libhours-locations-open-m">
    <?php foreach($variables as $location): ?>
      <tr class="<?php echo (($location['child']) ? ' child' : '') ?>">
        <td class="libhours-open-location-m">
          <a href="<?php global $base_path; print $base_path; ?>hours/m/<?php echo $location['id']; ?>"><?php echo (($location['child']) ? '&ndash;&nbsp;' : '') ?><?php echo $location['location'] ?></a>
        </td>
        <td class="libhours-open-location-hours-m"><?php echo $location['hours'] ?></td>
      </tr>
    <?php endforeach; ?>
  </table>
  <table id="libhours-navigation-m">
      <tr>
          <td><a href="<?php print $base_path; ?>hours/m/">All Locations</a></td>
      </tr>
      <tr>
        <td>
          <div class="libhours-disclaimer-m">
            <p><?php echo t('In most libraries, the circulation and reserve desks close 15 minutes before the library closes.') ?></p>
            <p><?php echo t('UT Libraries are restricted to UT students, faculty and staff between the hours of 10pm and 7am.') ?></p>
            <p><strong><?php echo t('Hours are subject to change without notice.') ?></strong></p>
          </div>
        </td>
      </tr>
  </table>
</div>