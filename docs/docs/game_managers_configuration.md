---
id: game_managers_configuration
title: Adding a game manager to your project
description: How to add a game manager to your project that you've previously created.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

Once you create a game manager blueprint, you need to add it to your project configuration so it gets instantiated when the game starts, and so that it's available in other blueprints.

Open **Project Settings** and locate **Game 🡒 Game Management Framework**. Then under **Game Managers** click the `+` to add a new entry. You can have as many game managers in your project configuration as you like.

<img alt="Screenshot of game manager configuration" src={useBaseUrl('img/game_manager_config_step_1.png')} />

In the left hand side field, give your game manager a name - this is the unique name to be used when you're accessing the game manager in other blueprints. On the right hand side, select the game manager blueprint.

Later on, you can change the type of game manager blueprint; as long as the name remains the same all the blueprint references will remain intact. This allows you to replace the game manager implementation later without updating all your other blueprints. _However, if you change the name of a game manager (the left hand side field), then existing references to that game manager will stop working._