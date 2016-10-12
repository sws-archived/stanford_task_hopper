<?php

define('STH_INSTALL_PROFILE', variable_get('install_profile'));

$path = DRUPAL_ROOT . '/sites/default/libraries';

if (is_dir($path)) {
  define('LIBRARY_PATH', $path);
} else {
  define('LIBRARY_PATH', DRUPAL_ROOT . '/sites/all/libraries');
}

switch (STH_INSTALL_PROFILE) {
  case "stanford_sites_jumpstart_engineering":
    define('STH_TASKS_DIR_JSE_INSTALL_BLOCK', LIBRARY_PATH .'/tasks/Stanford/JumpstartEngineering/Install/Block');
    define('STH_TASKS_DIR_JSE_INSTALL_CAPX', LIBRARY_PATH  . '/tasks/Stanford/JumpstartEngineering/Install/CAPx');
    define('STH_TASKS_DIR_JSE_INSTALL_CONTENT', LIBRARY_PATH  . '/tasks/Stanford/JumpstartEngineering/Install/Content');
    define('STH_TASKS_DIR_JSE_INSTALL_LAYOUTS', LIBRARY_PATH  . '/tasks/Stanford/JumpstartEngineering/Install/Layouts');
    define('STH_TASKS_DIR_JSE_INSTALL_MENU', LIBRARY_PATH . '/tasks/Stanford/JumpstartEngineering/Install/Menu');
    define('STH_TASKS_DIR_JSE_INSTALL_USERS', LIBRARY_PATH  . '/tasks/Stanford/JumpstartEngineering/Install/Users');
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/InstallTaskInterface.php';
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/AbstractTask.php';
    require_once DRUPAL_ROOT . '/profiles/stanford_sites_jumpstart_engineering/includes/AbstractInstallTask.php';
    break;
}

require_once DRUPAL_ROOT . '/includes/password.inc';
require_once LIBRARY_PATH . '/stanford_sites_content_importer/SitesContentImporter.php';
require_once LIBRARY_PATH . '/stanford_sites_content_importer/SitesContentImporterViews.php';
require_once LIBRARY_PATH . '/stanford_sites_content_importer/ImporterFieldProcessor.php';
require_once LIBRARY_PATH . '/tasks/autoloader.php';

//require_once DRUPAL_ROOT . drupal_get_path('library', 'tasks');


use Stanford\JumpstartEngineering\Install\Block;
use Stanford\JumpstartEngineering\Install\CAPx;
use Stanford\JumpstartEngineering\Install\Content;
use Stanford\JumpstartEngineering\Install\Layouts;
use Stanford\JumpstartEngineering\Install\Menu;
use Stanford\JumpstartEngineering\Install\Users;

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
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['jse-install-menu'] = array(
    'description' => 'Site layouts will be installed',
    'callback' => 'stanford_task_hopper_jse_install_menu',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-menu'),
  );
  $items['jse-install-users'] = array(
    'description' => 'Site layouts will be installed',
    'callback' => 'stanford_task_hopper_jse_install_users',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-users'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_SITE,
  );
  $items['jse-dept-mods'] = array(
    'description' =>
    'This imports content and enables modules for department sites on a JSE (PICAL) site',
    'callback' => 'stanford_task_hopper_jse_dept_mods',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-dept-mods'),
  );
  $items['jse-dept-unmods'] = array(
    'description' => 'This reverts the actions jse_dept_mods command.',
    'callback' => 'stanford_task_hopper_jse_dept_unmods',
    'drupal dependencies' => array('stanford_task_hopper'),
    'aliases' => array('sth-jse-dept-unmods'),
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
      drush_log('JSE nodes imported', 'ok');
    }
    if (in_array('ImportJSEBeans.php', $tasks)) {
      $task = new Content\ImportJSEBeans();
      $task->execute();
      drush_log('JSE beans imported', 'ok');
    }
    if (in_array('ImportJSECustomBeans.php', $tasks)) {
      $task = new Content\ImportJSECustomBeans();
      $task->execute();
      drush_log('JSE custom beans imported', 'ok');
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
  module_load_include('inc', 'stanford_jumpstart_home', 'stanford_jumpstart_home.context.inc');
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
      drush_log('JSE pical homepage installed', 'ok');
    }
    if (in_array('PicalPeople.php', $tasks)) {
      $task = new Layouts\PicalPeople();
      $task->execute();
      drush_log('JSE pical people installed', 'ok');
    }
    if (in_array('Sitewide.php', $tasks)) {
      $task = new Layouts\Sitewide();
      $task->execute();
      drush_log('JSE sitewide installed', 'ok');
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
 * Callback for the jse-install-users command
 */
function stanford_task_hopper_jse_install_users() {
  $install_profile = STH_INSTALL_PROFILE;
  if ($install_profile == 'stanford_sites_jumpstart_engineering') {
    $options = array();
    $choices = scandir(STH_TASKS_DIR_JSE_INSTALL_USERS);
    $choices = array_diff($choices, array('..', '.'));
    foreach ($choices as $choice) {
        $options[$choice] = $choice;
    }
    $tasks = drush_choice_multiple($options, dt('What tasks do you want to run?'));
    if (in_array('RichardUser.php', $tasks)) {
      $richard = user_load_by_name('richard');
        if (!$richard) {
        $task = new Users\RichardUser();
        $task->execute();
        drush_log('jse user richard installed', 'ok');
      }
      else {
        drush_log('jse user richard already installed', 'ok');
      }
    }
    if (in_array('UserOne.php', $tasks)) {
      $user_one = user_load_by_name('admin');
      if (!$user_one) {
      $task = new Users\UserOne();
      $task->execute();
      drush_log('jse user one installed', 'ok');
      }
      else {
        drush_log('jse user one already installed', 'ok');
      }
    }
  }
  else {
    drush_log('The drush command sth-jse-users is only for use with Jumpstart Engineering', 'ok');
  }
}

/**
 * Callback for the jse-dept-mods command
 * @param array $args
 *
 * This imports content and enables modules for department sites
 * on a JSE (PICAL) site
 */
function stanford_task_hopper_jse_dept_mods(&$args = array()) {

  // Todo: verify this is a JSE site.

  // Install the custom beans and css classes for the homepage layout.
  $task = new Content\ImportJSECustomBeans();
  $task->execute();
  drush_log('Custom beans imported', 'ok');

  $task = new Block\JSEBlockClasses();
  $task->execute();
  drush_log('jse block classes imported', 'ok');

  $task = new Block\JSEContextualBlockClasses();
  $task->execute();
  drush_log('jse contextual block classes imported', 'ok');

  $modules = array('stanford_jumpstart_home_dept', 'stanford_jse_dept',);
  module_enable($modules);

  drush_log('Enabled modules: ' . implode(', ', $modules), 'ok');
}

/**
 * Callback for the jse-dept-unmods command
 * @param array $args
 *
 * This reverts the jse_dept_mods command. Well, as much as possible.
 * It disables modules for department sites
 * on a JSE (PICAL) site
 */
function stanford_task_hopper_jse_dept_unmods(&$args = array()) {

  $modules = array('stanford_jumpstart_home_dept', 'stanford_jse_dept',);
  module_disable($modules);
  module_uninstall_modules($modules);

  drush_log('Disabled and uninstalled modules: ' . implode(', ', $modules), 'ok');
}
