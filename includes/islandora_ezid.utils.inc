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
  if (db_table_exists('islandora_ezid_arks') && count_arks() > 0) {
    watchdog('ezid_uninstall', 'Exporting ARKs before dropping db table.');

    $fs_path = drupal_get_path('module', 'islandora_ezid');
    $ark_file = fopen($fs_path . '/exports/arks-' . time() . '.csv', 'a');

    // Get everything from the database table we created at install.
    $result = db_query("SELECT * FROM islandora_ezid_arks");

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

    drupal_write_record('islandora_ezid_arks', $record, 'id');
  }
  // If we don't have a primary key, we're just reserving this ARK.
  else {
    $record->ark = $ark;
    $record->date_reserved = $curr_date;
    /* don't need to set 'state'; reserved (i.e., 0) is the default */

    if ($debug) {
      $message = 'Database record (@id) insert: ' . print_r($record, TRUE);
      watchdog('ezid', $message, array('@id' => $key), WATCHDOG_DEBUG);
    }

    drupal_write_record('islandora_ezid_arks', $record);
  }
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
 * Counts how many ARKs have been reserved (minted but not yet assigned).
 *
 * @return int
 *   The number of ARKs that are reserved for later use.
 */
function count_reserved_arks() {
  $query = 'SELECT COUNT(*) FROM islandora_ezid_arks WHERE state = 0';
  return db_query($query)->fetchField(); /* How many reserved in cache? */
}

/**
 * Counts how many ARKs are in the local database.
 *
 * @return int
 *   The number of ARKs in the local database.
 */
function count_arks() {
  $query = 'SELECT COUNT(*) FROM islandora_ezid_arks';
  return db_query($query)->fetchField(); /* How many reserved or assigned */
}

/**
 * Counts how many assigned ARKs are in the local database.
 *
 * @return int
 *   The number of assigned ARKs in the local database.
 */
function count_assigned_arks() {
  $query = 'SELECT COUNT(*) FROM islandora_ezid_arks WHERE state = 1';
  return db_query($query)->fetchField(); /* How many ARKs are assigned? */
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

/**
 * Returns the VID for the EZID ARK tag vocubulary.
 */
function get_ezid_tag_vid() {
  // Checks machine name first but then checks variable in case of renaming.
  $vid_query = "SELECT vid FROM taxonomy_vocabulary WHERE machine_name = ?";
  $vid = db_query($vid_query, array('ezid_ark_tags'))->fetchObject()->vid;
  return isset($vid) ? $vid : variable_get('EZID_VOCAB_VID');
}

/**
 * Uninstalls the EZID tag vocabulary.
 */
function remove_ezid_vocabulary() {
  $vid = get_ezid_tag_vid();

  // First, test that we can find the EZID tag vocabulary.
  if (isset($vid)) {
    $vocab_terms = taxonomy_get_tree($vid);

    // Then, go through and delete all the assigned tags.
    foreach ($vocab_terms as $term) {
      taxonomy_term_delete($term->tid);
    }

    // After that, we can delete the tag vocabulary itself.
    if (taxonomy_vocabulary_delete($vid) == SAVED_DELETED) {
      watchdog('ezid_uninstall', 'Tag vocabulary ' . $vid . ' deleted');
      variable_del('EZID_VOCAB_VID');
    }
    else {
      $message = 'Unable to delete EZID tag vocabulary; VID: ' . $vid;
      watchdog('ezid_uninstall', $message, NULL, WATCHDOG_WARNING);
    }
  }
  else {
    $message = 'Unable to find EZID_VOCAB_VID to remove EZID tag vocabulary.';
    watchdog('ezid_uninstall', $message, NULL, WATCHDOG_WARNING);
  }
}
