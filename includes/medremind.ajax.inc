<?php

/**
 * @file
 * AJAX callbacks for MedRemind.
 * Handle Take/Skip actions without page reload.
 */

/**
 * AJAX: Mark medication dose as taken.
 *
 * Called when user clicks the "Take" button.
 * URL: /medremind/take/{med_id}/ajax
 */
function medremind_take_dose_ajax($med_id) {
  global $user;

  // Record the dose in the log table
  $log_id = db_insert('medremind_log')
    ->fields(array(
      'med_id' => $med_id,
      'uid' => $user->uid,
      'scheduled_time' => REQUEST_TIME,
      'action' => MEDREMIND_ACTION_TAKEN,
      // ↑ Uses our constant 'taken'
      'action_time' => REQUEST_TIME,
      // ↑ When the user actually took it (now)
      'notes' => '',
    ))
    ->execute();

  // Update the streak
  _medremind_update_streak($med_id, $user->uid);

  // Get updated info for the response
  $med_name = db_query(
    'SELECT name FROM {medremind_medications} WHERE med_id = :mid',
    array(':mid' => $med_id)
  )->fetchField();

  $streak = _medremind_get_streak_count($med_id, $user->uid);

  // Return JSON response to JavaScript
  drupal_json_output(array(
    'status' => 'ok',
    'action' => 'taken',
    'med_id' => $med_id,
    'med_name' => $med_name,
    'streak' => $streak,
    'message' => t('Great job! @name marked as taken.', array('@name' => $med_name)),
  ));
  // ↑ JavaScript will receive this JSON and update the page

  drupal_exit();
  // ↑ IMPORTANT: Stop Drupal from rendering the full page
  //   We only want JSON output, not HTML
}

/**
 * AJAX: Mark medication dose as skipped.
 */
function medremind_skip_dose_ajax($med_id) {
  global $user;

  db_insert('medremind_log')
    ->fields(array(
      'med_id' => $med_id,
      'uid' => $user->uid,
      'scheduled_time' => REQUEST_TIME,
      'action' => MEDREMIND_ACTION_SKIPPED,
      'action_time' => REQUEST_TIME,
      'notes' => '',
    ))
    ->execute();

  // Skipping breaks the streak
  _medremind_reset_streak($med_id, $user->uid);

  $med_name = db_query(
    'SELECT name FROM {medremind_medications} WHERE med_id = :mid',
    array(':mid' => $med_id)
  )->fetchField();

  drupal_json_output(array(
    'status' => 'ok',
    'action' => 'skipped',
    'med_id' => $med_id,
    'med_name' => $med_name,
    'message' => t('@name skipped.', array('@name' => $med_name)),
  ));

  drupal_exit();
}


// ============================================
// STREAK HELPERS
// ============================================

/**
 * Update streak when user takes a dose.
 * Increases current_streak by 1.
 * Updates best_streak if current is higher.
 */
function _medremind_update_streak($med_id, $uid) {

  // Load current streak data
  $streak = db_select('medremind_streaks', 's')
    ->fields('s')
    ->condition('s.med_id', $med_id)
    ->condition('s.uid', $uid)
    ->execute()
    ->fetchObject();

  if (!$streak) {
    // No streak record yet, create one
    db_insert('medremind_streaks')
      ->fields(array(
        'uid' => $uid,
        'med_id' => $med_id,
        'current_streak' => 1,
        'best_streak' => 1,
        'last_taken' => REQUEST_TIME,
      ))
      ->execute();
    return;
  }

  // Check if last taken was yesterday (streak continues)
  // or earlier (streak resets to 1)
  $last_date = date('Y-m-d', $streak->last_taken);
  // ↑ Get date part only: "2026-02-22"
  $yesterday = date('Y-m-d', strtotime('yesterday'));
  $today = date('Y-m-d');

  if ($last_date == $today) {
    // Already taken today — just update timestamp, don't double count
    db_update('medremind_streaks')
      ->fields(array('last_taken' => REQUEST_TIME))
      ->condition('med_id', $med_id)
      ->condition('uid', $uid)
      ->execute();
  }
  elseif ($last_date == $yesterday) {
    // Taken yesterday — streak continues!
    $new_streak = $streak->current_streak + 1;
    $best = max($new_streak, $streak->best_streak);
    // ↑ max() returns the larger of two values
    //   If new streak beats record, update best_streak

    db_update('medremind_streaks')
      ->fields(array(
        'current_streak' => $new_streak,
        'best_streak' => $best,
        'last_taken' => REQUEST_TIME,
      ))
      ->condition('med_id', $med_id)
      ->condition('uid', $uid)
      ->execute();
  }
  else {
    // Missed a day — streak resets to 1
    db_update('medremind_streaks')
      ->fields(array(
        'current_streak' => 1,
        'last_taken' => REQUEST_TIME,
        // ↑ best_streak stays unchanged (keep the record)
      ))
      ->condition('med_id', $med_id)
      ->condition('uid', $uid)
      ->execute();
  }
}

/**
 * Reset streak when user skips a dose.
 */
function _medremind_reset_streak($med_id, $uid) {
  db_update('medremind_streaks')
    ->fields(array(
      'current_streak' => 0,
      // ↑ Reset to 0 — skipping breaks the streak
      //   best_streak stays unchanged
    ))
    ->condition('med_id', $med_id)
    ->condition('uid', $uid)
    ->execute();
}

/**
 * Get current streak count.
 */
function _medremind_get_streak_count($med_id, $uid) {
  $streak = db_select('medremind_streaks', 's')
    ->fields('s', array('current_streak'))
    ->condition('s.med_id', $med_id)
    ->condition('s.uid', $uid)
    ->execute()
    ->fetchField();

  return $streak ? (int) $streak : 0;
}