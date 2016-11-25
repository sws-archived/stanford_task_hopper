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
    'aliases' => array('sth-t'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'category' => 'Task Hopper',
    'drush dependencies' => array("stanford_task_hopper.tasks"),
  );

  $items['hopper-task-execute'] = array(
    'description' => 'Execute a specific task.',
    'callback' => 'stanford_task_hopper_execute_task',
    'arguments' => array(
      'class' => 'The namespaced class name.',
    ),
    'aliases' => array('sth-tx'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'category' => 'Task Hopper',
    'drush dependencies' => array("stanford_task_hopper.tasks"),
  );

  $items['hopper-task-describe'] = array(
    'description' => 'Describe a specific task.',
    'callback' => 'stanford_task_hopper_describe_task',
    'arguments' => array(
      'class' => 'The namespaced class name.',
    ),
    'aliases' => array('sth-td'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'category' => 'Task Hopper',
    'drush dependencies' => array("stanford_task_hopper.tasks"),
  );

  $items['hopper-recipes'] = array(
    'description' => 'Get listings of all recipes.',
    'callback' => 'stanford_task_hopper_get_recipes',
    'aliases' => array('sth-r'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'category' => 'Task Hopper',
    'drush dependencies' => array(
      "stanford_task_hopper.recipes",
      "stanford_task_hopper.tasks",
    ),
  );

  $items['hopper-recipe-execute'] = array(
    'description' => 'Execute a recipe of tasks.',
    'callback' => 'stanford_task_hopper_execute_recipe',
    'arguments' => array(
      'file' => 'The path to the recipe file.',
    ),
    'aliases' => array('sth-rx'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'category' => 'Task Hopper',
    'drush dependencies' => array(
      "stanford_task_hopper.recipes",
      "stanford_task_hopper.tasks",
    ),
  );

  $items['hopper-recipe-describe'] = array(
    'description' => 'Describe a specific recipe.',
    'callback' => 'stanford_task_hopper_describe_recipe',
    'arguments' => array(
      'class' => 'The namespaced class name.',
    ),
    'aliases' => array('sth-rd'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'category' => 'Task Hopper',
    'drush dependencies' => array(
      "stanford_task_hopper.recipes",
      "stanford_task_hopper.tasks",
    ),
  );

  // TODO:
  // - List out all of the dependencies of a profile
  // - List out all of the dependencies of a recipe
  // - List out all of the dependencies of a task.

  return $items;
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
 * Load up any dependencies that are needed for this thing to run.
 */
function stanford_task_hopper_load_dependencies() {
  // Load the autoloader.
  stanford_task_hopper_load_task_autoloader();

  // Add a couple of Drupal root items.
  require_once DRUPAL_ROOT . '/includes/password.inc';
  require_once DRUPAL_ROOT . '/includes/install.inc';
  require_once DRUPAL_ROOT . '/includes/update.inc';
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

  // First try to find a specific configuration setting.
  $vget = variable_get("stanford_task_hopper_task_path", NULL);

  if (!is_null($vget)) {

    // If the directory does not exist or the path from this execution is wrong
    // error out.
    if (!is_dir($vget)) {
      throw new Exception("Task directory not found.");
    }

    // Checks out. Go with the setting.
    return $vget;
  }

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
 * Check to see if a string is a yml file.
 *
 * @param string $opt
 *   The string value of the last choice.
 *
 * @return bool
 *   True for a match.
 */
function stanford_task_hopper_path_is_recipe($opt) {
  return preg_match('/^.*\.(yml)$/i', $opt);
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
