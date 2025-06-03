---
id: game_managers_creating
title: Creating a game manager
description: How to create a new game manager in your project.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

Game managers are game-wide subsystems that you can implement in blueprints. To create a new game manager, right-click in the **Content Browser** and select **Select Basic Asset 🡒 Blueprint Class**:

<img alt="Select Basic Asset 🡒 Blueprint Class" src={useBaseUrl('img/game_manager_create_step_1.png')} />

Then in the **Pick Parent Class** dialog, expand **All Classes** and search for `GameManager`. Then, select it from the list:

<img alt="Select GameManager under All Classes, then click Select" src={useBaseUrl('img/game_manager_create_step_2.png')} />

Then click **Select**. You now have a new game manager that you can build in the blueprint editor.