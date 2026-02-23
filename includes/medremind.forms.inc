<?php

/**
 * @file
 * All form definitions for MedRemind.
 * Contains add/edit medication form, delete confirmation,
 * and user settings form.
 *
 * Drupal Form API works in 3 steps:
 *   1. BUILD   → function medremind_medication_form()
 *   2. VALIDATE → function medremind_medication_form_validate()
 *   3. SUBMIT   → function medremind_medication_form_submit()
 *
 * Drupal auto-discovers _validate and _submit by naming convention!
 * If your form is called "myform", Drupal looks for:
 *   myform_validate() and myform_submit() automatically.
 */


// ============================================
// ADD / EDIT MEDICATION FORM
// ============================================

/**
 * Form builder: Add or Edit a medication.
 *
 * @param array $form
 *   Empty form array that Drupal gives us to build on.
 * @param array &$form_state
 *   Form state (passed by reference with &).
 *   Contains submitted values, rebuild flags, etc.
 * @param int|null $med_id
 *   NULL = adding new medication.
 *   Number = editing existing medication with this ID.
 */
function medremind_medication_form($form, &$form_state, $med_id = NULL) {
  global $user;

  // If editing, load the existing medication data
  $medication = NULL;
  if ($med_id) {
    $medication = db_select('medremind_medications', 'm')
      ->fields('m')
      ->condition('m.med_id', $med_id)
      ->condition('m.uid', $user->uid)
      // ↑ Security: only load if current user owns it
      ->execute()
      ->fetchObject();
      // ↑ fetchObject() returns one row as an object
      //   $medication->name, $medication->dosage, etc.

    if (!$medication) {
      drupal_set_message(t('Medication not found.'), 'error');
      // ↑ drupal_set_message() shows colored alert on page
      //   'error' = red box, 'warning' = yellow, 'status' = green
      drupal_goto('medremind');
      // ↑ drupal_goto() redirects to another page
      return;
    }
  }

  // Store med_id in form for use in submit handler
  $form['med_id'] = array(
    '#type' => 'hidden',
    // ↑ hidden = invisible field, stores data without showing it
    //   Like <input type="hidden">
    '#value' => $med_id,
  );

  // --- Medication Name ---
  $form['name'] = array(
    '#type' => 'textfield',
    // ↑ textfield = single line text input
    //   Like <input type="text">
    '#title' => t('Medication Name'),
    // ↑ Label shown above the field
    '#description' => t('Enter the name of your medication (e.g., Paracetamol, Amoxicillin).'),
    // ↑ Small help text shown below the field
    '#required' => TRUE,
    // ↑ Required = Drupal auto-validates this is not empty
    //   Shows red asterisk (*) next to the label
    '#maxlength' => 255,
    // ↑ Max characters allowed
    '#default_value' => $medication ? $medication->name : '',
    // ↑ If editing: show existing value
    //   If adding: show empty
    //   This is the ternary operator: condition ? true_value : false_value
  );

  // --- Dosage ---
  $form['dosage'] = array(
    '#type' => 'textfield',
    '#title' => t('Dosage'),
    '#description' => t('Amount per dose (e.g., 500mg, 2 tablets, 5ml).'),
    '#required' => TRUE,
    '#maxlength' => 100,
    '#default_value' => $medication ? $medication->dosage : '',
  );

  // --- Frequency ---
  $form['frequency'] = array(
    '#type' => 'select',
    // ↑ select = dropdown menu
    //   Like <select><option>...</option></select>
    '#title' => t('Frequency'),
    '#description' => t('How often do you take this medication?'),
    '#required' => TRUE,
    '#options' => array(
      // ↑ Dropdown options: 'stored_value' => 'Display Text'
      'once_daily' => t('Once Daily'),
      'twice_daily' => t('Twice Daily'),
      'three_daily' => t('Three Times Daily'),
      'four_daily' => t('Four Times Daily'),
      'weekly' => t('Once a Week'),
      'as_needed' => t('As Needed'),
      'custom' => t('Custom'),
    ),
    '#default_value' => $medication ? $medication->frequency : 'once_daily',
  );

  // --- Dose Times ---
  $form['times_wrapper'] = array(
    '#type' => 'fieldset',
    // ↑ fieldset = groups related fields together
    //   Shows a box with a legend/title around the fields
    '#title' => t('Dose Times'),
    '#description' => t('Set the time(s) you need to take this medication.'),
    '#collapsible' => FALSE,
    // ↑ FALSE = always open
    //   TRUE = user can click to expand/collapse
  );

  // Parse existing times or set defaults
  $default_times = array('08:00');
  if ($medication && !empty($medication->times)) {
    $decoded = drupal_json_decode($medication->times);
    // ↑ drupal_json_decode() = PHP's json_decode()
    //   Converts JSON string '["08:00","20:00"]' to PHP array
    if (is_array($decoded)) {
      $default_times = $decoded;
    }
  }

  $form['times_wrapper']['time_1'] = array(
    '#type' => 'textfield',
    '#title' => t('Time 1 (Morning)'),
    '#description' => t('Use 24-hour format (e.g., 08:00, 14:30).'),
    '#size' => 10,
    // ↑ size = visual width of input field (not max characters)
    '#maxlength' => 5,
    '#default_value' => isset($default_times[0]) ? $default_times[0] : '08:00',
    '#required' => TRUE,
  );

  $form['times_wrapper']['time_2'] = array(
    '#type' => 'textfield',
    '#title' => t('Time 2 (Afternoon)'),
    '#description' => t('Leave empty if only once daily.'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => isset($default_times[1]) ? $default_times[1] : '',
  );

  $form['times_wrapper']['time_3'] = array(
    '#type' => 'textfield',
    '#title' => t('Time 3 (Evening)'),
    '#description' => t('Leave empty if not needed.'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => isset($default_times[2]) ? $default_times[2] : '',
  );

  $form['times_wrapper']['time_4'] = array(
    '#type' => 'textfield',
    '#title' => t('Time 4 (Night)'),
    '#description' => t('Leave empty if not needed.'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => isset($default_times[3]) ? $default_times[3] : '',
  );

  // --- Start Date ---
  $form['start_date'] = array(
    '#type' => 'date',
    // ↑ date = Drupal's date picker with day/month/year dropdowns
    //   Returns array: array('year' => 2026, 'month' => 2, 'day' => 18)
    '#title' => t('Start Date'),
    '#description' => t('When did you start (or will start) this medication?'),
    '#required' => TRUE,
    '#default_value' => $medication && $medication->start_date
      ? array(
          'year' => date('Y', $medication->start_date),
          'month' => date('n', $medication->start_date),
          // ↑ 'n' = month without leading zero (1-12)
          'day' => date('j', $medication->start_date),
          // ↑ 'j' = day without leading zero (1-31)
        )
      : array(
          'year' => date('Y'),
          'month' => date('n'),
          'day' => date('j'),
        ),
    // ↑ Default to today's date for new medications
  );

  // --- End Date (Optional) ---
  $form['has_end_date'] = array(
    '#type' => 'checkbox',
    // ↑ checkbox = single on/off toggle
    //   Returns 1 if checked, 0 if unchecked
    '#title' => t('This medication has an end date'),
    '#default_value' => ($medication && $medication->end_date > 0) ? 1 : 0,
  );

  $form['end_date'] = array(
    '#type' => 'date',
    '#title' => t('End Date'),
    '#description' => t('When should you stop taking this medication?'),
    '#default_value' => ($medication && $medication->end_date > 0)
      ? array(
          'year' => date('Y', $medication->end_date),
          'month' => date('n', $medication->end_date),
          'day' => date('j', $medication->end_date),
        )
      : array(
          'year' => date('Y'),
          'month' => date('n'),
          'day' => date('j'),
        ),
    '#states' => array(
      // ↑ #states = Drupal's conditional visibility (no JavaScript needed!)
      //   Shows/hides this field based on another field's value
      'visible' => array(
        ':input[name="has_end_date"]' => array('checked' => TRUE),
        // ↑ Only show end_date field when checkbox is checked
        //   Drupal handles the JavaScript automatically!
      ),
    ),
  );

  // --- Notes ---
  $form['notes'] = array(
    '#type' => 'textarea',
    // ↑ textarea = multi-line text input
    //   Like <textarea></textarea>
    '#title' => t('Special Instructions'),
    '#description' => t('Any notes (e.g., take with food, avoid sunlight, etc.).'),
    '#rows' => 3,
    // ↑ Number of visible text rows
    '#default_value' => $medication ? $medication->notes : '',
  );

  // --- Submit Button ---
  $form['submit'] = array(
    '#type' => 'submit',
    // ↑ submit = form submit button
    '#value' => $medication ? t('Update Medication') : t('Add Medication'),
    // ↑ Button text changes based on add vs edit
    '#attributes' => array(
      'class' => array('medremind-submit-btn'),
      // ↑ Add CSS class to the button for styling
    ),
  );

  // --- Cancel Link ---
  $form['cancel'] = array(
    '#markup' => ' ' . l(t('Cancel'), 'medremind', array(
      'attributes' => array('class' => array('medremind-cancel-btn')),
    )),
    // ↑ #markup = raw HTML element
    //   l() creates a link back to dashboard
  );

  return $form;
}


// ============================================
// FORM VALIDATION
// Function name = form_name + _validate
// Drupal calls this automatically before submit!
// ============================================

/**
 * Validation handler for medication form.
 * Runs BEFORE submit. If errors are set, form won't submit.
 */
function medremind_medication_form_validate($form, &$form_state) {

  $values = $form_state['values'];
  // ↑ $form_state['values'] contains all submitted form data
  //   $values['name'] = what user typed in the name field
  //   $values['dosage'] = what user typed in dosage field, etc.

  // Validate medication name length
  if (drupal_strlen($values['name']) < 2) {
    // ↑ drupal_strlen() = multi-byte safe string length
    //   Works correctly with non-English characters
    form_set_error('name', t('Medication name must be at least 2 characters.'));
    // ↑ form_set_error('field_name', 'message')
    //   Highlights the field in red
    //   Shows error message at top of form
    //   STOPS the form from submitting!
  }

  // Validate time format (HH:MM)
  $time_fields = array('time_1', 'time_2', 'time_3', 'time_4');
  foreach ($time_fields as $field) {
    $time_value = trim($values[$field]);
    // ↑ trim() removes whitespace from start and end

    if (!empty($time_value)) {
      // Check format with regex
      if (!preg_match('/^([01][0-9]|2[0-3]):[0-5][0-9]$/', $time_value)) {
        // ↑ preg_match() = regex matching
        //   ^         = start of string
        //   [01][0-9] = 00-19
        //   2[0-3]    = 20-23
        //   :         = colon separator
        //   [0-5][0-9]= 00-59
        //   $         = end of string
        //   Valid: 08:00, 14:30, 23:59
        //   Invalid: 25:00, 8:00, abc
        form_set_error($field, t('@field must be in HH:MM format (e.g., 08:00, 14:30).', array(
          '@field' => $form['times_wrapper'][$field]['#title'],
          // ↑ @field placeholder inserts the field title in the error message
        )));
      }
    }
  }

  // At least one time is required
  $has_time = FALSE;
  foreach ($time_fields as $field) {
    if (!empty(trim($values[$field]))) {
      $has_time = TRUE;
      break;
      // ↑ break = exit the loop early (we found at least one time)
    }
  }
  if (!$has_time) {
    form_set_error('time_1', t('At least one dose time is required.'));
  }

  // Validate end date is after start date
  if (!empty($values['has_end_date'])) {
    $start = mktime(0, 0, 0,
      $values['start_date']['month'],
      $values['start_date']['day'],
      $values['start_date']['year']
    );
    // ↑ mktime(hour, min, sec, month, day, year)
    //   Converts date parts to Unix timestamp

    $end = mktime(0, 0, 0,
      $values['end_date']['month'],
      $values['end_date']['day'],
      $values['end_date']['year']
    );

    if ($end <= $start) {
      form_set_error('end_date', t('End date must be after start date.'));
    }
  }

  // Check maximum medications limit
  if (empty($values['med_id'])) {
    // ↑ Only check limit when ADDING new (not editing)
    global $user;
    $max = variable_get('medremind_max_medications', 20);
    // ↑ variable_get('name', default_value)
    //   Gets the admin setting we stored in hook_install

    $current_count = db_select('medremind_medications', 'm')
      ->condition('m.uid', $user->uid)
      ->countQuery()
      ->execute()
      ->fetchField();

    if ($current_count >= $max) {
      form_set_error('name', t('You have reached the maximum of @max medications.', array(
        '@max' => $max,
      )));
    }
  }
}


// ============================================
// FORM SUBMIT
// Function name = form_name + _submit
// Runs ONLY if validation passes (no errors)
// ============================================

/**
 * Submit handler for medication form.
 * Saves data to database.
 */
function medremind_medication_form_submit($form, &$form_state) {
  global $user;

  $values = $form_state['values'];

  // Build times array from the 4 time fields
  $times = array();
  foreach (array('time_1', 'time_2', 'time_3', 'time_4') as $field) {
    $time = trim($values[$field]);
    if (!empty($time)) {
      $times[] = $time;
    }
  }

  // Convert start date to timestamp
  $start_timestamp = mktime(0, 0, 0,
    $values['start_date']['month'],
    $values['start_date']['day'],
    $values['start_date']['year']
  );

  // Convert end date to timestamp (0 if no end date)
  $end_timestamp = 0;
  if (!empty($values['has_end_date'])) {
    $end_timestamp = mktime(0, 0, 0,
      $values['end_date']['month'],
      $values['end_date']['day'],
      $values['end_date']['year']
    );
  }

  // Build the data array
  $data = array(
    'uid' => $user->uid,
    'name' => $values['name'],
    'dosage' => $values['dosage'],
    'frequency' => $values['frequency'],
    'times' => drupal_json_encode($times),
    // ↑ drupal_json_encode() converts PHP array to JSON string
    //   array('08:00', '20:00') → '["08:00","20:00"]'
    'start_date' => $start_timestamp,
    'end_date' => $end_timestamp,
    'notes' => $values['notes'],
    'status' => MEDREMIND_STATUS_ACTIVE,
    'updated' => REQUEST_TIME,
    // ↑ REQUEST_TIME = current timestamp (set once per request)
  );

  if ($values['med_id']) {
    // ---- UPDATING existing medication ----
    db_update('medremind_medications')
      // ↑ db_update('table_name') = UPDATE query
      ->fields($data)
      // ↑ ->fields(array) = SET column = value for each item
      ->condition('med_id', $values['med_id'])
      // ↑ WHERE med_id = this medication
      ->condition('uid', $user->uid)
      // ↑ Security: only update if user owns it
      ->execute();

    drupal_set_message(t('Medication %name updated successfully.', array(
      '%name' => $values['name'],
      // ↑ %name = placeholder wrapped in <em> tags automatically
      //   Shows as: Medication "Paracetamol" updated successfully.
    )));

    watchdog('medremind', 'Medication @name updated by user @uid.', array(
      '@name' => $values['name'],
      '@uid' => $user->uid,
    ));
    // ↑ watchdog() logs to Admin → Reports → Recent log messages
  }
  else {
    // ---- INSERTING new medication ----
    $data['created'] = REQUEST_TIME;
    // ↑ Set created timestamp only for new medications

    $med_id = db_insert('medremind_medications')
      // ↑ db_insert('table_name') = INSERT query
      ->fields($data)
      // ↑ ->fields(array) = columns and values to insert
      ->execute();
      // ↑ ->execute() returns the new auto-increment ID

    // Create schedule entries for each dose time
    foreach ($times as $time) {
      db_insert('medremind_schedule')
        ->fields(array(
          'med_id' => $med_id,
          'uid' => $user->uid,
          'dose_time' => $time,
          'days_of_week' => 'all',
          'status' => 1,
        ))
        ->execute();
    }

    // Create initial streak record
    db_insert('medremind_streaks')
      ->fields(array(
        'uid' => $user->uid,
        'med_id' => $med_id,
        'current_streak' => 0,
        'best_streak' => 0,
        'last_taken' => 0,
      ))
      ->execute();

    drupal_set_message(t('Medication %name added successfully!', array(
      '%name' => $values['name'],
    )));

    watchdog('medremind', 'New medication @name added by user @uid.', array(
      '@name' => $values['name'],
      '@uid' => $user->uid,
    ));
  }

  // Redirect back to dashboard after saving
  $form_state['redirect'] = 'medremind';
  // ↑ $form_state['redirect'] = where to go after form submits
  //   'medremind' = the dashboard page
}


// ============================================
// DELETE CONFIRMATION FORM
// Always confirm before deleting!
// ============================================

/**
 * Form builder: Confirm medication deletion.
 */
function medremind_delete_confirm_form($form, &$form_state, $med_id) {
  global $user;

  // Load medication to show its name in confirmation
  $medication = db_select('medremind_medications', 'm')
    ->fields('m', array('name', 'med_id'))
    ->condition('m.med_id', $med_id)
    ->condition('m.uid', $user->uid)
    ->execute()
    ->fetchObject();

  if (!$medication) {
    drupal_set_message(t('Medication not found.'), 'error');
    drupal_goto('medremind');
    return;
  }

  // Store med_id for submit handler
  $form['med_id'] = array(
    '#type' => 'hidden',
    '#value' => $medication->med_id,
  );

  // Use Drupal's built-in confirm_form helper
  return confirm_form(
    $form,
    // ↑ The form array with our hidden field

    t('Are you sure you want to delete %name?', array('%name' => $medication->name)),
    // ↑ Question shown to user

    'medremind',
    // ↑ Where to go if user clicks "Cancel"

    t('This will permanently delete this medication and all its dose history. This action cannot be undone.'),
    // ↑ Warning message

    t('Delete'),
    // ↑ Confirm button text

    t('Cancel')
    // ↑ Cancel button text
  );
  // ↑ confirm_form() is a Drupal helper that creates a nice
  //   confirmation page with warning, confirm, and cancel buttons
}

/**
 * Submit handler: Delete the medication.
 */
function medremind_delete_confirm_form_submit($form, &$form_state) {
  global $user;
  $med_id = $form_state['values']['med_id'];

  // Get name before deleting (for the success message)
  $name = db_query(
    'SELECT name FROM {medremind_medications} WHERE med_id = :mid',
    array(':mid' => $med_id)
  )->fetchField();

  // Delete from all related tables (order matters!)
  // Delete child records first, then parent

  // 1. Delete dose logs
  db_delete('medremind_log')
    // ↑ db_delete('table_name') = DELETE FROM table
    ->condition('med_id', $med_id)
    ->condition('uid', $user->uid)
    // ↑ Security: only delete current user's data
    ->execute();

  // 2. Delete schedules
  db_delete('medremind_schedule')
    ->condition('med_id', $med_id)
    ->condition('uid', $user->uid)
    ->execute();

  // 3. Delete reminders
  db_delete('medremind_reminders')
    ->condition('med_id', $med_id)
    ->condition('uid', $user->uid)
    ->execute();

  // 4. Delete streaks
  db_delete('medremind_streaks')
    ->condition('med_id', $med_id)
    ->condition('uid', $user->uid)
    ->execute();

  // 5. Delete the medication itself (last!)
  db_delete('medremind_medications')
    ->condition('med_id', $med_id)
    ->condition('uid', $user->uid)
    ->execute();

  drupal_set_message(t('Medication %name has been deleted.', array('%name' => $name)));

  watchdog('medremind', 'Medication @name (ID: @id) deleted by user @uid.', array(
    '@name' => $name,
    '@id' => $med_id,
    '@uid' => $user->uid,
  ));

  $form_state['redirect'] = 'medremind';
}


// ============================================
// USER SETTINGS FORM
// ============================================

/**
 * Form builder: User personal settings.
 */
function medremind_user_settings_form($form, &$form_state) {
  global $user;

  // Load existing settings or use defaults
  $settings = db_select('medremind_settings', 's')
    ->fields('s')
    ->condition('s.uid', $user->uid)
    ->execute()
    ->fetchObject();

  $form['reminder_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable email reminders'),
    '#description' => t('Receive email notifications before each dose.'),
    '#default_value' => $settings ? $settings->reminder_email : 1,
  );

  $form['reminder_before'] = array(
    '#type' => 'select',
    '#title' => t('Remind me before dose'),
    '#options' => array(
      5 => t('5 minutes before'),
      10 => t('10 minutes before'),
      15 => t('15 minutes before'),
      30 => t('30 minutes before'),
      60 => t('1 hour before'),
    ),
    '#default_value' => $settings ? $settings->reminder_before : 15,
  );

  $form['timezone'] = array(
    '#type' => 'select',
    '#title' => t('Your Timezone'),
    '#options' => system_time_zones(),
    // ↑ system_time_zones() = Drupal function that returns
    //   array of all timezones like 'Asia/Kathmandu' => 'Asia/Kathmandu'
    '#default_value' => $settings ? $settings->timezone : drupal_get_user_timezone(),
    // ↑ drupal_get_user_timezone() gets current user's timezone setting
  );

  $form['daily_summary'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send daily summary email'),
    '#description' => t('Get a daily email with your medication adherence summary.'),
    '#default_value' => $settings ? $settings->daily_summary : 1,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
    '#attributes' => array(
      'class' => array('medremind-submit-btn'),
    ),
  );

  return $form;
}

/**
 * Submit handler: Save user settings.
 */
function medremind_user_settings_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];

  // Check if settings row exists
  $exists = db_select('medremind_settings', 's')
    ->fields('s', array('uid'))
    ->condition('s.uid', $user->uid)
    ->execute()
    ->fetchField();

  $data = array(
    'uid' => $user->uid,
    'reminder_email' => $values['reminder_email'],
    'reminder_before' => $values['reminder_before'],
    'timezone' => $values['timezone'],
    'daily_summary' => $values['daily_summary'],
  );

  if ($exists) {
    // Update existing settings
    db_update('medremind_settings')
      ->fields($data)
      ->condition('uid', $user->uid)
      ->execute();
  }
  else {
    // Insert new settings row
    db_insert('medremind_settings')
      ->fields($data)
      ->execute();
  }

  drupal_set_message(t('Your settings have been saved.'));
}