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
function _debug($variable, $text = '', $start = '', $end = '') {
  drupal_set_message($start . $text . ' ' . print_r($variable, TRUE) . $end);
}

/**
 * A utility function to aid in output debugging information.
 *
 * @param unknown $var
 *   A variable to pass to print_r()
 * @param string $text
 *   Text used to label the variable output
 */
function _debug_pre($var, $text = '') {
  drupal_set_message('<pre>' . $text . ' ' . print_r($var, TRUE) . '</pre>');
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
function save_ark_record($ark, $key = NULL, $metadata = NULL, $debug = FALSE) {
  $curr_date = date("Y-m-d H:i:s");

  if (!isset($ark)) {
    throw new Exception(t('ARK supplied to save_ark() cannot be empty'));
  }

  // If we have metadata, add it to the database record.
  if (isset($metadata)) {
    if (is_array($metadata)) {
      $record = new stdClass();

      foreach ($metadata as $md_key => $md_value) {
        $record->$md_key = $md_value;
      }
    }
    elseif (is_object($metadata)) {
      $record = $metadata;
    }
    else {
      $message = t("Metadata supplied to save_ark() isn't an array or object");
      throw new Exception($message);
    }
  }
  else {
    $record = new stdClass();
  }

  // Update our record's last updated date.
  $record->date_lastupdate = $curr_date;

  // If we have a primary key, we're updating an existing record.
  if (isset($key)) {
    $record->id = $key;

    if ($debug) {
      $message = 'Database record (@id) update: ' . print_r($record, TRUE);
      watchdog('ezid', $message, array('@id' => $key), WATCHDOG_DEBUG);
    }
  }
  // If we don't have a primary key, we're just reserving this ARK.
  else {
    $record->ark = $ark;
    $record->date_reserved = $curr_date;
    $record->state = 0;

    if ($debug) {
      $message = 'Database record (@id) insert: ' . print_r($record, TRUE);
      watchdog('ezid', $message, array('@id' => $key), WATCHDOG_DEBUG);
    }
  }

  // Update the record if the 'id' is passed in; else, insert new record.
  drupal_write_record('islandora_ezid_arks', $record, 'id');
}

/**
 * Tests whether a supplied variable is a whole number.
 *
 * @param unknown $variable
 *   A variable to test to determine if it's a whole number.
 *
 * @return bool
 *   True if the supplied number is a bool; else, false.
 */
function is_whole_number($variable) {
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

/**
 * Removes a set of keys (and their values) from the supplied array.
 *
 * @param array $keys
 *   The things to be removed from the array.
 * @param array $array
 *   The array from which to remove the supplied keys.
 *
 * @return array
 *   The array with the keys removed.
 */
function remove_from_array($keys, $array) {
  foreach ($keys as $key) {
    if (($key = array_search($key, $array)) !== FALSE) {
      unset($array[$key]);
    }
  }

  return $array;
}
