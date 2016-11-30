#[Stanford Task Hopper](https://github.com/SU-SWS/stanford_task_hopper)
##### Version: 7.x-1.x

Maintainer: [boznik](https://github.com/boznik), [cjwest](https://github.com/cjwest), [sherakama](https://github.com/sherakama)

[Changelog.txt](CHANGELOG.txt)

A Drush plugin for iTasks task execution. This plugin provides the ability to execute single (task) or multiple (recipe) files which we call tasks. see usage below for more information.

Installation
---

Install this module like any other module. [See Drupal Documentation](https://drupal.org/documentation/install/modules-themes/modules-7)

Configuration
---

If you don't use the iTasks installation profile set up you can override the tasks path with your own setting. 

```
drush vset stanford_task_hopper_task_path relative/to/drupal/root/path
```

Requirements
---

**Installation Profile**  
If you are not setting the configuration variable for the task library you must use an installation profile which uses the iTasks install framework. Specifically, the installation profile must have a taskDir option in the .info file. eg:

```
; ITasks Config
taskdir = sites/all/libraries/tasks
```

**Task Library**  
You must have a task library available. You can download it at [https://github.com/SU-SWS/stanford_install_tasks](https://github.com/SU-SWS/stanford_install_tasks). This library must be installed at the path configured in the installation profile.


**Drush Version**  
Currently the task hopper requires the use of Drush version 8.


Usage
-----

###Task Browser  
Browse and execute a single task through a multiple choice prompt. This function allows you to navigate through all of the tasks that are available and choose one to execute. Validation is performed before execution if the task provides it.

**Commands:**  
`drush hopper-tasks`  
`drush sth-t`

---

###Single Task Execution  
Execute a single task directly. Validation is performed before execution if the task provides it. 

**Commands:**  
`drush hopper-task-execute $class`  
`drush sth-tx $class`

**Arguements:**  
`$class` A fully namespaced class name. Required.

**Example:**  
`drush sth-tx "\Stanford\JumpstartEngineering\Install\CAPx\CAPxConfig"`

---

###Describe A Single Task  
Want to know more about the task? You can ask it to describe itself. This function prints out the task description to the screen.

**Commands:**  
`drush hopper-task-describe $class`  
`drush sth-td $class`

**Arguements:**  
`$class` A fully namespaced class name. Required.

**Example:**  
`drush sth-td "\Stanford\JumpstartEngineering\Install\CAPx\CAPxConfig"`

---

###Recipe Browser  
Browse and execute a recipe through a multiple choice prompt. This function allows you to navigate through all of the recipes that are available and choose one to execute. Validation is performed before execution if the tasks provide it.

**Commands:**  
`drush hopper-recipes`  
`drush sth-r`

---

###Recipe Execution  
Execute a recipe directly. Validation is performed before execution if the tasks provide it. 

**Commands:**  
`drush hopper-recipe-execute $recipe`  
`drush sth-tx $recipe`

**Arguements:**  
`$recipe` A relative to the Recipe directory path. Required.

**Example:**  
`drush sth-rx "JumpstartEngineering/jse-install-capx.yml"`

---

###Describe A Recipe  
Want to know more about the recipe? You can ask it to describe itself. This function prints out the recipe's description to the screen.

**Commands:**  
`drush hopper-recipe-describe $recipe`  
`drush sth-rd $recipe`

**Arguements:**  
`$recipe` A relative to the Recipe directory path. Required.

**Example:**  
`drush sth-rd "JumpstartEngineering/jse-install-capx.yml"`
