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
 * 
 * Checks machine name first, but then checks variable in case of renaming.
 */
function get_ezid_tag_vid() {
  $vid = taxonomy_vocabulary_get_names()['ezid_ark_tags']->vid;
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

/**
 * Reads a directory and returns a list of files representing jobs to be run.
 * 
 * @param bool $debug
 *   Whether debugging is turned on.
 * 
 * @return array
 *   Returns the batch jobs waiting to be run.
 */
function read_cron_jobs($debug) {
  $dir = drupal_realpath(file_build_uri('cron-arks'));
  $jobs = array();

  foreach (scandir($dir) as $file_name) {
    $file = $dir . '/' . $file_name;

    if (is_file($file)) {
      if ($debug) {
        watchdog('ezid_cron', 'Loading cron job config file: ' . $file);
      }

      $settings = parse_ini_file($file);
      $settings['file_name'] = $file_name;
      $settings['file'] = $file;
      $jobs[] = $settings;
    }
  }

  return $jobs;
}

/**
 * Returns an array from a correctly formatted, comma delimited string of IPs.
 * 
 * @param string $value
 *   A correctly formatted, comma delimited string of IP addresses.
 *
 * @return array
 *   An array of IP addresses in string form.
 */
function to_ip_array($value) {
  $ips = array();

  foreach (explode(',', $value) as $ip) {
    $ips[] = trim($ip);
  }

  return $ips;
}

/**
 * Makes an ARK look like an ARK from Noid; for example: "id: 99999/fk40p33bq".
 * 
 * @param string $ark
 *   An ARK in the form: "ark:/99999/fk40p33bq".
 *
 * @return string
 *   An ARK in the form: "id: 99999/fk40p33bq"; returns 'ERROR' on error.
 */
function as_noid($ark) {
  if (strpos($ark, 'ark:/') === 0) {
    return 'id: ' . substr($ark, 5);
  }
  else {
    return 'ERROR';
  }
}

/**
 * Assigns ARKs that have been previously reserved.
 * 
 * This handles assignments that are done on-the-fly as a Web user waits and
 * assignments that are run via the crontask (for jobs that were too large to
 * complete on-the-fly).
 * 
 * @param array $config
 *   The configuration options used to assign ARKs.
 */
function assign_arks($config) {
  // Seconds are the most granular timestamp we can use unfortunately.
  $date = date("Y-m-d H:i:s");
  $count = $config['count'];
  $email = $config['email'];
  $debug = $config['debug'];
  $label = $config['label'];

  // This isn't database independent... it's using the MySQL LIMIT syntax.
  // The PostgreSQL syntax would be: LIMIT $count (without a range start).
  // TODO: Support different/configurable queries for different databases?
  $sql = 'UPDATE islandora_ezid_arks SET state = 1, date_assigned = :DATE, ' .
      'date_lastupdate = :DATE WHERE id IN (SELECT id FROM (SELECT id FROM ' .
      'islandora_ezid_arks WHERE state = 0 ORDER BY id LIMIT 0, ' . $count .
      ') tmp)';
  $result = db_query($sql, array(':DATE' => $date));

  $dir_path = file_build_uri('arks');
  $file_name = $config['file_name'];
  $file_path = $dir_path . '/' . $file_name;

  if ($debug && isset($email)) {
    $message = 'Assigning @count ARKs from a scheduled cron job: @file';
    $details = array('@count' => $count, '@file' => $file_name);
    watchdog('ezid', $message, $details, WATCHDOG_DEBUG);
  }

  // Open our ARK file.
  $ark_file = fopen($file_path, 'a');

  if ($label) {
    $labels = array(
      'ARK',  'URL', 'TAGS', 'CREATOR', 'TITLE', 'PUBLISHER', 'DATE', 'TYPE',
    );

    fputcsv($ark_file, $labels);
  }

  // Pulls those records we just updated so we can output their ARKs to CSV.
  $query = 'SELECT ark FROM islandora_ezid_arks WHERE date_assigned = ? ';
  $result = db_query_range($query . 'ORDER BY id', 0, $count, array($date));

  foreach ($result as $record) {
    if (isset($config['tags']) && $config['tags'] != 'COLLECTION_NAME') {
      // The empty string below is for URL (that should be supplied later).
      fputcsv($ark_file, array($record->ark, '', $config['tags']));
    }
    else {
      // This would need a URL, but we don't have to leave a space for it.
      fputcsv($ark_file, array($record->ark));
    }
  }

  // Close our ARK file.
  fclose($ark_file);

  // If we have an email that means we're processing a scheduled cron job.
  if (isset($email)) {
    if ($debug) {
      $message = 'Emailing @user the results of an ARK batch job: @file';
      $details = array('@user' => $email, '@file' => $file_name);
      watchdog('ezid_cron', $message, $details);
    }

    $httplink = file_create_url('public://arks/' . $file_name);
    $params = array('link' => $httplink, 'file_name' => $file_name);
    drupal_mail('islandora_ezid', 'cron', $email, language_default(), $params);

    $dir_path = file_build_uri('cron-arks');
    $file_path = $dir_path . '/' . $file_name;

    if (!unlink($file_path)) {
      $message = "Couldn't delete ARK cron job scheduling file: " . $file_path;
      watchdog('ezid_cron', $message, NULL, WATCHDOG_WARNING);
    }
  }

  // We only need this to display to website users waiting on their batch job.
  return $file_path;
}
