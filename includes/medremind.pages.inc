<?php

/**
 * @file
 * Page callbacks for MedRemind.
 * These functions generate the page content.
 * Separated from .module to keep things organized.
 * Drupal loads this file ONLY when these pages are visited.
 */

/**
 * Dashboard page — shows at /medremind
 */
function medremind_dashboard_page() {
  global $user;
  // ↑ Get current logged-in user

  $output = '<div class="medremind-welcome">';

  $output .= '<div class="medremind-logo">💊</div>';

  $output .= '<h2>' . t('Welcome to MedRemind, @name!', array(
    '@name' => format_username($user),
    // ↑ t() with @name = safe placeholder that auto-escapes HTML
    //   format_username() = Drupal function to get display name safely
  )) . '</h2>';

  $output .= '<p>' . t('Track your medications, never miss a dose.') . '</p>';

  $output .= '<p>' . l(t('+ Add Medication'), 'medremind/add', array(
    'attributes' => array(
      'class' => array('medremind-add-btn'),
    ),
    // ↑ l() = Drupal's link function: l(text, path, options)
    //   ALWAYS use l() for internal links, never write raw <a href>
    //   It handles base path, clean URLs, active classes automatically
  )) . '</p>';

  $output .= '</div>';

  return $output;
  // ↑ Whatever we return becomes the main content area of the page
  //   Drupal wraps it with the site theme (header, sidebar, footer)
}

/**
 * History page — shows at /medremind/history
 */
function medremind_history_page() {
  $output = '<div class="medremind-welcome">';
  $output .= '<h2>' . t('Medication History') . '</h2>';
  $output .= '<p>' . t('Your dose history and streaks will appear here.') . '</p>';
  $output .= '</div>';

  return $output;
}