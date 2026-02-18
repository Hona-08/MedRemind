<?php

/**
 * @file
 * Admin settings and reports for MedRemind.
 * Only users with 'administer medremind' permission can access these.
 */

/**
 * Form: Admin settings page.
 */
function medremind_admin_settings_form($form, &$form_state) {

  $form['info'] = array(
    '#markup' => '<p>' . t('MedRemind admin configuration will be available here.') . '</p>',
  );

  return $form;
}