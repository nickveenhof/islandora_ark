<?php

/**
 * @file
 * islandora_ezid.utils.inc
 */

/**
 * A utility function to aid in output debugging information.
 *
 * @param unknown $variable
 *   A variable to pass to print_r()
 * @param string $text
 *   Text used to label the variable output
 * @param string $start
 *   Starting tag (could be '<pre>')
 * @param string $end
 *   Ending tag (could be '</pre>')
 */
function print_debug($variable, $text = '', $start = '', $end = '') {
  drupal_set_message($start . $text . ' ' . print_r($variable, TRUE) . $end);
}

/**
 * A utility to export all the ARKs in the database to a CSV file.
 */
function export_arks() {
  if (db_table_exists('islandora_ezid_arks')) {
    $fs_path = drupal_get_path('module', 'islandora_ezid');
    $ark_file = fopen($fs_path . '/exports/arks-' . time() . '.csv', 'a');

    // Get everything from the database table we created at install.
    $result = db_query("SELECT * FROM {islandora_ezid_arks}");

    foreach ($result as $record) {
      fwrite($ark_file, print_r($record, TRUE) . "\n");
    }

    fclose($ark_file);
  }
}

/**
 * Saves an ARK to the local database.
 *
 * @param string $ark
 *   Newly minted ARK that should be kept in our local database as well
 */
function save_ark($ark, $key = NULL, $metadata = NULL) {
  $curr_date = date("Y-m-d H:i:s");

  if (isset($key)) {
    // Do stuff with metadata array...
  }
  else {
    $record = array(
      'ark' => $ark,
      'date_reserved' => $curr_date,
      'date_lastupdate' => $curr_date,
      'state' => 0,
    );
  }

  drupal_write_record('islandora_ezid_arks', $record);
}

/**
 * Runs a check on the number of cached ARKs in islandora_ezid.
 */
function cron_for_islandora_ezid() {
  $settings = islandora_ezid_get_settings();
  $username = $settings['ezidUsername'];
  $password = $settings['ezidPassword'];
  $shoulder = $settings['ezidShoulder'];
  $total = $settings['cachedARKCount'];
  $url = $settings['ezidService'];
  $debug = $settings['debugMode'];
  $naan = $settings['ezidNAAN'];

  $query = 'SELECT COUNT(*) FROM {islandora_ezid_arks} WHERE state = 0';
  $count = db_query($query)->fetchField(); /* How many reserved in cache? */
  $count = $total - $count; /* How many more reserveds do we need to add? */

  if ($debug) {
    $message = 'Need ' . $count . ' new ARKs; ';
    $message .= ($count > 0 ? 'reserving...' : 'resting...');
    watchdog('ezid_cron', $message, NULL, WATCHDOG_DEBUG);
  }

  // We have nothing more to do!
  if ($count == 0) {
    return;
  }

  try {
    $ezid = new EZID($url, $naan, $shoulder, $username, $password, $debug);

    for ($index = 1; $index <= $count; $index++) {
      save_ark($ezid->reserveARK());
    }

    if ($debug) {
      $message = 'Successfully reserved ' . $count . ' new ARKs';
      watchdog('ezid_cron', $message, NULL, WATCHDOG_DEBUG);
    }
  }
  catch (Exception $details) {
    watchdog('ezid_cron', 'Exception: ' . $details->getMessage());
  }
}
