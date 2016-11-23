<?php
/**
 * @file
 * Stanford Task Hopper Drush Commands.
 *
 * If your module will not enable, check to see if it has any dependencies.
 */

/**
 * Implements hook_drush_command().
 */
function stanford_task_hopper_drush_command() {
  $items = array();

  $items['hopper-tasks'] = array(
    'description' => 'Get listings of executable tasks.',
    'callback' => 'stanford_task_hopper_get_tasks',
    'aliases' => array('sth'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_SITE,
    'category' => 'Task Hopper',
  );

  return $items;
}

/**
 * Provide a prompt for users to find and execute tasks.
 */
function stanford_task_hopper_get_tasks() {

  // Check to see if the user has the required version first.
  stanford_task_hopper_check_drush_version();

  // Load the autoloader.
  stanford_task_hopper_load_task_autoloader();

  // Get the tasks.
  $tasks_path = stanford_task_hopper_get_tasks_path();
  $mask = array(".", "..", "ITasks", "autoloader.php", "README.md", "img");
  $opt = NULL;

  // While the selection is not a task keep looping deeper in to the options.
  while (!stanford_task_hopper_path_is_task($opt) && $opt !== FALSE) {
    $scan_dir = $tasks_path;
    if (is_array($choices)) {
      // @todo: prepare for windows as as well.
      $scan_dir .= "/" . implode("/", $choices);
    }
    $dir = drush_scan_directory($scan_dir, "*.*", $mask, NULL, 0, "basename");
    // Add an option to go back up a level to the option #1.
    array_unshift($dir, array("Back" => "Back"));
    // Keys are the basename of the result.
    $keys = array_keys($dir);

    $message = "Choose a directory to explore or task to execute.";
    $opt = drush_choice($dir, $message, "!key");

    // Keep track of the navigation so we can offer new prompts.
    $choices[] = $opt;

    // Pop two off the end of the choices if we are to go back up a level.
    if ($opt == "Back") {
      array_pop($choices);
      array_pop($choices);
    }

  } // End while loop.

  // If the user canceled the operation end the journey.
  if ($opt == FALSE) {
    drush_user_abort("Canceled operation.");
  }

  // Load up and execute the task.
  $class = stanford_task_hopper_get_class_name_from_path_array($choices);
  stanford_task_hopper_execute_task($class);
}

/**
 * Attempts to instantiate and execute a task.
 *
 * Adds execution to logs so we can determine at a later date which tasks have
 * been run on a particular site.
 *
 * @param string $class
 *   A globally namespaced class name. eg: \Drupal\Standard\Install\TaskName
 */
function stanford_task_hopper_execute_task($class) {

  $task = new $class();
  try {
    $task->execute();
  }
  // If the task failed to execute then record that and exit out.
  catch (Exception $e) {
    stanford_task_hopper_record_task($class, 'fail');
    drush_set_error("Task could not execute: ", $e->message(), "error");
    return;
  }

  // Record the execution of a task to the db schema.
  stanford_task_hopper_record_task($class, 'success');
  drush_log($class . " executed successfully", "ok");
}

/**
 * Records history to the database.
 *
 * @param string $class
 *   Global namespace class name eg: \Drupal\Standard\Install\TaskName.
 * @param string $status
 *   A status type. Currently supports "fail", "success".
 */
function stanford_task_hopper_record_task($class, $status) {
  // TODO write to db.
}








/**
 * A**********************************************.
 *
 * Helper / Utility Functions.
 *
 * A**********************************************.
 */

/**
 * Checks to see if the user meets the minimum requirements.
 */
function stanford_task_hopper_check_drush_version() {
  $version = drush_core_version();
  if ($version < 7) {
    drush_log("Your drush version is too old. Please update to version 7.", "error");
    drush_user_abort();
  }
}

/**
 * Gets the root path to the tasks directory.
 *
 * This function throws errors instead of setting drush_set_error because if
 * the task directory cannot be found there is no reason to continue to run.
 *
 * @return string
 *   the path to the tasks directory.
 */
function stanford_task_hopper_get_tasks_path() {

  // Load up the current installation profile as it has the info we need.
  $profile = variable_get("install_profile", FALSE);

  // If the profile cannot be found error out.
  if (!$profile) {
    throw new Exception("No installation profile defined");
  }

  // Get the settings from the .info file as we need the taskdir value.
  $info = system_get_info('module', $profile);
  if (!$info['taskdir']) {
    throw new Exception("No tasks path set in installation profile");
  }

  // If the directory does not exist or the path from this execution is wrong
  // error out.
  if (!is_dir($info['taskdir'])) {
    throw new Exception("Task directory not found.");
  }

  // At last return the path. eg: "sites/default/libraries/tasks".
  return $info['taskdir'];
}

/**
 * Requires the autoload.php file in the tasks directory.
 *
 * Throws an error if it could not load the file.
 */
function stanford_task_hopper_load_task_autoloader() {
  $task_dir = stanford_task_hopper_get_tasks_path();
  require_once $task_dir . "/autoloader.php";
}

/**
 * Check to see if a string is a php file.
 *
 * @param string $opt
 *   The string value of the last choice.
 *
 * @return bool
 *   True for a match.
 */
function stanford_task_hopper_path_is_task($opt) {
  return preg_match('/^.*\.(php)$/i', $opt);
}

/**
 * Takes an array of values and implodes them in to a namespaced class.
 *
 * @return string
 *   A fully namespaced class name string.
 */
function stanford_task_hopper_get_class_name_from_path_array(array $choices) {
  $concat = implode("\\", $choices);
  $name = "\\" . str_ireplace(".php", "", $concat);
  return $name;
}
