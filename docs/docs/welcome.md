---
id: welcome
title: Getting Started
description: Getting started with the Game Management Framework plugin for Unreal Engine.
slug: /
---

import useBaseUrl from '@docusaurus/useBaseUrl';

This guide will walk you through the process of installing the Game Management Framework plugin.

## Installing the plugin

### From the Marketplace

Once you have [purchased Game Management Framework on the Unreal Marketplace](https://www.unrealengine.com/marketplace/en-US/product/631d5ee26def4ec69565285b9889798a), you will be able to download and install it through the Epic Games Launcher. It will then be available for all projects on your machine.

### From source code

If you have source code access for the Game Management Framework plugin, you can clone the Git repository or add it as a submodule. If you don't have source code access, but you've purchased a license on the itch.io, reach out to [support on Discord](https://discord.gg/wKR82eK) to get access.

If your Unreal Engine project is in a Git repository, add it as a submodule like so:

```shell
git submodule add git@src.redpoint.games:redpointgames/game-management-framework Plugins/GameManagementFramework
```

Then, launch the editor and open your project.

:::note
You can also clone the repository using HTTPS using `https://src.redpoint.games/redpointgames/game-management-framework` as the URL, but you must [create a personal access token](https://src.redpoint.games/-/profile/personal_access_tokens) with the `read_repository` scope and use this personal access token as the password for HTTPS authentication. It is recommended that you use SSH URLs instead, as shown in the `git submodule add` command above.
:::

## Next steps

Read the documentation relevant to what you want to use in your project:

- [Game-wide subsystems in blueprints](game_managers_creating)
- [Performing asynchronous tasks in synchronous code](async_tasks_creating)
- [Showing loading screens or enqueuing async tasks](loading_screens_creating)