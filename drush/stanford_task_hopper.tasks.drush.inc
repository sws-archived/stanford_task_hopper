<?php
/**
 * @file
 * A collection of task related functions.
 */

/**
 * Provide a prompt for users to find and execute tasks.
 */
function stanford_task_hopper_get_tasks() {

  // Get the tasks.
  $tasks_path = stanford_task_hopper_get_tasks_path();
  $mask = array(".", "..", "ITasks", "Recipes", "autoloader.php", "README.md", "img");
  $opt = NULL;
  $choices = NULL;

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
  drush_stanford_task_hopper_hopper_task_execute_pre_validate($class);
  stanford_task_hopper_execute_task($class);
}

/**
 * Attempts to instantiate and execute a task.
 *
 * Adds execution to logs so we can determine at a later date which tasks have
 * been run on a particular site.
 *
 * @param string $class
 *   A globally namespaced class name. eg: \Drupal\Standard\Install\TaskName.
 */
function stanford_task_hopper_execute_task($class) {

  // Give the task a go.
  try {
    $task = new $class();
    $task->execute();
  }
  // If the task failed to execute then record that and exit out.
  catch (Exception $e) {
    stanford_task_hopper_record_task($class, 'fail');
    drush_set_error("error", $e->getMessage(), "Task could not execute. The error that was given:\n\n");
    return;
  }

  // Record the execution of a task to the db schema.
  stanford_task_hopper_record_task($class, 'success');
  drush_log($class . " executed successfully", "ok");
}

/**
 * Attempts to instantiate and describe a task.
 *
 * @param string $class
 *   A globally namespaced class name. eg: \Drupal\Standard\Install\TaskName.
 */
function stanford_task_hopper_describe_task($class) {

  // Load the dependencies.
  stanford_task_hopper_load_dependencies();

  $desc = NULL;

  // Give the task a go.
  try {
    $task = new $class();
    $desc = $task->getDescription();
  }
  // If the task failed to execute then record that and exit out.
  catch (Exception $e) {
    drush_set_error("error", $e->getMessage(), "Task could not execute. The error that was given:\n\n");
    return;
  }

  drush_log($desc, "status");

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
  $record = array(
    'task' => $class,
    'status' => $status,
    'time' => time(),
  );
  drupal_write_record('stanford_task_hopper', $record);
}

/**
 * A***********************************************.
 *
 * Drush hooks.
 *
 * A***********************************************.
 */

/**
 * Validation hook for executing a task.
 *
 * @param string $class
 *   The class name that is going to be called.
 */
function drush_stanford_task_hopper_hopper_task_execute_pre_validate($class) {
  // Load the dependencies.
  stanford_task_hopper_load_dependencies();

  // Validate that the task has it's dependencies.
  try {
    $task = new $class();
  }
  // If the task failed to execute then record that and exit out.
  catch (Exception $e) {
    drush_set_error("error", $e->getMessage());
    drush_user_abort();
  }

  // Fetch and check that each of the dependencies are available.
  $req = $task->requirements();
  foreach ($req as $module) {
    if (!module_exists($module)) {
      drush_set_error("error", "Missing dependency: " . $module);
      drush_user_abort();
    }
  }
}

/**
 * Before task execution hook.
 *
 * Adds checks for versions, autoloading, and dependencies here.
 */
function drush_stanford_task_hopper_pre_hopper_task_execute($class) {
  // Load the dependencies so tasks can run.
  stanford_task_hopper_load_dependencies();
}
