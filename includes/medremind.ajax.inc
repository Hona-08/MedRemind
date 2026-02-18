<?php

/**
 * @file
 * AJAX callbacks for MedRemind.
 * These return JSON responses (no page reload).
 * Called by JavaScript when user clicks Take/Skip buttons.
 */

/**
 * AJAX: Mark a medication dose as taken.
 *
 * @param int $med_id
 *   The medication ID.
 */
function medremind_take_dose_ajax($med_id) {
  drupal_json_output(array(
    'status' => 'ok',
    'action' => 'taken',
    'med_id' => $med_id,
  ));
  // ↑ drupal_json_output() does two things:
  //   1. Sets header Content-Type: application/json
  //   2. Encodes the array as JSON and prints it
}

/**
 * AJAX: Mark a medication dose as skipped.
 */
function medremind_skip_dose_ajax($med_id) {
  drupal_json_output(array(
    'status' => 'ok',
    'action' => 'skipped',
    'med_id' => $med_id,
  ));
}