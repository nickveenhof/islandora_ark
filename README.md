# islandora_ezid

An EZID Archival Resource Key (ARK) management interface for Islandora.

### Introduction

A rough plan, sure to be revised and revised again...

Stage 1: An administrative UI for batch ARK reservations.  []
Stage 2: An administrative UI for managing/minting reserved ARKs.  []
Stage 3: An object page redirect for ARKs.  []
Stage 4: A resolution mechanism for having ARKs in system URLs.  []

*Open Questions:*

* Allow more than one shoulder to be minted and managed?
* How to do "Stage 4" generically? A different module?  Or Pathauto + something custom for ARKs?
* Multisite vs. single Drupal.  Some way for this to sit at root and work with multisites?

db_table: ark, url/path, title, author, collection, unused-0/reserved-1/minted-2

### Project Status

[![Build Status](https://travis-ci.org/ksclarke/islandora\_ezid.png?branch=master)](https://travis-ci.org/ksclarke/islandora\_ezid) [Not activated yet]

### License

The BSD-3-Clause license used by the University of California. See LICENSE file.

### Contact

If you have questions about islandora\_ezid feel free to contact me: Kevin S. Clarke (ksclarke@gmail.com).

If you encounter a problem or have a feature to suggest, submit it to the [issue queue](https://github.com/ksclarke/islandora_ezid/issues "GitHub Issue Queue").
