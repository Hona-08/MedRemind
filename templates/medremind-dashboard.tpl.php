<?php
/**
 * @file
 * Dashboard page template.
 * Shows today's medications, stats, and quick actions.
 *
 * Available variables:
 *   $user_name    - Current user's display name
 *   $medications  - Array of user's medications
 *   $stats        - Array with total, taken, missed counts
 */
?>

<div class="medremind-dashboard">

  <!-- Top Stats Bar -->
  <div class="medremind-stats-bar">

    <div class="stat-card stat-total">
      <div class="stat-number"><?php print $stats['total']; ?></div>
      <!-- ↑ print = output the value
           $stats['total'] comes from theme() call in .pages.inc -->
      <div class="stat-label"><?php print t('Total Meds'); ?></div>
      <!-- ↑ t() = translation function. Always wrap text in t() -->
    </div>

    <div class="stat-card stat-taken">
      <div class="stat-number"><?php print $stats['taken']; ?></div>
      <div class="stat-label"><?php print t('Taken Today'); ?></div>
    </div>

    <div class="stat-card stat-missed">
      <div class="stat-number"><?php print $stats['missed']; ?></div>
      <div class="stat-label"><?php print t('Missed'); ?></div>
    </div>

    <div class="stat-card stat-streak">
      <div class="stat-number"><?php print $stats['streak']; ?></div>
      <div class="stat-label"><?php print t('Day Streak'); ?></div>
    </div>

  </div>

  <!-- Today's Medications Section -->
  <div class="medremind-section">
    <h3><?php print t("Today's Medications"); ?></h3>

    <?php if (!empty($medications)): ?>
      <!-- ↑ Only show medication list if there are medications -->

      <div class="medremind-med-list">
        <?php foreach ($medications as $med): ?>
          <!-- ↑ Loop through each medication -->

          <?php print theme('medremind_med_card', array(
            'medication' => $med,
            'next_dose' => isset($med->next_dose) ? $med->next_dose : '',
            'streak' => isset($med->streak) ? $med->streak : 0,
          )); ?>
          <!-- ↑ theme() renders the med card template for each medication
               We pass medication data to the card template -->

        <?php endforeach; ?>
      </div>

    <?php else: ?>
      <!-- ↑ Show this if user has no medications -->

      <div class="medremind-empty">
        <div class="empty-icon">💊</div>
        <h4><?php print t('No medications yet'); ?></h4>
        <p><?php print t('Start by adding your first medication.'); ?></p>
        <?php print l(t('+ Add Medication'), 'medremind/add', array(
          'attributes' => array('class' => array('medremind-add-btn')),
        )); ?>
        <!-- ↑ l() = Drupal's link function
             l(text, path, options)
             Always use l() for links, never raw <a href> -->
      </div>

    <?php endif; ?>

  </div>

</div>