# Configuring Islandora EZID

When you first install the Islandora EZID module, you'll want to go to the configuration page to configure it.  If you have the Islandora module installed, that page will be found at `/admin/islandora/ezid`.  If you don't have the Islandora module installed in your Drupal instance, that page will be found at `/admin/config/ezid`.

### The Default Settings Page

The first four settings you'll see on this page are: 1) **Service URL**, 2) **Organization NAAN**, 3) **ARK Shoulder**, and 4) **Number of ARKs Cached**.<br/><br/>

![Islandora EZID configuration, page 1](http://projects.freelibrary.info/islandora_ezid/images/ezid-admin-screenshot-1.png "Islandora EZID configuration, page 1")<br/><br/>

The **Service URL** in the screenshot is for the WEb-based UC3 [EZID service](http://n2t.net/ezid/ "EZID").  There really shouldn't be any need to change this (it's very stable), but it's exposed for the "just in case" situation.

The **Organization NAAN** is the unique organizational identifier that is assigned to an organization that has registered as an ARK minter.  Any organization that has the ability to mint ARKs will have a NAAN that distinguishes their ARKs from those minted by other institutions.  The **99999** identifier in the screenshot is just a fake NAAN that the UC3 has created as a test NAAN.  You can use this NAAN to test the Islandora EZID module, but you'll want to change it to your institution's NAAN before putting this module into production.

**ARK Shoulder** is a prefix that is used before all the ARKs minted through this module.  It provides a sub-namespace that can be used to group ARKs minted by an organization.  For instance, a library's digital projects team may use one ARK shoulder while the institutional repository team uses another.  The convention for newly assigned ARK shoulders is to have consonants terminated by a numeric.  This allows the length of the shoulder space to be variable.  The default **fk4** shoulder in the screenshot is the fake shoulder UC3 has created for testing.  Like the NAAN, it's fine to use for testing, but you'll want to change it before you put the module into production.

The last of these four settings is the **Number of ARKs Cached**.  This is used to configure how many ARKs the Islandora EZID module should keep in its local database.  These are "reserved" ARKs.  They've been created by the remote EZID minter, but not yet assigned to any resource.  Keeping them in the local Drupal database provides a faster response time when they're needed, and protects against any possible disruptions in the EZID service.  When they're paired with a resource, they become "assigned" and are no longer available in the reserved ARK cache.

The number of ARKs cached will deplete as they are used.  The module provides a cron job that can be run periodically to repopulate the cache.  Either the standard Drupal cron can be used (which runs all the Drupal crons at the same time) or you can use a more sophisticated Drupal cron like [Elysia Cron](https://drupal.org/project/elysia_cron "Elysia Cron"), which allows module cron jobs to be run independently.  For instance, you may run the Islandora EZID cron job every hour, but run the cron job to check for Drupal module updates just once a day.

It's important to note that the Islandora EZID will still function if the ARK cache gets depleted.  For on-demand individual ARK requests, it will just respond slower (as the module contacts EZID for each request).  For batch requests, the request will be scheduled to be run when the next Islandora EZID cron job runs.  After it completes, an email is then sent out with a link to a spreadsheet containing the newly minted ARKs.  Still, it's best to pick a cache size that is large enough to handle an average number of requests; the Islandora EZID module is much faster when it's working from its internal cache of reserved ARKs.  

After the first four settings are configured, you can scroll down to configure the rest.

![Islandora EZID configuration, page 2](http://projects.freelibrary.info/islandora_ezid/images/ezid-admin-screenshot-2.png "Islandora EZID configuration, page 2")<br/><br/>