<?php

define('STH_TASKS_DIR_JSE_INSTALL_CONTENT', DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/Content');
define('STH_INSTALL_PROFILE', variable_get('install_profile'));

libraries_load('tasks');

switch (STH_INSTALL_PROFILE) {
  case "stanford_sites_jumpstart_engineering":
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/InstallTaskInterface.php';
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/AbstractTask.php';
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/AbstractInstallTask.php';
    break;
}

require_once DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/Jumpstart/Install/Content/Importer/LoadImporterLibrary.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/stanford_sites_content_importer/SitesContentImporter.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/stanford_sites_content_importer/SitesContentImporterViews.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/stanford_sites_content_importer/ImporterFieldProcessor.php';

require_once DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/Jumpstart/Install/Content/Importer/ImporterFieldProcessorCustomBody.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/Jumpstart/Install/Content/Importer/ImporterFieldProcessorCustomFieldSDestinationPublish.php';

require_once DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/Content/ImportJSENodes.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/Content/ImportJSEBeans.php';

/**
 * Implements hook_drush_command().
 */
function stanford_task_hopper_drush_command() {
  $items = array();
  $items['jse-get-tasks'] = array(
    'description' => 'Get install tasks related to this Site Type',
    'callback' => 'stanford_task_hopper_jse_get_tasks',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-gt'),
    //'arguments' => array(
    //  'site_type' => 'Site content of this type will be installed.',
    //),
  );
  $items['jse-install-content'] = array(
    'description' => 'Site content of this type will be installed',
    'callback' => 'stanford_task_hopper_jse_install_content',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-ic'),
    'arguments' => array(
      'site_type' => 'Site content of this type will be installed.',
    ),
  );
  $items['jse-dept-mods'] = array(
    'description' => 'Site content of this type will be installed',
    'callback' => 'stanford_task_hopper_jse_dept_mods',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-dept-mods'),
    'arguments' => array(
      'site_type' => 'Site content of this type will be installed.',
    ),
  );
  return $items;
}

/**
 * Callback for the get-tasks command
 */
function stanford_task_hopper_jse_get_tasks() {
  drush_log('performed get tasks', 'ok');
}

/**
 * Callback for the install-content command
 */
function stanford_task_hopper_jse_install_content() {
  $install_profile = STH_INSTALL_PROFILE;
  if ($install_profile = 'stanford_sites_jumpstart_engineering') {
    $options = array();
    $choices = scandir(STH_TASKS_DIR_JSE_INSTALL_CONTENT);
    $choices = array_diff($choices, array('..', '.'));
    foreach ($choices as $choice) {
        $options[$choice] = $choice;
    }
    $tasks = drush_choice_multiple($options, dt('What tasks do you want to run?'));
    if (in_array('ImportJSENodes.php', $tasks)) {
      $task = new Stanford\JumpstartEngineering\Install\Content\ImportJSENodes();
      $task->execute();
      drush_log('jse nodes imported', 'ok');
    }
    if (in_array('ImportJSEBeans.php', $tasks)) {
      $task = new Stanford\JumpstartEngineering\Install\Content\ImportJSEBeans();
      $task->execute();
      drush_log('jse beans imported', 'ok');
    }
  }
  else {
    drush_log('The drush command sth-jse-ic is only for use with Jumpstart Engineering', 'ok');
  }
}

/**
 * Callback for the jse-dept-mods command
 */
function stanford_task_hopper_jse_dept_mods(&$args = array()) {
  
  module_enable(array(
    //"<module-name-goes-here>",
  ));
  drush_log('dept modules enabled', 'ok');
}
