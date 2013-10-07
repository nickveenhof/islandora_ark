<?php
/**
 * @file
 * islandora_ezid.admin.inc
 */

// Pulls in the EZID class which handles our connection to EZID.
module_load_include('inc', 'islandora_ezid', 'includes/islandora_ezid.ezid');

// Pulls in some convenience utility functions.
module_load_include('inc', 'islandora_ezid', 'includes/islandora_ezid.utils');

/**
 * Admin form for the Islandora EZID module.
 *
 * @return array
 *   Returns The EZID admin form.
 */
function islandora_ezid_admin() {
  $form = array();

  // Get the EZID settings.
  $settings = islandora_ezid_get_settings();

  // Setting tree to TRUE so we parse the hierarchy of settings.
  $form['islandora_ezid_settings'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  // Base URL for EZID service (shouldn't change, but making it obvious).
  $form['islandora_ezid_settings']['ezidService'] = array(
    '#type' => 'textfield',
    '#title' => t('Service URL'),
    '#element_validate' => array('ezid_service_validate'),
    '#default_value' => $settings['ezidService'],
    '#description' => t("Base URL for the EZID service (shouldn't change)."),
  );
  // The NAAN for the institution that is minting ARKs through this service.
  $form['islandora_ezid_settings']['ezidNAAN'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization NAAN'),
    '#element_validate' => array('ezid_naan_validate'),
    '#default_value' => $settings['ezidNAAN'],
    '#description' => t("Your organization's Name Assigning Authority Number."),
  );
  // The 'shoulder' to be used for newly minted ARKs.
  $form['islandora_ezid_settings']['ezidShoulder'] = array(
    '#type' => 'textfield',
    '#title' => t('ARK Shoulder'),
    '#element_validate' => array('ezid_shoulder_validate'),
    '#default_value' => $settings['ezidShoulder'],
    '#description' => t("The 'shoulder' (or namespace) for newly minted ARKs."),
  );
  $form['islandora_ezid_settings']['cachedARKCount'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of cached ARKs'),
    '#element_validate' => array('ark_count_validate'),
    '#default_value' => $settings['cachedARKCount'],
    '#description' => t('ARKs can be cached in case EZID is ever unavailable.'),
  );
  // The username to use when interacting with EZID.
  $form['islandora_ezid_settings']['ezidUsername'] = array(
    '#type' => 'textfield',
    '#title' => t('EZID Username'),
    '#default_value' => $settings['ezidUsername'],
    '#description' => t('The username to use when interacting with EZID.'),
  );
  // The password to use when interacting with EZID.
  $form['islandora_ezid_settings']['ezidPassword'] = array(
    '#type' => 'password',
    '#title' => t('EZID Password'),
    '#element_validate' => array('ezid_userpass_validate'),
    '#description' => t('The password to use when interacting with EZID.'),
  );
  // Debug mode.
  $form['islandora_ezid_settings']['debugMode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),
    '#default_value' => $settings['debugMode'],
    '#description' => t('Toggles whether logging messages should be logged.'),
  );

  // The form's actions.
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#weight' => 1,
    '#submit' => array('islandora_ezid_admin_submit'),
  );

  return system_settings_form($form);
}

/**
 * Admin form for the Islandora ARK manager.
 *
 * @return array
 *   Returns The ARK manage form.
 */
function islandora_ezid_manage() {
  $form = array();
}

/**
 * Admin form for the Islandora EZID batch interface.
 *
 * @return array
 *   Returns The batch form.
 */
function islandora_ezid_batch() {
  $form = array();

  // Get the EZID batch settings.
  $settings = islandora_ezid_batch_get_settings();

  // Setting tree to TRUE so we parse the hierarchy of settings.
  $form['#tree'] = TRUE;

  // The number of ARKs we want to reserve in batch mode.
  $form['numberOfARKs'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of ARKs'),
    '#default_value' => $settings['numberOfARKs'],
    '#description' => t('Number of ARKs we want to reserve in batch mode.'),
  );
  // An optional collection name with which we want to associate the ARKs.
  $form['arkCollectionName'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection'),
    '#default_value' => $settings['arkCollectionName'],
    '#description' => t('An (optional) collection name for the ARKs.'),
  );
  // A name of the ARK file we want to download.
  $form['arkExportFileName'] = array(
    '#type' => 'textfield',
    '#title' => t('File name'),
    '#default_value' => $settings['arkExportFileName'] . time() . '.csv',
    '#description' => t('Name of the file in which to put the minted ARKs.'),
  );
  // Debug mode.
  $form['debugMode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),
    '#default_value' => $settings['debugMode'],
    '#description' => t('Toggles whether logging messages should be logged.'),
  );

  // The form's actions.
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit batch job'),
    '#weight' => -1,
    '#submit' => array('islandora_ezid_batch_submit'),
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#weight' => 1,
    '#submit' => array('islandora_ezid_batch_reset'),
  );
  $form['actions']['configure'] = array(
    '#type' => 'submit',
    '#value' => t('Reset default number'),
    '#weight' => 3,
    '#submit' => array('islandora_ezid_batch_config'),
  );

  return $form;
}

/**
 * Handles a submit from the EZID admin pane.
 */
function islandora_ezid_admin_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#id'];

  switch ($op) {
    case 'edit-reset':
      variable_del('islandora_ezid_settings');
      break;
  }
}

/**
 * Handles a submit from the EZID batch pane.
 */
function islandora_ezid_batch_submit($form, &$form_state) {
  /* cron_for_islandora_ezid(); */
  drupal_set_message("submit");
}

/**
 * Handles a submit from the EZID batch pane.
 */
function islandora_ezid_batch_reset($form, &$form_state) {
  // We don't need to do anything, form is reset to its original state.
  drupal_set_message("The batch job form was not submitted, but reset to its" .
      " default values.");
}

/**
 * Handles a submit from the EZID batch pane.
 */
function islandora_ezid_batch_config($form, &$form_state) {
  $settings = islandora_ezid_batch_get_settings();
  $settings['numberOfARKs'] = $form['numberOfARKs']['#value'];

  variable_set('islandora_ezid_batch_settings', $settings);

  drupal_set_message("The batch job form's defaults were reset to the below.");
}

/**
 * Validates a EZID service base URL.
 */
function ezid_service_validate($element, &$form_state, $form) {
  $url = $element['#value'];

  if (empty($url)) {
    form_error($element, t('This field is required.'));
  }
  else {
    $result = drupal_http_request(check_url($url . '/status'));

    if ($result->code != 200) {
      form_error($element, t("EZID isn't up or this is a bad service URL"));
    }
  }
}

/**
 * Validates a EZID registered NAAN by expected pattern.
 */
function ezid_naan_validate($element, &$form_state, $form) {
  $naan = $element['#value'];

  if (empty($naan)) {
    form_error($element, t('This field is required.'));
  }
  elseif (!preg_match("/^\d{5}$/", $naan)) {
    form_error($element, t("This doesn't look like a NAAN (5 digit number)"));
  }
}

/**
 * Validates the number of ARKs we've reserved for use in Islandora.
 */
function ark_count_validate($element, &$form_state, $form) {
  $ark_count = $element['#value'];

  if (empty($ark_count)) {
    form_error($element, t('This field is required.'));
  }
  elseif ($ark_count < 0 || $ark_count > 5000) {
    form_error($element, t("Please select a number between 0 and 5000"));
  }
}

/**
 * Validates an optional EZID ARK shoulder.
 */
function ezid_shoulder_validate($element, &$form_state, $form) {
  $shoulder = $element['#value'];

  /* TODO: Do we really want to enforce this convention?*/
  if (!preg_match("/^[a-z]*\d$/", $shoulder)) {
    form_error($element, t("Convention is alpha chars terminated by a digit"));
  }

  /* TODO: Support multiple shoulders through this one interface? */
}

/**
 * Validates a EZID service username and password.
 */
function ezid_userpass_validate($element, &$form_state, $form) {
  $base_url = $form['islandora_ezid_settings']['ezidService']['#value'];
  $username = $form['islandora_ezid_settings']['ezidUsername']['#value'];
  $shoulder = $form['islandora_ezid_settings']['ezidShoulder']['#value'];
  $debug = $form['islandora_ezid_settings']['debugMode']['#value'];
  $naan = $form['islandora_ezid_settings']['ezidNAAN']['#value'];
  $password = $element['#value'];

  if (empty($password)) {
    form_error($element, t('This field is required.'));
  }
  else {
    try {
      new EZID($base_url, $naan, $shoulder, $username, $password, $debug);
    }
    catch (Exception $details) {
      form_error($element, $details->getMessage());
    }
  }
}
