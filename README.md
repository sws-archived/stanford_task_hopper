#[Stanford Task Hopper](https://github.com/SU-SWS/stanford_task_hopper)
##### Version: 7.x-1.x

Maintainer: [boznik](https://github.com/boznik), [cjwest](https://github.com/cjwest), [sherakama](https://github.com/sherakama)

[Changelog.txt](CHANGELOG.txt)

A Drush plugin for iTasks task execution. This plugin provides the ability to execute single (task) or multiple (recipe) files which we call tasks.

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
You must be using an installation profile which uses the iTasks install framework. Specifically, the installation profile must have a taskDir option in the .info file. eg:

```
; ITasks Config
taskdir = sites/all/libraries/tasks
```

**Task Library**  
You must have a task library available. You can download it at https://github.com/SU-SWS/stanford_install_tasks. This library must be installed at the path configured in the installation profile.


Usage
-----
