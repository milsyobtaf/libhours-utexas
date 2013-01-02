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
  <div id="libhours-periods-m">
      <div class="libhours-disclaimer-m">
  		  <p><?php echo t('In most libraries, the circulation and reserve desks close 15 minutes before the library closes.') ?></p>
  		  <p><?php echo t('UT Libraries are restricted to UT students, faculty and staff between the hours of 10pm and 7am.') ?></p>
  		  <p><strong><?php echo t('Hours are subject to change without notice.') ?></strong></p>
      </div>
    <?php foreach($periods as $period): ?>
      <div id="libhours-period-m-<?php echo $period['pid'] ?>" class="libhours-view <?php echo (($period['pid'] == $pid) ? 'selected' : 'deselected') ?>">
        <div class="libhours-stdhours-m">
          <div class="libhours-name-m">
	          <?php foreach($locations as $location): ?>
	          	<?php echo (($location['lid'] == $lid) ? '<p class="libhours-location-name-m">' . $location['name'] . '</p>' : '') ?>
	          		<?php foreach($location['children'] as $child): ?>
	          			<?php echo (($child['lid'] == $lid) ? '<p class="libhours-location-name-m">'. $location['name'] . '</p><p class="libhours-location-childname-m">' . $child['name'] . '</p>' : '') ?>
	          		<?php endforeach; ?>
	          <?php endforeach; ?>
	          <p class="libhours-period-name-m"><?php echo $period['name'] ?>&nbsp;Hours</p>
	          <!-- This date range output is here strictly for print styling - it is hidden by the default view CSS -->
	          <p class="libhours-period-name-m libhours-daterange-m"><?php echo date("F j", $period['from_date']) ?> &ndash; <?php echo date("F j", $period['to_date']) ?></p>
	      </div>
          <ul>
            <?php for($i = 0; $i < count($period['hours']); $i++): ?>
              <li class="libhours-hour-m">
                <div class="libhours-dow-m">
                  <?php echo _libhours_dow($i) ?>:
                </div>
                <div class="libhours-time-m">
                  <?php echo $period['hours'][$i] ?>
                </div>
              </li>
            <?php endfor; ?>
          </ul>
        </div>
        <?php if($period['exceptions'] || $period['emerexceptions']): ?>
        <div class="libhours-exceptions-m">
          <div class="libhours-name-m"><?php echo t('Exceptions') ?></div>
          <div class="libhours-hour-m">
            <ul>
              <?php if($period['emerexceptions']): ?>
              <?php foreach($period['emerexceptions'] as $emerexception): ?>
                <li class="libhours-label-m libhours-emergency-m"><span><?php echo $emerexception['name'] ?></span></li>
                <?php foreach($emerexception['times'] as $time): ?>
                  <li class="libhours-time-m"><?php echo $time ?></li>
                <?php endforeach; ?>
              <?php endforeach; ?>
              <?php endif; ?>
              <?php if($period['exceptions']): ?>
              <?php foreach($period['exceptions'] as $exception): ?>
                <li class="libhours-label-m"><?php echo $exception['name'] ?></li>
                <?php foreach($exception['times'] as $time): ?>
                  <li class="libhours-time-m"><?php echo $time ?></li>
                <?php endforeach; ?>
              <?php endforeach; ?>
              <?php endif; ?>
            </ul>
          </div>
        </div>
        <?php endif; ?>
        <div class="libhours-locationdescription-m"><?php echo $description ?></div>
      </div>
    <?php endforeach; ?>
  </div>
</div>