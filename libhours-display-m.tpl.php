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
<div id="libhours-content">
  <div id="libhours-periods">
    <ul id="libhours-periods-tabs">
      <?php foreach($periods as $period): ?>
        <li id="libhours-tab-<?php echo $period['pid'] ?>" class="<?php echo (($period['pid'] == $pid) ? 'selected' : 'deselected') ?>" onclick="javascript:libhours.tab('<?php echo $period['pid'] ?>')">
          <div class="libhours-name"><?php echo $period['name'] ?></div>
          <div class="libhours-daterange"><?php echo date("M j", $period['from_date']) ?> - <?php echo date("M j", $period['to_date']) ?></div>
        </li>
      <?php endforeach; ?>
    </ul>
    <?php foreach($periods as $period): ?>
      <div id="libhours-period-<?php echo $period['pid'] ?>" class="libhours-view <?php echo (($period['pid'] == $pid) ? 'selected' : 'deselected') ?>">
        <div class="libhours-stdhours">
          <div class="libhours-name">
	          <?php foreach($locations as $location): ?>
	          	<?php echo (($location['lid'] == $lid) ? '<p class="libhours-location-name">' . $location['name'] . '</p>' : '') ?>
	          		<?php foreach($location['children'] as $child): ?>
	          			<?php echo (($child['lid'] == $lid) ? '<p class="libhours-location-name">'. $location['name'] . '</p><p class="libhours-location-childname">' . $child['name'] . '</p>' : '') ?>
	          		<?php endforeach; ?>
	          <?php endforeach; ?>
	          <p class="libhours-period-name"><?php echo $period['name'] ?>&nbsp;Hours</p>
	          <!-- This date range output is here strictly for print styling - it is hidden by the default view CSS -->
	          <p class="libhours-period-name libhours-daterange"><?php echo date("F j", $period['from_date']) ?> &ndash; <?php echo date("F j", $period['to_date']) ?></p>
	      </div>
          <ul>
            <?php for($i = 0; $i < count($period['hours']); $i++): ?>
              <li class="libhours-hour">
                <div class="libhours-dow">
                  <?php echo _libhours_dow($i) ?>:
                </div>
                <div class="libhours-time">
                  <?php echo $period['hours'][$i] ?>
                </div>
              </li>
            <?php endfor; ?>
          </ul>
        </div>
        <?php if($period['exceptions'] || $period['emerexceptions']): ?>
        <div class="libhours-exceptions">
          <div class="libhours-name"><?php echo t('Exceptions') ?></div>
          <div class="libhours-hour">
            <ul>
              <?php if($period['emerexceptions']): ?>
              <?php foreach($period['emerexceptions'] as $emerexception): ?>
                <li class="libhours-label libhours-emergency"><span><?php echo $emerexception['name'] ?></span></li>
                <?php foreach($emerexception['times'] as $time): ?>
                  <li class="libhours-time"><?php echo $time ?></li>
                <?php endforeach; ?>
              <?php endforeach; ?>
              <?php endif; ?>
              <?php if($period['exceptions']): ?>
              <?php foreach($period['exceptions'] as $exception): ?>
                <li class="libhours-label"><?php echo $exception['name'] ?></li>
                <?php foreach($exception['times'] as $time): ?>
                  <li class="libhours-time"><?php echo $time ?></li>
                <?php endforeach; ?>
              <?php endforeach; ?>
              <?php endif; ?>
            </ul>
          </div>
        </div>
        <?php endif; ?>
        <div class="libhours-locationdescription"><?php echo $description ?></div>
        <div class="libhours-disclaimer">
          		  <p><?php echo t('In most libraries, the circulation and reserve desks close 15 minutes before the library closes.') ?></p>
          		  <p><?php echo t('UT Libraries are restricted to UT students, faculty and staff between the hours of 10pm and 7am.') ?></p>
          		  <p><strong><?php echo t('Hours are subject to change without notice.') ?></strong></p>
        </div>
      </div>
    <?php endforeach; ?>
  </div>
</div>