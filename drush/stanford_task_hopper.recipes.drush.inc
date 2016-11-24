<?php
/**
 * @file
 * A collection of recipe related functions.
 */

$me = dirname(__FILE__);
require_once $me . "/../includes/yaml/Exception/ExceptionInterface.php";
require_once $me . "/../includes/yaml/Exception/RuntimeException.php";
require_once $me . "/../includes/yaml/Exception/ParseException.php";
require_once $me . "/../includes/yaml/Yaml.php";

use Symfony\Component\Yaml\Yaml;

/**
 * Provide a prompt for users to find and execute recipes.
 */
function stanford_task_hopper_get_recipes() {

  // Get the recipes.
  $recipes_path = stanford_task_hopper_get_tasks_path() . "/Recipes";
  $mask = array(".", "..", "ITasks", "autoloader.php", "README.md", "img", "*.php");
  $opt = NULL;
  $choices = NULL;

  // While the selection is not a recipe keep looping deeper in to the options.
  while (!stanford_task_hopper_path_is_recipe($opt) && $opt !== FALSE) {
    $scan_dir = $recipes_path;
    if (is_array($choices)) {
      // @todo: prepare for windows as as well.
      $scan_dir .= "/" . implode("/", $choices);
    }
    $dir = drush_scan_directory($scan_dir, "*.*", $mask, NULL, 0, "basename");
    // Add an option to go back up a level to the option #1.
    array_unshift($dir, array("Back" => "Back"));
    // Keys are the basename of the result.
    $keys = array_keys($dir);

    $message = "Choose a directory to explore or recipe to execute.";
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
    drush_user_abort("Cancelled operation.");
  }

  // Load up and execute the recipe.
  $path = implode("/", $choices);
  stanford_task_hopper_execute_recipe($path);

}

/**
 * Attempts to instantiate and execute a recipe.
 *
 * Adds execution to logs so we can determine at a later date which recipes have
 * been run on a particular site.
 *
 * @param string $recipe
 *   A path to a recipe.
 */
function stanford_task_hopper_execute_recipe($recipe) {
  // Get the yml in to a form we can work with.
  $info = stanford_task_hopper_get_recipe_yml_info($recipe);

  // Something is wrong if we have no tasks.
  if (!isset($info['tasks']) || !is_array($info['tasks'])) {
    throw new Exception("No tasks found in recipe yml file.");
  }

  // Validate all the tasks.
  array_map('drush_stanford_task_hopper_hopper_task_execute_pre_validate', $info['tasks']);

  // Execute each of the tasks.
  array_map('stanford_task_hopper_execute_task', $info['tasks']);

  // Let the user know. :D.
  drush_log($info['machine_name'] . " completed successfully.", "status");
}

/**
 * Attempts to instantiate and describe a recipe.
 *
 * @param string $recipe
 *   A path to a recipe yml file.
 */
function stanford_task_hopper_describe_recipe($recipe) {
  $info = stanford_task_hopper_get_recipe_yml_info($recipe);

  if (!isset($info['name'])) {
    drush_log("No name available.", "error");
    return;
  }

  if (!isset($info['description'])) {
    drush_log("No description available.", "error");
    return;
  }

  drush_log($info['name'], 'status');
  drush_log($info['description'], 'status');
}

/**
 * A*******************************************************.
 *
 * HELPER FUNCTIONS.
 *
 * A*******************************************************.
 */

/**
 * Takes a relative path to a yml recipe file and turns it into a php array.
 *
 * @param string $recipe
 *   A string with a file path relative to the Recipes folder in the tasks repo.
 *
 * @return array
 *   An array with the parsed info from the yml file.
 */
function stanford_task_hopper_get_recipe_yml_info($recipe) {
  $path = stanford_task_hopper_get_tasks_path() . "/Recipes/";
  $full = $path . $recipe;
  $value = NULL;

  if (!is_file($full)) {
    throw new Exception("Recipe YML File Not Found");
  }

  try {
    $value = Yaml::parse(file_get_contents($full));
  }
  catch (ParseException $e) {
    drush_set_error("error", $e->getMessage(), "Could not parse yml file: $full");
    drush_user_abort();
  }
  return $value;
}
