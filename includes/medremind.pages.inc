<?php

/**
 * @file
 * Page callbacks for MedRemind.
 * Generates page content for dashboard and history.
 */

/**
 * Dashboard page — shows at /medremind
 */
function medremind_dashboard_page() {
  global $user;

  $medications = array();
  $stats = array(
    'total' => 0,
    'taken' => 0,
    'missed' => 0,
    'streak' => 0,
  );

  if (!db_table_exists('medremind_medications')) {
    // Tables not installed yet, show empty dashboard
    return theme('medremind_dashboard', array(
      'medications' => $medications,
      'stats' => $stats,
      'user_name' => format_username($user),
    ));
  }

  // -------------------------------------------
  // LOAD ALL ACTIVE MEDICATIONS FOR CURRENT USER
  // -------------------------------------------
  $result = db_select('medremind_medications', 'm')
    ->fields('m')
    ->condition('m.uid', $user->uid)
    ->condition('m.status', MEDREMIND_STATUS_ACTIVE)
    ->orderBy('m.created', 'DESC')
    ->execute();

  foreach ($result as $med) {
    // Calculate next dose time for this medication
    $med->next_dose = _medremind_get_next_dose($med);
    // ↑ We'll create this helper function below

    // Load streak for this medication
    $med->streak = _medremind_get_streak($med->med_id, $user->uid);

    // Check if today's dose was already taken
    $med->taken_today = _medremind_is_taken_today($med->med_id, $user->uid);

    $medications[] = $med;
  }

  // -------------------------------------------
  // CALCULATE STATS
  // -------------------------------------------
  $stats['total'] = count($medications);

  $today_start = strtotime('today');
  // ↑ Midnight of today as timestamp

  // Count taken doses today
  if (db_table_exists('medremind_log')) {
    $stats['taken'] = (int) db_select('medremind_log', 'l')
      ->condition('l.uid', $user->uid)
      ->condition('l.action', MEDREMIND_ACTION_TAKEN)
      ->condition('l.action_time', $today_start, '>=')
      ->countQuery()
      ->execute()
      ->fetchField();

    // Count missed doses today
    $stats['missed'] = (int) db_select('medremind_log', 'l')
      ->condition('l.uid', $user->uid)
      ->condition('l.action', MEDREMIND_ACTION_MISSED)
      ->condition('l.scheduled_time', $today_start, '>=')
      ->countQuery()
      ->execute()
      ->fetchField();
  }

  // Get best current streak across all medications
  if (db_table_exists('medremind_streaks')) {
    $best_streak = db_select('medremind_streaks', 's')
      ->fields('s', array('current_streak'))
      ->condition('s.uid', $user->uid)
      ->orderBy('s.current_streak', 'DESC')
      // ↑ Order by highest streak first
      ->range(0, 1)
      // ↑ Only get the top 1 result
      ->execute()
      ->fetchField();

    $stats['streak'] = $best_streak ? (int) $best_streak : 0;
  }

  // Render dashboard template
  return theme('medremind_dashboard', array(
    'medications' => $medications,
    'stats' => $stats,
    'user_name' => format_username($user),
  ));
}


// ============================================
// HELPER FUNCTIONS
// Prefix with _ = internal/private functions
// ============================================

/**
 * Calculate next dose time for a medication.
 *
 * Looks at the dose times stored in JSON,
 * finds which one is coming up next today.
 *
 * @param object $med
 *   Medication object from database.
 *
 * @return string
 *   Next dose time like "2:00 PM" or "Done for today".
 */
function _medremind_get_next_dose($med) {

  // Decode the times JSON
  $times = drupal_json_decode($med->times);
  // ↑ Converts '["08:00","14:00","20:00"]' to PHP array

  if (empty($times) || !is_array($times)) {
    return t('No schedule');
  }

  // Get current time in HH:MM format
  $now = date('H:i');
  // ↑ date('H:i') = "14:30" (24-hour format)

  // Sort times in order
  sort($times);
  // ↑ sort() arranges: ["08:00", "14:00", "20:00"]

  // Find the next upcoming time
  foreach ($times as $time) {
    if ($time > $now) {
      // This dose is still coming today
      return _medremind_format_time($time);
      // ↑ Convert "14:00" to "2:00 PM"
    }
  }

  // All doses passed for today
  return t('Done for today ✓');
}

/**
 * Convert 24hr time to 12hr format.
 * "14:00" → "2:00 PM"
 * "08:30" → "8:30 AM"
 */
function _medremind_format_time($time_24) {
  $timestamp = strtotime($time_24);
  // ↑ strtotime('14:00') converts to timestamp for today at 2:00 PM

  if ($timestamp === FALSE) {
    return $time_24;
    // ↑ If conversion fails, return as-is
  }

  return date('g:i A', $timestamp);
  // ↑ g = hour without leading zero (1-12)
  //   i = minutes with leading zero
  //   A = AM/PM
  //   Result: "2:00 PM"
}

/**
 * Get current streak for a medication.
 *
 * @param int $med_id
 * @param int $uid
 *
 * @return int
 *   Current streak count (0 if none).
 */
function _medremind_get_streak($med_id, $uid) {
  if (!db_table_exists('medremind_streaks')) {
    return 0;
  }

  $streak = db_select('medremind_streaks', 's')
    ->fields('s', array('current_streak'))
    ->condition('s.med_id', $med_id)
    ->condition('s.uid', $uid)
    ->execute()
    ->fetchField();

  return $streak ? (int) $streak : 0;
}

/**
 * Check if a medication dose was already taken today.
 *
 * @param int $med_id
 * @param int $uid
 *
 * @return bool
 *   TRUE if taken today, FALSE if not.
 */
function _medremind_is_taken_today($med_id, $uid) {
  if (!db_table_exists('medremind_log')) {
    return FALSE;
  }

  $today_start = strtotime('today');

  $count = db_select('medremind_log', 'l')
    ->condition('l.med_id', $med_id)
    ->condition('l.uid', $uid)
    ->condition('l.action', MEDREMIND_ACTION_TAKEN)
    ->condition('l.action_time', $today_start, '>=')
    ->countQuery()
    ->execute()
    ->fetchField();

  return ($count > 0);
}


/**
 * History page — shows at /medremind/history
 */
function medremind_history_page() {
  global $user;

  $logs = array();
  $adherence = 0;
  $month = format_date(REQUEST_TIME, 'custom', 'F Y');

  if (!db_table_exists('medremind_log')) {
    return theme('medremind_history', array(
      'logs' => $logs,
      'month' => $month,
      'adherence' => $adherence,
    ));
  }

  $month_start = strtotime('first day of this month midnight');

  // Load this month's logs
  $result = db_select('medremind_log', 'l')
    ->fields('l')
    ->condition('l.uid', $user->uid)
    ->condition('l.scheduled_time', $month_start, '>=')
    ->orderBy('l.action_time', 'DESC')
    ->range(0, 50)
    ->execute();

  foreach ($result as $log) {
    // Get medication name
    $log->med_name = db_query(
      'SELECT name FROM {medremind_medications} WHERE med_id = :mid',
      array(':mid' => $log->med_id)
    )->fetchField();

    if (empty($log->med_name)) {
      $log->med_name = t('Unknown');
    }

    $logs[] = $log;
  }

  // Calculate adherence
  $total_scheduled = (int) db_select('medremind_log', 'l')
    ->condition('l.uid', $user->uid)
    ->condition('l.scheduled_time', $month_start, '>=')
    ->countQuery()
    ->execute()
    ->fetchField();

  $total_taken = (int) db_select('medremind_log', 'l')
    ->condition('l.uid', $user->uid)
    ->condition('l.action', MEDREMIND_ACTION_TAKEN)
    ->condition('l.scheduled_time', $month_start, '>=')
    ->countQuery()
    ->execute()
    ->fetchField();

  if ($total_scheduled > 0) {
    $adherence = round(($total_taken / $total_scheduled) * 100);
  }

  return theme('medremind_history', array(
    'logs' => $logs,
    'month' => $month,
    'adherence' => $adherence,
  ));
}