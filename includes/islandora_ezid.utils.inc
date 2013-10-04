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
 *   Starting tag (could be <pre>)
 * @param string $end
 *   Ending tag (could be </pre>)
 */
function print_debug($variable, $text = '', $start = '', $end = '') {
  drupal_set_message($start . $text . ' ' . print_r($variable, TRUE) . $end);
}

class EZID {

  /**
   * EZID service base URL string.
   *
   * @var string
   * @access public
   */
  public $url;

  /**
   * EZID user account username.
   *
   * @var string
   * @access public
   */
  public $username;

  /**
   * EZID user account password.
   *
   * @var string
   * @access public
   */
  public $password;

  /**
   * EZID cookie provided after a successful login.
   *
   * @var string
   * @access public
   */
  public $cookie;

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
  public function __construct($service, $username, $password, $debug = FALSE) {
    $url = $this->endsWith($service, '/') ? rtrim($service, '/') : $service;

    $this->url = $url;
    $this->username = $username;
    $this->password = $password;
    $this->debug = $debug;

    $auth_data = $username . ':' . (isset($password) ? $password : '');
    $headers = array('Authorization' => 'Basic ' . base64_encode($auth_data));
    $options = array('headers' => $headers);

    if ($debug) {
      $ssl_version = curl_version()['ssl_version'];

      print_debug('Connecting to: ' . check_url($url . '/login'));

      if (isset($ssl_version)) {
        print_debug('Using SSL version: ' . $ssl_version);
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
        print_debug('Received EZID cookie: ' . $this->cookie);
      }
    }
  }

  /**
   * Destructor for EZID object.
   */
  public function __destruct() {
    var_dump($this->url);
    var_dump($this->username);
    var_dump($this->password);
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
  protected function endsWith($str, $substr) {
    return $substr === "" || substr($str, -strlen($substr)) === $substr;
  }
}
