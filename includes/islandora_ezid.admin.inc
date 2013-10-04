<?php
/**
 * @file
 * islandora_ezid.admin.inc
 */

// Pulls in the EZID class which handles our connection to EZID.
module_load_include('inc', 'islandora_ezid', 'includes/islandora_ezid.utils');

/**
 * Admin form for the Islandora EZID module.
 *
 * @return array
 *   Returns The admin form.
 */
function islandora_ezid_admin() {
  $form = array();

  // Get the EZID settings.
  $settings = islandora_ezid_get_settings();

  // Setting tree to TRUE so we parse the hierarchy of islandora_ezid_settings
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
    '#description' => t('Toggles whether logging messages should be output.'),
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

  // Get the EZID settings.
  $settings = islandora_ezid_get_settings();
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
  $debug = $form['islandora_ezid_settings']['debugMode']['#value'];
  $password = $element['#value'];

  if (empty($password)) {
    form_error($element, t('This field is required.'));
  }
  else {
    try {
      new EZID($base_url, $username, $password, $debug);
    }
    catch (Exception $details) {
      form_error($element, $details->getMessage());
    }
  }
}
