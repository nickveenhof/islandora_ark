<?php

/**
 * @file
 * A simple Web test to confirm that Drupal is up and initialized.
 */

/**
 * Tests that Drupal's Web interface is up and functioning.
 */
class WebTest extends PHPUnit_Extensions_Selenium2TestCase {
  protected $captureScreenshotOnFailure = TRUE;

  /**
   * Sets up the test's default browser settings.
   */
  protected function setUp() {
    $this->setBrowser('firefox');
    $this->setBrowserUrl('http://localhost:8080/');
  }

  /**
   * Tests whether the Drupal instance has been successfully initialized.
   */
  public function testTitle() {
    $this->url('http://localhost:8080/');
    $this->assertEquals('Welcome to localhost | localhost', $this->title());
  }

  /*
  public function testHeaderContainerWrapperDisabled() {
  $this->url('http://localhost:8080/');
  $element = $this->byCssSelector('header.container-wrapper');
  $this->assertEquals('none', $element->css('display'));
  }*/
}
