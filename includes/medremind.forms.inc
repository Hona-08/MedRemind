<?php

/**
 * @file
 * All form definitions for MedRemind.
 * Forms for adding, editing, deleting medications and user settings.
 */

/**
 * Form: Add or Edit a medication.
 *
 * @param array $form
 *   The form array that Drupal builds on.
 * @param array &$form_state
 *   Form state, passed by reference (& means changes persist).
 * @param int $med_id
 *   Medication ID. NULL = adding new, number = editing existing.
 */
function medremind_medication_form($form, &$form_state, $med_id = NULL) {

  $form['info'] = array(
    '#markup' => '<div class="medremind-welcome"><h2>' . t('Add Medication') . '</h2>'
      . '<p>' . t('Medication form will be built in the next step.') . '</p></div>',
    // ↑ #markup = display raw HTML in a form (no input field)
  );

  return $form;
  // ↑ Always return the $form array
}

/**
 * Form: Confirm before deleting a medication.
 */
function medremind_delete_confirm_form($form, &$form_state, $med_id) {

  $form['info'] = array(
    '#markup' => '<p>' . t('Are you sure you want to delete this medication?') . '</p>',
  );

  return $form;
}

/**
 * Form: User personal settings (reminder preferences, timezone).
 */
function medremind_user_settings_form($form, &$form_state) {

  $form['info'] = array(
    '#markup' => '<div class="medremind-welcome"><h2>' . t('My Settings') . '</h2>'
      . '<p>' . t('Your preferences will be configurable here.') . '</p></div>',
  );

  return $form;
}