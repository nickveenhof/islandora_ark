<?php
/**
 * @file
 * This is the template file for the ezid object page.
 *
 * Here we build an html page using the variables passed in by
 * the islandora_ezid_preprocess_islandora_ezid
 * function.  Elements such as labels and buttons can be added here.
 */
$islandora_content = $islandora_object['ezid']->content;
$path = drupal_get_path('module', 'islandora_ezid');

drupal_add_css("$path/css/islandora_ezid.css");
drupal_add_js("$path/js/islandora_ezid.js");
?>

<div class="islandora-ezid-object islandora">
  <button id="ezid_image">Do something</button>
  <div class ="ezid_thumb">
   <!-- put thumbnail here using a php print statement -->
  </div>
  <div class="islandora-basic-image-content-wrapper clearfix">
    <?php if (isset($islandora_content)): ?>
      <div class="islandora-ezid-content">
        <!-- Put actual content here -->
      </div>
    <?php endif; ?>
  </div>
