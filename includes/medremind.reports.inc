<?php
function medremind_reports_page() {
  global $user;

  $output = '<div class="medremind-reports">';

  // --- Section 1: Weekly Overview ---
  $output .= '<div class="report-section">';
  $output .= '<h3>📊 ' . t('Weekly Overview (Last 7 Days)') . '</h3>';
  $output .= _medremind_weekly_chart($user->uid);
  $output .= '</div>';

  // --- Section 2: Per-Medication Breakdown ---
  $output .= '<div class="report-section">';
  $output .= '<h3>💊 ' . t('Medication Breakdown') . '</h3>';
  $output .= _medremind_med_breakdown($user->uid);
  $output .= '</div>';

  // --- Section 3: Streaks ---
  $output .= '<div class="report-section">';
  $output .= '<h3>🔥 ' . t('Streak Records') . '</h3>';
  $output .= _medremind_streak_report($user->uid);
  $output .= '</div>';

  // --- Section 4: Export Button ---
  $output .= '<div class="report-section report-export">';
  $output .= '<h3>📥 ' . t('Export Data') . '</h3>';
  $output .= '<p>' . t('Download your complete medication history as CSV.') . '</p>';
  $output .= l(t('⬇ Download CSV'), 'medremind/export/csv', array(
    'attributes' => array('class' => array('medremind-add-btn'))));
  $output .= '</div>';

  $output .= '</div>';
  return $output;
}

/**
 * Weekly adherence bar chart (pure HTML/CSS — no JavaScript library needed).
 */
function _medremind_weekly_chart($uid) {
  if (!db_table_exists('medremind_log')) return '<p>' . t('No data yet.') . '</p>';

  $out = '<div class="weekly-chart">';
  $max_height = 100; // px

  for ($i = 6; $i >= 0; $i--) {
    $day_start = strtotime("-$i days midnight");
    $day_end = $day_start + 86400;
    $day_label = ($i == 0) ? t('Today') : format_date($day_start, 'custom', 'D');

    $taken = (int) db_select('medremind_log', 'l')->condition('l.uid', $uid)
      ->condition('l.action', MEDREMIND_ACTION_TAKEN)
      ->condition('l.action_time', $day_start, '>=')->condition('l.action_time', $day_end, '<')
      ->countQuery()->execute()->fetchField();
    $total = (int) db_select('medremind_log', 'l')->condition('l.uid', $uid)
      ->condition('l.action_time', $day_start, '>=')->condition('l.action_time', $day_end, '<')
      ->countQuery()->execute()->fetchField();

    $pct = $total > 0 ? round(($taken / $total) * 100) : 0;
    $height = $total > 0 ? max(5, round(($pct / 100) * $max_height)) : 5;
    $color = $pct >= 80 ? '#27ae60' : ($pct >= 50 ? '#f39c12' : '#e74c3c');
    if ($total == 0) $color = '#ddd';

    $out .= '<div class="chart-bar-wrapper">'
      . '<div class="chart-pct">' . ($total > 0 ? $pct . '%' : '—') . '</div>'
      . '<div class="chart-bar" style="height:' . $height . 'px;background:' . $color . ';"></div>'
      . '<div class="chart-label">' . $day_label . '</div>'
      . '</div>';
  }
  $out .= '</div>';

  // Weekly summary numbers
  $week_start = strtotime('-6 days midnight');
  $w_taken = (int) db_select('medremind_log', 'l')->condition('l.uid', $uid)
    ->condition('l.action', MEDREMIND_ACTION_TAKEN)->condition('l.action_time', $week_start, '>=')
    ->countQuery()->execute()->fetchField();
  $w_total = (int) db_select('medremind_log', 'l')->condition('l.uid', $uid)
    ->condition('l.action_time', $week_start, '>=')->countQuery()->execute()->fetchField();
  $w_missed = (int) db_select('medremind_log', 'l')->condition('l.uid', $uid)
    ->condition('l.action', MEDREMIND_ACTION_MISSED)->condition('l.action_time', $week_start, '>=')
    ->countQuery()->execute()->fetchField();
  $w_pct = $w_total > 0 ? round(($w_taken / $w_total) * 100) : 0;

  $out .= '<div class="weekly-summary">'
    . '<div class="ws-item"><span class="ws-num">' . $w_taken . '</span><span class="ws-label">' . t('Taken') . '</span></div>'
    . '<div class="ws-item"><span class="ws-num">' . $w_missed . '</span><span class="ws-label">' . t('Missed') . '</span></div>'
    . '<div class="ws-item"><span class="ws-num">' . $w_pct . '%</span><span class="ws-label">' . t('Adherence') . '</span></div>'
    . '</div>';

  return $out;
}

/**
 * Per-medication adherence breakdown.
 */
function _medremind_med_breakdown($uid) {
  if (!db_table_exists('medremind_medications') || !db_table_exists('medremind_log'))
    return '<p>' . t('No medications yet.') . '</p>';

  $meds = db_select('medremind_medications', 'm')->fields('m', array('med_id', 'name', 'dosage'))
    ->condition('m.uid', $uid)->condition('m.status', MEDREMIND_STATUS_ACTIVE)->execute();

  $out = '<div class="med-breakdown">';
  $has_data = FALSE;
  $month_start = strtotime('first day of this month midnight');

  foreach ($meds as $med) {
    $taken = (int) db_select('medremind_log', 'l')->condition('l.med_id', $med->med_id)
      ->condition('l.uid', $uid)->condition('l.action', MEDREMIND_ACTION_TAKEN)
      ->condition('l.action_time', $month_start, '>=')->countQuery()->execute()->fetchField();
    $total = (int) db_select('medremind_log', 'l')->condition('l.med_id', $med->med_id)
      ->condition('l.uid', $uid)->condition('l.action_time', $month_start, '>=')
      ->countQuery()->execute()->fetchField();
    $pct = $total > 0 ? round(($taken / $total) * 100) : 0;
    $color = $pct >= 80 ? '#27ae60' : ($pct >= 50 ? '#f39c12' : '#e74c3c');

    $out .= '<div class="breakdown-row">'
      . '<div class="breakdown-name">' . check_plain($med->name) . ' <small>' . check_plain($med->dosage) . '</small></div>'
      . '<div class="breakdown-bar-bg"><div class="breakdown-bar-fill" style="width:' . $pct . '%;background:' . $color . ';"></div></div>'
      . '<div class="breakdown-pct">' . $pct . '% <small>(' . $taken . '/' . $total . ')</small></div>'
      . '</div>';
    $has_data = TRUE;
  }

  if (!$has_data) $out .= '<p>' . t('No data for this month.') . '</p>';
  $out .= '</div>';
  return $out;
}

/**
 * Streak records.
 */
function _medremind_streak_report($uid) {
  if (!db_table_exists('medremind_streaks')) return '<p>' . t('No streaks yet.') . '</p>';

  $streaks = db_query(
    'SELECT s.current_streak, s.best_streak, s.last_taken, m.name FROM {medremind_streaks} s '
    . 'INNER JOIN {medremind_medications} m ON s.med_id = m.med_id '
    . 'WHERE s.uid = :uid ORDER BY s.best_streak DESC',
    array(':uid' => $uid));

  $out = '<div class="streak-report">';
  $has = FALSE;
  foreach ($streaks as $s) {
    $last = $s->last_taken > 0 ? format_date($s->last_taken, 'short') : t('Never');
    $out .= '<div class="streak-row">'
      . '<div class="streak-med">' . check_plain($s->name) . '</div>'
      . '<div class="streak-current">🔥 ' . (int) $s->current_streak . ' ' . t('days') . '</div>'
      . '<div class="streak-best">🏆 ' . t('Best: @b', array('@b' => (int) $s->best_streak)) . '</div>'
      . '<div class="streak-last">' . t('Last: @d', array('@d' => $last)) . '</div>'
      . '</div>';
    $has = TRUE;
  }
  if (!$has) $out .= '<p>' . t('Take your first dose to start a streak!') . '</p>';
  $out .= '</div>';
  return $out;
}
