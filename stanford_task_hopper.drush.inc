<?php

define('STH_INSTALL_PROFILE', variable_get('install_profile'));

switch (STH_INSTALL_PROFILE) {
  case "stanford_sites_jumpstart_engineering":
    define('STH_TASKS_DIR_JSE_INSTALL_BLOCK', DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/Block');
    define('STH_TASKS_DIR_JSE_INSTALL_CAPX', DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/CAPx');
    define('STH_TASKS_DIR_JSE_INSTALL_CONTENT', DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/Content');
    define('STH_TASKS_DIR_JSE_INSTALL_LAYOUTS', DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/Layouts');
    define('STH_TASKS_DIR_JSE_INSTALL_MENU', DRUPAL_ROOT . '/sites/all/libraries/tasks/Stanford/JumpstartEngineering/Install/Menu');
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/InstallTaskInterface.php';
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/AbstractTask.php';
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/AbstractInstallTask.php';
    break;
}

require_once DRUPAL_ROOT . '/sites/all/libraries/stanford_sites_content_importer/SitesContentImporter.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/stanford_sites_content_importer/SitesContentImporterViews.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/stanford_sites_content_importer/ImporterFieldProcessor.php';
require_once DRUPAL_ROOT . '/sites/all/libraries/tasks/autoloader.php';

use Stanford\JumpstartEngineering\Install\Block;
use Stanford\JumpstartEngineering\Install\CAPx;
use Stanford\JumpstartEngineering\Install\Content;
use Stanford\JumpstartEngineering\Install\Layouts;
use Stanford\JumpstartEngineering\Install\Menu;

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
  );
  $items['jse-install-blocks'] = array(
    'description' => 'Install jse block classes.',
    'callback' => 'stanford_task_hopper_jse_install_block',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-ib'),
  );
  $items['jse-install-capx'] = array(
    'description' => 'Install jse block classes.',
    'callback' => 'stanford_task_hopper_jse_install_capx',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-capx'),
  );
  $items['jse-install-content'] = array(
    'description' => 'Site content of this type will be installed',
    'callback' => 'stanford_task_hopper_jse_install_content',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-ic'),
  );
  $items['jse-install-layouts'] = array(
    'description' => 'Site layouts will be installed',
    'callback' => 'stanford_task_hopper_jse_install_layouts',
    'drupal dependencies' => array('stanford_task_hopper', 'stanford_jumpstart_home'),
    'aliases' => array('sth-jse-layouts'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_SITE,
  );
  $items['jse-install-menu'] = array(
    'description' => 'Site layouts will be installed',
    'callback' => 'stanford_task_hopper_jse_install_menu',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-menu'),
  );
  $items['jse-dept-mods'] = array(
    'description' => 'Site content of this type will be installed',
    'callback' => 'stanford_task_hopper_jse_dept_mods',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-dept-mods'),
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
function stanford_task_hopper_jse_install_block() {
  $install_profile = STH_INSTALL_PROFILE;
  if ($install_profile == 'stanford_sites_jumpstart_engineering') {
    $options = array();
    $choices = scandir(STH_TASKS_DIR_JSE_INSTALL_BLOCK);
    $choices = array_diff($choices, array('..', '.'));
    foreach ($choices as $choice) {
        $options[$choice] = $choice;
    }
    $tasks = drush_choice_multiple($options, dt('What tasks do you want to run?'));
    if (in_array('JSEBlockClasses.php', $tasks)) {
      $task = new Block\JSEBlockClasses();
      $task->execute();
      drush_log('jse block classes imported', 'ok');
    }
    if (in_array('JSEContextualBlockClasses.php', $tasks)) {
      $task = new Block\JSEContextualBlockClasses();
      $task->execute();
      drush_log('jse contextual block classes imported', 'ok');
    }
  }
  else {
    drush_log('The drush command sth-jse-ic is only for use with Jumpstart Engineering', 'ok');
  }
}

/**
 * Callback for the install-capx command
 */
function stanford_task_hopper_jse_install_capx() {
  $install_profile = STH_INSTALL_PROFILE;
  if ($install_profile == 'stanford_sites_jumpstart_engineering') {
    $options = array();
    $choices = scandir(STH_TASKS_DIR_JSE_INSTALL_CAPX);
    $choices = array_diff($choices, array('..', '.'));
    foreach ($choices as $choice) {
        $options[$choice] = $choice;
    }
    $tasks = drush_choice_multiple($options, dt('What tasks do you want to run?'));
    if (in_array('CAPxConfig.php', $tasks)) {
      $machine_names = array('default', 'jse_default');
      $q = db_select('capx_cfe', 'cfe');
      $q->addField('cfe', 'machine_name');
      $q->condition('cfe.machine_name', $machine_names, 'IN');
      $r = $q->execute()->fetchObject();
      if ($r) {
        drush_log('jse capx config already imported', 'ok');
      }
      else {
        $task = new CAPx\CAPxConfig();
        $task->execute();
        drush_log('jse capx config imported', 'ok');
      }
    }
    if (in_array('CAPxDisplay.php', $tasks)) {
      $task = new CAPx\CAPxDisplay();
      $task->execute();
      drush_log('jse capx display imported', 'ok');
    }
  }
  else {
    drush_log('The drush command sth-jse-ic is only for use with Jumpstart Engineering', 'ok');
  }
}

/**
 * Callback for the install-content command
 */
function stanford_task_hopper_jse_install_content() {
  $install_profile = STH_INSTALL_PROFILE;
  if ($install_profile == 'stanford_sites_jumpstart_engineering') {
    $options = array();
    $choices = scandir(STH_TASKS_DIR_JSE_INSTALL_CONTENT);
    $choices = array_diff($choices, array('..', '.'));
    foreach ($choices as $choice) {
        $options[$choice] = $choice;
    }
    $tasks = drush_choice_multiple($options, dt('What tasks do you want to run?'));
    if (in_array('ImportJSENodes.php', $tasks)) {
      $task = new Content\ImportJSENodes();
      $task->execute();
      drush_log('jse nodes imported', 'ok');
    }
    if (in_array('ImportJSEBeans.php', $tasks)) {
      $task = new Content\ImportJSEBeans();
      $task->execute();
      drush_log('jse beans imported', 'ok');
    }
    if (in_array('ImportJSEGibbonsBeans.php', $tasks)) {
      $task = new Content\ImportJSEGibbonsBeans();
      $task->execute();
      drush_log('jse beans for Gibbons layout imported', 'ok');
    }

  }
  else {
    drush_log('The drush command sth-jse-ic is only for use with Jumpstart Engineering', 'ok');
  }
}

/**
 * Callback for the jse-install-layouts command
 */
function stanford_task_hopper_jse_install_layouts() {
  $install_profile = STH_INSTALL_PROFILE;
  if ($install_profile == 'stanford_sites_jumpstart_engineering') {
    $options = array();
    $choices = scandir(STH_TASKS_DIR_JSE_INSTALL_LAYOUTS);
    $choices = array_diff($choices, array('..', '.'));
    foreach ($choices as $choice) {
        $options[$choice] = $choice;
    }
    $tasks = drush_choice_multiple($options, dt('What tasks do you want to run?'));
    if (in_array('PicalHomepage.php', $tasks)) {
      $task = new Layouts\PicalHomepage();
      $task->execute();
      drush_log('jse pical homepage installed', 'ok');
    }
    if (in_array('PicalPeople.php', $tasks)) {
      $task = new Layouts\PicalPeople();
      $task->execute();
      drush_log('jse pical people installed', 'ok');
    }
    if (in_array('Sitewide.php', $tasks)) {
      $task = new Layouts\Sitewide();
      $task->execute();
      drush_log('jse sitewide installed', 'ok');
    }
  }
  else {
    drush_log('The drush command sth-jse-layouts is only for use with Jumpstart Engineering', 'ok');
  }
}

/**
 * Callback for the jse-install-menu command
 */
function stanford_task_hopper_jse_install_menu() {
  $install_profile = STH_INSTALL_PROFILE;
  if ($install_profile == 'stanford_sites_jumpstart_engineering') {
    $options = array();
    $choices = scandir(STH_TASKS_DIR_JSE_INSTALL_MENU);
    $choices = array_diff($choices, array('..', '.'));
    foreach ($choices as $choice) {
        $options[$choice] = $choice;
    }
    $tasks = drush_choice_multiple($options, dt('What tasks do you want to run?'));
    if (in_array('JSEAdminShortcutMenuItems.php', $tasks)) {
      $task = new Menu\JSEAdminShortcutMenuItems();
      $task->execute();
      drush_log('jse admin shortcuts installed', 'ok');
    }
    if (in_array('JSEMenuItems.php', $tasks)) {
      $task = new Menu\JSEMenuItems();
      $task->execute();
      drush_log('jse menu items installed', 'ok');
    }
    if (in_array('JSEMenuPositionRules.php', $tasks)) {
      $task = new Menu\JSEMenuPositionRules();
      $task->execute();
      drush_log('jse menu position rules installed', 'ok');
    }
    if (in_array('JSEMenuRedirects.php', $tasks)) {
      $task = new Menu\JSEMenuRedirects();
      $task->execute();
      drush_log('jse menu redirects installed', 'ok');
    }
    if (in_array('JSEPrivateMenuItems.php', $tasks)) {
      $task = new Menu\JSEPrivateMenuItems();
      $task->execute();
      drush_log('jse private menu items installed', 'ok');
    }
  }
  else {
    drush_log('The drush command sth-jse-menu is only for use with Jumpstart Engineering', 'ok');
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
