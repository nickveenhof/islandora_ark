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
 * Tests whether a supplied variable is a whole number.
 * 
 * @param unknown $variable
 *   A variable to test to determine if it's a whole number.
 *
 * @return boolean
 *   True if the supplied number is a boolean; else, false.
 */
function is_whole_number($variable){
  return (is_numeric($variable) && (intval($variable) == floatval($variable)));
}

/**
 * Counts have many ARKs have been reserved (minted but not yet assigned).
 * 
 * @return int
 *   The number of ARKs that are reserved for later use.
 */
function count_reserved_arks() {
  $query = 'SELECT COUNT(*) FROM {islandora_ezid_arks} WHERE state = 0';
  return db_query($query)->fetchField(); /* How many reserved in cache? */
}
