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
    '#description' => t("URL for the EZID service (really shouldn't change)."),
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
    '#description' => t('ARKs can be cached for quicker batch processing.'),
  );
  $form['islandora_ezid_settings']['allowedIPs'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed IPs'),
    '#element_validate' => array('ws_ip_validate'),
    '#default_value' => $settings['allowedIPs'],
    '#description' => t('IP addresses allowed to connect to the Web service.'),
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
    /* Password is encrypted using the key in your Drupal settings file. */
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

  $form['column_left'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('ezid-column-left'),
    ),
  );
  $form['column_right'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('ezid-column-right'),
    ),
  );

  $form['column_left']['reserved'] = array(
    '#type' => 'item',
    '#title' => t('Reserved ARKs:'),
    '#markup' => count_reserved_arks(),
  );
  $form['column_left']['assigned'] = array(
    '#type' => 'item',
    '#title' => t('Assigned ARKs:'),
    '#markup' => count_assigned_arks(),
  );
  $form['column_left']['total'] = array(
    '#type' => 'item',
    '#title' => t('Total ARK Count:'),
    '#markup' => count_arks(),
  );

  $form['column_right']['manage-display'] = array(
    '#type' => 'item',
    '#title' => t('ARKs'),
    '#markup' => 'Yada <em>yada</em> yada...',
  );

  return $form;
}

/**
 * Admin form for the Islandora ARK batch uploads.
 *
 * @return array
 *   Returns The ARK upload form.
 */
function islandora_ezid_upload() {
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
    '#element_validate' => array('reserved_count_validate'),
    '#default_value' => $settings['numberOfARKs'],
    '#description' => t('Number of ARKs we want to reserve in batch mode.'),
  );
  // An optional collection or tag with which we want to associate the ARKs.
  $form['arkTagsDefault'] = array(
    '#type' => 'textfield',
    '#title' => t('Tags'),
    '#default_value' => $settings['arkTagsDefault'],
    '#autocomplete_path' => 'ezid/tags/autocomplete',
    '#description' => t('Comma delimited tags can be used for ARK management.'),
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
  // Labels mode.
  $form['labelsMode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Labels mode'),
    '#default_value' => $settings['labelsMode'],
    '#description' => t('Toggles whether labels should be used in the output.'),
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
    '#value' => t('Update defaults'),
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

  // Resetting the form is used to change the default values in the database.
  // Users can't reset the form without supplying the password; that's okay...
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
  $file_name = $form['arkExportFileName']['#value'];
  $tags = $form['arkTagsDefault']['#value'];
  $total_count = count_reserved_arks();
  $count = $form['numberOfARKs']['#value'];
  $debug = $form['debugMode']['#value'];
  $use_labels = $form['labelsMode']['#value'];

  if ($count < $total_count) {
    $config = array(
      'label' => $use_labels,
      'file_name' => $file_name,
      'count' => $count,
      'debug' => $debug,
      'tags' => $tags,
    );

    // Assign the ARKs so they're no longer available for other records.
    $file = assign_arks($config);

    // Create a link so we can display the file for the user to download.
    $www_path = file_create_url($file);

    // Display the link at which the user can download the new ARKs file.
    $link = l(t('Download the ARKs in spreadsheet format'), $www_path);
    drupal_set_message(t('The batch has been run. ') . $link . '.');
  }
  else {
    global $user;

    // Save the job request in a place where the cron job can pick it up.
    $dir_path = file_build_uri('cron-arks');
    $file_path = $dir_path . '/' . $file_name;

    // Open our cron job scheduler file.
    $ark_file = fopen($file_path, 'a');

    // Write out the details of our job...
    fwrite($ark_file, 'email = ' . $user->mail . PHP_EOL);
    fwrite($ark_file, 'count = ' . $count . PHP_EOL);
    fwrite($ark_file, 'debug = ' . $debug . PHP_EOL);
    fwrite($ark_file, 'label = ' . $use_labels . PHP_EOL);

    if (isset($tags) && $tags != 'COLLECTION_NAME') {
      fwrite($ark_file, 'tags = ' . $tags . PHP_EOL);
    }

    // Close our cron job scheduler file.
    fclose($ark_file);

    if ($debug) {
      $details = array(
        '@count' => $count,
        '@totalcount' => $total_count,
        '@email' => $user->mail,
      );

      drupal_set_message(t("Requested number of ARKs (@count) exceeds the
          number of cached ARKs (@totalcount). The batch will be run and
          an email sent to @email when it's done.", $details));
    }
    else {
      drupal_set_message(t("Requested number of ARKs exceeds the number of
          cached ARKs. The batch will be run and an email sent to @email
          when it's done.", array('@email' => $user->mail)));
    }
  }
}

/**
 * Handles a submit from the EZID batch pane.
 */
function islandora_ezid_batch_reset($form, &$form_state) {
  // Don't need to do anything... the form gets reset to its original state.
  drupal_set_message('The batch job form was not submitted, but reset to its' .
      ' default values.');
}

/**
 * Handles a submit to reconfigure the defaults for EZID ARK batches.
 */
function islandora_ezid_batch_config($form, &$form_state) {
  $settings = islandora_ezid_batch_get_settings();
  $settings['numberOfARKs'] = $form['numberOfARKs']['#value'];
  $settings['debugMode'] = $form['debugMode']['#value'];
  $settings['labelsMode'] = $form['labelsMode']['#value'];

  variable_set('islandora_ezid_batch_settings', $settings);

  drupal_set_message("The batch job form's defaults have been reset.");
}

/**
 * Validates the comma delimited list of IPs.
 *
 * @param unknown $element
 *   The element associated with the allowedIPs variable.
 * @param unknown $form_state
 *   The current state of the admin form.
 * @param unknown $form
 *   A reference to the form itself.
 */
function ws_ip_validate($element, &$form_state, $form) {
  $ips = trim($element['#value']);

  if (isset($ips)) {
    foreach (explode(',', $ips) as $ip) {
      $ip = trim($ip);

      // TODO: Handle IPv6 addresses (check for colons and process differently).
      $parts = explode('.', $ip);

      if (count($parts) != 4) {
        form_error($element, t("Doesn't have four parts like an IPv4 address"));
      }
      else {
        foreach ($parts as $part) {
          if (!is_numeric($part)) {
            $details = array('@value' => $part);
            form_error($element, t("'@value' isn't a number", $details));
          }
          elseif (!($part >= 0 && $part <= 255)) {
            $details = array('@value' => $part);
            form_error($element, t("@value isn't in range 0 to 255", $details));
          }
        }
      }
    }
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
    form_error($element, t("This doesn't look like a five digit NAAN"));
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
  elseif (!is_whole_number($ark_count)) {
    form_error($element, t("Supplied value isn't a whole number."));
  }
  elseif ($ark_count > 10000) {
    /* TODO: 10000 is an arbitrary ceiling; should it be higher? lower? */
    form_error($element, t('Please select a number between 0 and 10000.'));
  }
}

/**
 * Validates the number of ARKs we're requesting to be reserved.
 */
function reserved_count_validate($element, &$form_state, $form) {
  $ark_count = $element['#value'];

  if (empty($ark_count)) {
    form_error($element, t('This field is required.'));
  }
  elseif (!is_whole_number($ark_count)) {
    form_error($element, t("Supplied value isn't a whole number."));
  }
}

/**
 * Validates an optional EZID ARK shoulder.
 */
function ezid_shoulder_validate($element, &$form_state, $form) {
  $shoulder = $element['#value'];

  /* TODO: Do we really want to enforce this convention?*/
  if (!preg_match("/^[a-z]*\d$/", $shoulder)) {
    form_error($element, t("Should be alpha chars terminated by a digit"));
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
