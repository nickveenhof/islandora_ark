<?php

/**
 * @file
 * islandora_ezid.ezid.inc
 */

/**
 * An encapsulation of the EZID service's functionality.
 *
 * @author Kevin S. Clarke <ksclarke@gmail.com>
 */
class EZID {

  /**
   * EZID service base URL string.
   *
   * @var string
   * @access public
   */
  public $url;

  /**
   * EZID cookie provided after a successful login.
   *
   * @var string
   * @access public
   */
  public $cookie;

  /**
   * The NAAN that should be used to mint new ARKs.
   *
   * @var string
   * @access public
   */
  public $naan;

  /**
   * The shoulder that should be used to mint new ARKs.
   *
   * @var string
   * @access public
   */
  public $shoulder;

  /**
   * Whether debugging messages should be printed.
   *
   * @var boolean
   * @access public
   */
  public $debug;

  /**
   * Constructor for EZID object.
   *
   * @param string $service
   *   EZID service URL
   * @param string $username
   *   EZID username
   * @param string $password
   *   EZID password
   * @param bool $debug
   *   Whether debugging is on
   */
  public function __construct($service = NULL, $naan = NULL, $shoulder = NULL, 
      $username = NULL, $password = NULL, $debug = NULL) {
    $settings = islandora_ezid_get_settings();

    if (!isset($username)) {
      $username = $settings['ezidUsername'];
    }

    if (!isset($password)) {
      $password = $settings['ezidPassword'];
    }

    if (!isset($shoulder)) {
      $shoulder = $settings['ezidShoulder'];
    }

    if (!isset($service)) {
      $service = $settings['ezidService'];
    }

    if (!isset($debug)) {
      $debug = $settings['debugMode'];
    }

    if (!isset($naan)) {
      $naan = $settings['ezidNAAN'];
    }

    $url = $this->ends_with($service, '/') ? rtrim($service, '/') : $service;
    $auth_data = $username . ':' . (isset($password) ? $password : '');
    $headers = array('Authorization' => 'Basic ' . base64_encode($auth_data));
    $options = array('headers' => $headers);

    $this->url = $url;
    $this->naan = $naan;
    $this->debug = $debug;
    $this->shoulder = $shoulder;

    if ($debug) {
      $ssl_version = curl_version()['ssl_version'];

      $message = 'Connecting to: ' . check_url($url . '/login');
      watchdog('ezid', $message, NULL, WATCHDOG_DEBUG);

      if (isset($ssl_version)) {
        $message = 'Using SSL version: ' . $ssl_version;
        watchdog('ezid', $message, NULL, WATCHDOG_DEBUG);
      }
    }

    // PHP must be compiled with OpenSSL support for https URLs to work.
    $result = drupal_http_request(check_url($url . '/login'), $options);

    if ($result->code != 200) {
      if ($debug) {
        throw new Exception(t('Failed to login using credentials: ') .
            $username . ':' . $password . ' [' . $result->code . ': ' .
            $result->error . ']');
      }
      else {
        throw new Exception(t('Failed to login using those credentials'));
      }
    }
    else {
      $this->cookie = $result->headers['set-cookie'];

      if ($debug) {
        $message = 'Received EZID cookie: ' . $this->cookie;
        watchdog('ezid', $message, NULL, WATCHDOG_DEBUG);
      }
    }
  }

  /**
   * Reserves and caches an ARK for later use.
   */
  public function reserve_ark() {
    $reserve_path = "/shoulder/ark:/" . $this->naan . '/' . $this->shoulder;
    $request = check_url($this->url . $reserve_path);

    if ($this->debug) {
      $message = 'Requesting new ARK: ' . $request;
      watchdog('ezid', $message, NULL, WATCHDOG_DEBUG);
    }

    // What we send to EZID to get it to reserve a new ARK for us.
    $options = array(
      'headers' => array(
        'Cookie' => $this->cookie,
        // EZID wants 'Content-Type' to be set to 'text/plain' for POSTs.
        'Content-Type' => 'text/plain',
      ),
      'method' => 'POST',
      // We just want to reserve it, not make it publicy resolvable yet.
      'data' => '_status:reserved',
    );

    // Send our request off to EZID...
    $result = drupal_http_request($request, $options);

    // Check if response code is anything but 'Created'
    if ($result->code != 201) {
      if ($this->debug) {
        throw new Exception(t('Failed to reserve ark: ') . $reserve_path
            . ' [' . $result->code . ': ' . $result->error . ']');
      }
      else {
        throw new Exception(t('Failed to reserve ark: ') . $reserve_path);
      }
    }
    else {
      // Successful minting at EZID returns "success: ark:/..."; strip prefix.
      $ark = substr($result->data, 9);

      if ($this->debug) {
        watchdog('ezid', 'Reserved ARK: ' . $ark, NULL, WATCHDOG_DEBUG);
      }

      return $ark;
    }
  }

  /**
   * Searches for substring at the end of the supplied string.
   *
   * @param string $str
   *   A string in which to look for the supplied substring
   * @param string $substr
   *   A substring to look for at the end of the supplied string
   *
   * @return bool
   *   True if the substring is found at the end of the string; else, false
   */
  protected function ends_with($str, $substr) {
    return $substr === "" || substr($str, -strlen($substr)) === $substr;
  }
}
