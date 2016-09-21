#[Stanford Task Hopper](https://github.com/SU-SWS/stanford_task_hopper)
##### Version: 7.x-1.x

Maintainer: [boznik](https://github.com/boznik)

[Changelog.txt](CHANGELOG.txt)

Drush Plugin Module for Task Management

This modules will be used to do custom configuration to Jumpstart Sites.

Installation
---

Install this module like any other module. [See Drupal Documentation](https://drupal.org/documentation/install/modules-themes/modules-7)

Usage
-----
`drush <command-name>`

* Enter command
* Select the number associated with the task to run
* Repeat selecting task numbers
* Enter the number for `Done` when have finised selecting tasks
* Watch for a confirmation (This may take a moment)

Here's an example usage:

```
drush sth-jse-ic
Select some numbers.
 [0]  :  Cancel             
 [1]  :  ImportJSEBeans.php 
 [2]  :  ImportJSENodes.php 
 [3]  :  Done
2
Select some numbers.
 [0]  :  Cancel                        
 [1]  :  ImportJSEBeans.php            
 [2]  :  ImportJSENodes.php (selected) 
 [3]  :  Done
3
 [notice] jse nodes imported
 ```