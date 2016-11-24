<?php
/**
 * @file
 * A collection of recipe related functions.
 */

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
    drush_user_abort("Canceled operation.");
  }

  // Load up and execute the recipe.


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



}

/**
 * Attempts to instantiate and describe a recipe.
 *
 * @param string $recipe
 *   A path to a recipe yml file.
 */
function stanford_task_hopper_describe_recipe($recipe) {

  if (!function_exists('yaml_parse')) {
    throw new Exception("You cannot parse yml. You need PECL.", 1);

  }

}
