<?php

/**
 * @file
 * Theme for patron hours display for the libhours module
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
 * @see template_preprocess_hours_display()
 */
?>
<div id="libhours-content-m">
<?php if ($variables['lid'] == 0): ?>
  <table id="libhours-location-m">
    <?php foreach($locations as $location): ?>
      <tr>
      <td class="libhours-location-m <?php echo (($location['lid'] == $lid) ? 'selected' : '') ?>">
        <a href="<?php global $base_path; print $base_path . "hours/m/" . $location['lid'] ?>"><?php echo $location['name'] ?></a>
      </td>
      </tr>
      <?php foreach($location['children'] as $child): ?>
        <tr>
        <td class="libhours-location libhours-child-m <?php echo (($child['lid'] == $lid) ? 'selected' : '') ?>">
          <a href="<?php global $base_path; print $base_path . "hours/m/" . $child['lid'] ?>">&ndash;&nbsp;<?php echo $child['name'] ?></a>
        </td>
        </tr>
      <?php endforeach; ?>
    <?php endforeach; ?>
  </table>
<?php endif; ?>
<?php if ($variables['lid'] != 0): ?>
  <div id="libhours-periods-m">
    <?php foreach($locations as $location): ?>
  	<?php echo (($location['lid'] == $lid) ? '<p class="libhours-location-name-m">' . $location['name'] . '</p>' : '') ?>
  		<?php foreach($location['children'] as $child): ?>
  			<?php echo (($child['lid'] == $lid) ? '<p class="libhours-location-name-m">'. $location['name'] . '</p><p class="libhours-location-childname-m">' . $child['name'] . '</p>' : '') ?>
  		<?php endforeach; ?>
    <?php endforeach; ?>
    <?php foreach($periods as $period): ?>
      <div id="libhours-period-m-<?php echo $period['pid'] ?>" class="libhours-view <?php echo (($period['pid'] == $pid) ? 'selected' : 'deselected') ?>">
        <div class="libhours-stdhours-m">
          <div class="libhours-name-m">
	          <p class="libhours-period-name-m"><?php echo $period['name'] ?>&nbsp;Hours</p>
	          <p class="libhours-period-name-m libhours-daterange-m"><?php echo date("F j", $period['from_date']) ?> &ndash; <?php echo date("F j", $period['to_date']) ?></p>
	      </div>
          <table class="libhours-stdhours-m">
            <?php for($i = 0; $i < count($period['hours']); $i++): ?>
            <tr class="libhours-hour">
              <td class="libhours-dow-m">
                <?php echo _libhours_dow($i) ?>:
              </td>
              <td class="libhours-time-m">
                <?php echo $period['hours'][$i] ?>
              </td>
            </tr>
            <?php endfor; ?>
          </table>
        </div>
        <?php if($period['exceptions'] || $period['emerexceptions']): ?>
        <div id="libhours-exceptions-m">
        <p class="libhours-name-m"><?php print t('Exceptions') ?></p>
        <table class="libhours-exceptions-m">
          <tbody class="libhours-hour-m">
              <?php if($period['emerexceptions']): ?>
              <?php foreach($period['emerexceptions'] as $emerexception): ?>
                <?php foreach($emerexception['times'] as $time): ?>
                <tr>
                  <td class="libhours-time-m"><span><?php echo $time ?></span></td>
                  <td class="libhours-label-m libhours-emergency-m"><span><?php echo $emerexception['name'] ?></span></td>
                </tr>
                <?php endforeach; ?>
              <?php endforeach; ?>
              <?php endif; ?>
              <?php if($period['exceptions']): ?>
              <?php foreach($period['exceptions'] as $exception): ?>
                <?php foreach($exception['times'] as $time): ?>
                <tr>
                  <td class="libhours-time-m"><span><?php echo $time ?></span></td>
                  <td class="libhours-label-m"><span><?php echo $exception['name'] ?></span></td>
                </tr>
                <?php endforeach; ?>
              <?php endforeach; ?>
              <?php endif; ?>
          </tbody>
        </table>
        </div>
        <?php endif; ?>
      </div>
    <?php endforeach; ?>
<?php endif; ?>
  <div class="libhours-locationdescription-m"><?php echo $description ?></div>
  <table id="libhours-navigation-m">
    <tr>
      <td><a href="<?php global $base_path; print $base_path; ?>hours/m/open">Open Now</a></td>
      <?php if ($variables['lid'] != 0): ?>
        <td><a href="<?php print $base_path; ?>hours/m/">All Locations</a></td>
      <?php endif; ?>
    </tr>
    <tr>
      <td colspan="2">
        <div class="libhours-footnote-m">
          <p>In most libraries, the circulation and reserve desks close 15 minutes before the library closes.</p>
          <p>UT Libraries are restricted to UT students, faculty and staff between the hours of 10pm and 7am.</p>
          <p><strong>Hours are subject to change without notice.</strong></p>
          <p class="summary-pdf"><a href="<?php print $base_path; ?>sites/default/files/hours/utl_hours_summary.pdf">UT Libraries Building Hours Summary (PDF)</a></p>
        </div>
      </td>
    </tr>
  </table>
</div>
