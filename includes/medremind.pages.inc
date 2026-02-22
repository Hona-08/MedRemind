<?php

/**
 * @file
 * Page callbacks for MedRemind.
 * Functions that generate page content.
 * Loaded ONLY when the page is visited (good for performance).
 */

/**
 * Dashboard page — shows at /medremind
 * Loads user's medications and renders the dashboard template.
 */
function medremind_dashboard_page() {
  global $user;
  // ↑ $user = current logged-in user
  //   $user->uid = user ID number

  // Try to load medications from database
  $medications = array();
  $stats = array(
    'total' => 0,     // Total medications
    'taken' => 0,     // Doses taken today
    'missed' => 0,    // Doses missed today
    'streak' => 0,    // Current best streak
  );

  // Check if our database table exists before querying
  if (db_table_exists('medremind_medications')) {
    // ↑ db_table_exists() checks if table was created
    //   Prevents errors if schema wasn't installed yet

    // Get all active medications for current user
    $result = db_select('medremind_medications', 'm')
      // ↑ db_select('table_name', 'alias')
      //   'alias' = short name to use in query (m = medications)
      //   Like: SELECT ... FROM medremind_medications m
      ->fields('m')
      // ↑ ->fields('m') = SELECT m.* (get all columns)
      //   You can also specify: ->fields('m', array('name', 'dosage'))
      ->condition('m.uid', $user->uid)
      // ↑ ->condition('column', 'value') = WHERE m.uid = current user
      //   Only get THIS user's medications
      ->condition('m.status', MEDREMIND_STATUS_ACTIVE)
      // ↑ Only get active medications (not paused)
      //   Uses our constant instead of magic number 1
      ->orderBy('m.created', 'DESC')
      // ↑ Newest medications first
      ->execute();
      // ↑ ->execute() runs the query and returns results

    // Loop through results
    foreach ($result as $row) {
      $medications[] = $row;
      // ↑ Add each medication to our array
      //   $row is an object with properties: $row->name, $row->dosage, etc.
    }

    $stats['total'] = count($medications);

    // Count today's taken doses
    if (db_table_exists('medremind_log')) {
      $today_start = strtotime('today');
      // ↑ strtotime('today') = timestamp for midnight today (00:00:00)

      $stats['taken'] = db_select('medremind_log', 'l')
        ->condition('l.uid', $user->uid)
        ->condition('l.action', MEDREMIND_ACTION_TAKEN)
        ->condition('l.action_time', $today_start, '>=')
        // ↑ Third parameter '>=' means: WHERE action_time >= today_start
        //   So only count doses from today
        ->countQuery()
        // ↑ ->countQuery() = SELECT COUNT(*) instead of SELECT *
        //   Returns the number of rows, not the rows themselves
        ->execute()
        ->fetchField();
        // ↑ ->fetchField() gets single value from result
    }
  }

  // Render the dashboard template
  return theme('medremind_dashboard', array(
    'medications' => $medications,
    'stats' => $stats,
    'user_name' => format_username($user),
    // ↑ theme('template_name', variables_array)
    //   Renders templates/medremind-dashboard.tpl.php
    //   Passes our data as variables to the template
  ));
}

/**
 * History page — shows at /medremind/history
 * Displays dose log and adherence stats.
 */
function medremind_history_page() {
  global $user;

  $logs = array();
  $adherence = 0;
  $month = format_date(REQUEST_TIME, 'custom', 'F Y');
  // ↑ REQUEST_TIME = timestamp when page was requested
  //   format_date with 'F Y' = "February 2026"

  // Load this month's dose logs
  if (db_table_exists('medremind_log')) {
    $month_start = strtotime('first day of this month midnight');
    // ↑ Gets timestamp for start of current month

    $result = db_select('medremind_log', 'l')
      ->fields('l')
      ->condition('l.uid', $user->uid)
      ->condition('l.scheduled_time', $month_start, '>=')
      ->orderBy('l.action_time', 'DESC')
      ->range(0, 50)
      // ↑ ->range(0, 50) = LIMIT 50 (only get last 50 entries)
      //   First parameter = offset, second = count
      ->execute();

    foreach ($result as $log) {
      // Get medication name for each log entry
      if (db_table_exists('medremind_medications')) {
        $log->med_name = db_query(
          'SELECT name FROM {medremind_medications} WHERE med_id = :mid',
          array(':mid' => $log->med_id)
        )->fetchField();
        // ↑ db_query() for simple queries with placeholders
        //   :mid = safe placeholder (prevents SQL injection)
      }

      if (empty($log->med_name)) {
        $log->med_name = t('Unknown');
      }

      $logs[] = $log;
    }

    // Calculate adherence percentage
    $total_scheduled = db_select('medremind_log', 'l')
      ->condition('l.uid', $user->uid)
      ->condition('l.scheduled_time', $month_start, '>=')
      ->countQuery()
      ->execute()
      ->fetchField();

    $total_taken = db_select('medremind_log', 'l')
      ->condition('l.uid', $user->uid)
      ->condition('l.action', MEDREMIND_ACTION_TAKEN)
      ->condition('l.scheduled_time', $month_start, '>=')
      ->countQuery()
      ->execute()
      ->fetchField();

    if ($total_scheduled > 0) {
      $adherence = round(($total_taken / $total_scheduled) * 100);
      // ↑ round() rounds to nearest whole number
      //   Example: 8 taken / 10 scheduled = 80%
    }
  }

  // Render the history template
  return theme('medremind_history', array(
    'logs' => $logs,
    'month' => $month,
    'adherence' => $adherence,
  ));
}