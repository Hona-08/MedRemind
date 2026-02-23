<?php
/**
 * @file
 * Dashboard page template.
 * Shows today's medications, stats, and quick actions.
 */
?>

<div class="medremind-dashboard">

  <!-- Top Stats Bar -->
  <div class="medremind-stats-bar">
    <div class="stat-card stat-total">
      <div class="stat-number"><?php print (int) $stats['total']; ?></div>
      <div class="stat-label"><?php print t('Total Meds'); ?></div>
    </div>
    <div class="stat-card stat-taken">
      <div class="stat-number"><?php print (int) $stats['taken']; ?></div>
      <div class="stat-label"><?php print t('Taken Today'); ?></div>
    </div>
    <div class="stat-card stat-missed">
      <div class="stat-number"><?php print (int) $stats['missed']; ?></div>
      <div class="stat-label"><?php print t('Missed'); ?></div>
    </div>
    <div class="stat-card stat-streak">
      <div class="stat-number"><?php print (int) $stats['streak']; ?></div>
      <div class="stat-label"><?php print t('Day Streak'); ?></div>
    </div>
  </div>

  <!-- Quick Add Button -->
  <div class="dashboard-actions">
    <?php print l(t('+ Add Medication'), 'medremind/add', array(
      'attributes' => array('class' => array('medremind-add-btn')),
    )); ?>
  </div>

  <!-- Today's Medications -->
  <div class="medremind-section">
    <h3>
      <?php print t("Today's Medications"); ?>
      <span class="section-date"><?php print format_date(REQUEST_TIME, 'custom', 'l, F j'); ?></span>
      <!-- ↑ Shows: "Monday, February 23" -->
    </h3>

    <?php if (!empty($medications)): ?>

      <div class="medremind-med-list">
        <?php foreach ($medications as $med): ?>

          <div class="med-card <?php if (!empty($med->taken_today)) print 'med-card-taken'; ?>"
               data-med-id="<?php print (int) $med->med_id; ?>">
            <!-- ↑ If taken today, adds 'med-card-taken' class for green styling -->

            <!-- Left: Medication Info -->
            <div class="med-card-info">
              <div class="med-card-icon">
                <?php if (!empty($med->taken_today)): ?>
                  ✅
                <?php else: ?>
                  💊
                <?php endif; ?>
              </div>
              <div class="med-card-details">
                <h4 class="med-name"><?php print check_plain($med->name); ?></h4>
                <p class="med-dosage">
                  <?php print check_plain($med->dosage); ?>
                  &bull;
                  <?php
                    // Show friendly frequency name
                    $freq_labels = array(
                      'once_daily' => t('Once Daily'),
                      'twice_daily' => t('Twice Daily'),
                      'three_daily' => t('3x Daily'),
                      'four_daily' => t('4x Daily'),
                      'weekly' => t('Weekly'),
                      'as_needed' => t('As Needed'),
                      'custom' => t('Custom'),
                    );
                    print isset($freq_labels[$med->frequency])
                      ? $freq_labels[$med->frequency]
                      : check_plain($med->frequency);
                  ?>
                </p>
                <?php if (!empty($med->notes)): ?>
                  <p class="med-notes">📝 <?php print check_plain($med->notes); ?></p>
                <?php endif; ?>
              </div>
            </div>

            <!-- Middle: Next Dose -->
            <div class="med-card-time">
              <div class="next-dose-label"><?php print t('Next Dose'); ?></div>
              <div class="next-dose-time"><?php print $med->next_dose; ?></div>
            </div>

            <!-- Right: Action Buttons -->
            <div class="med-card-actions">
              <?php if (empty($med->taken_today)): ?>
                <!-- Show Take/Skip only if NOT taken today -->
                <a href="<?php print url('medremind/take/' . $med->med_id . '/ajax'); ?>"
                   class="med-btn med-btn-take use-ajax"
                   data-action="take"
                   data-med-id="<?php print (int) $med->med_id; ?>">
                  ✅ <?php print t('Take'); ?>
                </a>
                <a href="<?php print url('medremind/skip/' . $med->med_id . '/ajax'); ?>"
                   class="med-btn med-btn-skip use-ajax"
                   data-action="skip"
                   data-med-id="<?php print (int) $med->med_id; ?>">
                  ⏭️ <?php print t('Skip'); ?>
                </a>
              <?php else: ?>
                <span class="med-taken-badge">✅ <?php print t('Taken'); ?></span>
              <?php endif; ?>
            </div>

            <!-- Streak -->
            <?php if (!empty($med->streak) && $med->streak > 0): ?>
              <div class="med-card-streak">
                🔥 <?php print (int) $med->streak; ?> <?php print t('day streak'); ?>
              </div>
            <?php endif; ?>

            <!-- Edit/Delete -->
            <div class="med-card-manage">
              <?php print l(t('Edit'), 'medremind/edit/' . $med->med_id, array(
                'attributes' => array('class' => array('med-link-edit')),
              )); ?>
              |
              <?php print l(t('Delete'), 'medremind/delete/' . $med->med_id, array(
                'attributes' => array('class' => array('med-link-delete')),
              )); ?>
            </div>

          </div>

        <?php endforeach; ?>
      </div>

    <?php else: ?>

      <div class="medremind-empty">
        <div class="empty-icon">💊</div>
        <h4><?php print t('No medications yet'); ?></h4>
        <p><?php print t('Start by adding your first medication.'); ?></p>
        <?php print l(t('+ Add Medication'), 'medremind/add', array(
          'attributes' => array('class' => array('medremind-add-btn')),
        )); ?>
      </div>

    <?php endif; ?>

  </div>

</div>