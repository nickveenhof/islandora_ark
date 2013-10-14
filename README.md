# islandora_ezid

An [EZID](http://n2t.net/ezid/ "EZID")-based Archival Resource Key ([ARK](http://en.wikipedia.org/wiki/Archival\_Resource\_Key "'Archival Resource Key' on Wikipedia")) management interface for [Islandora](http://islandora.ca/ "Islandora website") and/or [Drupal](https://drupal.org/ "Drupal website").  This module wraps the [UC3](http://www.cdlib.org/services/uc3/ "University of California Curation Center") EZID service in a Drupal environment and seeks to provide native support for ARKs in Drupal and Islandora.

### Introduction

There will be several stages to this project.  It's a work in progress.  This document describes where the project is now and where it wants to go in the future.

At this point, a configurable number of ARKs are "reserved" and stored in a local database "cache" via a regular cron job.  There is also an administrative user interface through which batch jobs can be run.  A user may request a specific number of ARKs to be "assigned".  If that request can be satisfied by the ARKs in the database cache, a spreadsheet of assigned ARKs is returned.  If that number exceeds the number of ARKs in the cache, then a cron job is scheduled and an email is sent to the user after the scheduled job has been completed.  The email will contain a hypertext link to a downloadable spreadsheet of newly assigned ARKs.

There is also a web service from which external programs can request an ARK.  It has its own syntax (e.g., just returning a plain text ARK) and it can, alternatively, mimic the response of Noid (the most popular ARK minter).

The next step is to assign ARKs to newly ingested Islandora and Drupal records that don't have them and to ensure that newly ingested records that do have ARKs are registered with EZID.  EZID keeps metadata about each record that has an ARK (and provides its own search and management interface, of course).

### Installation

Right now, installation is done through a checkout of the code from GitHub.  A more standard Drupal way (i.e., a downloadable tar.gz) will be available once the project approaches a more stable state.  Currently, there is also a `reload_mod` Bash script which will completely uninstall and reinstall the module (dropping old database tables, etc.).  It requires Drush be installed.

### Configuration and Use

For configuration assistance, please consult the project's [website](http://projects.freelibrary.info/islandora_ezid/configuring.html "How to configure Islandora EZID").  Information about how to use islandora_ezid can also be found on the project's [website](http://projects.freelibrary.info/islandora_ezid/using.html "How to use Islandora EZID").

### Development Tips

If you want to develop the islandora\_ezid code yourself, you'll probably notice there is an Ant _and_ Maven build script.  You can use whichever you prefer:

    ant build

or

    mvn install

Both will run the tests and a perform a code analysis.  If there are code style problems with the code, the build will fail with a notice about the changes that need to be made.  The only difference between the two build options is that Maven is also used to build and deploy the project's website (using `mvn site` and `mvn site:deploy`).  For what it's worth, if you choose to use Maven, you still need to have Ant installed on your system.

### Project Status

[![Build Status](https://travis-ci.org/ksclarke/islandora\_ezid.png?branch=master)](https://travis-ci.org/ksclarke/islandora\_ezid) [Not activated yet]

### License

The BSD-3-Clause license used by the University of California. See [LICENSE](https://raw.github.com/ksclarke/islandora_ezid/master/LICENSE "Project license") file.

### Contact

If you have questions about islandora\_ezid feel free to contact me: Kevin S. Clarke (ksclarke@gmail.com).

If you encounter a problem or have a feature to suggest, submit it to the [issue queue](https://github.com/ksclarke/islandora_ezid/issues "GitHub Issue Queue").
