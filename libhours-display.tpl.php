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
	<div id="libhours-locations">
		<ul id="libhours-location-list">
			<?php foreach($locations as $location): ?>
				<li class="libhours-location <?php echo (($location['lid'] == $lid) ? 'selected' : '') ?>" onclick="javascript:libhours.location('<?php echo $location['lid'] ?>')"><?php echo $location['name'] ?></li>
				<?php foreach($location['children'] as $child): ?>
					<li class="libhours-location libhours-child <?php echo (($child['lid'] == $lid) ? 'selected' : '') ?>" onclick="javascript:libhours.location('<?php echo $child['lid'] ?>')">- <?php echo $child['name'] ?></li>
				<?php endforeach; ?>
			<?php endforeach; ?>
		</ul>
	</div>
	<div id="libhours-periods">
		<ul id="libhours-periods-tabs">
			<?php foreach($periods as $period): ?>
				<li id="libhours-tab-<?php echo $period['pid'] ?>" class="<?php echo (($period['pid'] == $pid) ? 'selected' : '') ?>" onclick="javascript:libhours.tab('<?php echo $period['pid'] ?>')">
					<div class="libhours-name"><?php echo $period['name'] ?></div>
					<div class="libhours-daterange"><?php echo date("M j", $period['from_date']) ?> - <?php echo date("M j", $period['to_date']) ?></div>
				</li>
			<?php endforeach; ?>
		</ul>
		<?php foreach($periods as $period): ?>
			<div id="libhours-period-<?php echo $period['pid'] ?>" class="libhours-view <?php echo (($period['pid'] == $pid) ? 'selected' : '') ?>">
				<div class="libhours-stdhours">
					<div class="libhours-name"><?php echo $period['name'] ?> Hours</div>
					<ul>
						<?php for($i = 0; $i < count($period['hours']); $i++): ?>
							<li class="libhours-hour"><div class="libhours-dow"><?php echo _libhours_dow($i) ?>:</div><div class="libhours-time"><?php echo $period['hours'][$i] ?></div></li>
						<?php endfor; ?>
					</ul>
				</div>
				<?php if(count($period['exceptions']) > 0): ?>
				<div class="libhours-exceptions">
					<div class="libhours-name"><?php echo t('Exceptions') ?></div>
					<div class="libhours-hour">
						<ul>
							<?php foreach($period['exceptions'] as $exception): ?>
								<li class="libhours-label"><?php echo $exception['name'] ?></li>
								<?php foreach($exception['times'] as $time): ?>
									<li class="libhours-time"><?php echo $time ?></li>
								<?php endforeach; ?>
							<?php endforeach; ?>
						</ul>
					</div>
				</div>
				<?php endif; ?>
				<div class="libhours-locationdescription"><?php echo $description ?></div>
				<div class="libhours-disclaimer"><?php echo t('Library hours are subject to change') ?></div>
			</div>
		<?php endforeach; ?>
	</div>
</div>
