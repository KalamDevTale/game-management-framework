---
id: loading_screens_creating
title: Creating a loading screen
description: How to create a new loading screen in your project.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

To show a loading screen while asynchronous tasks are running, you need to create a loading screen blueprint.

## Creating the blueprint

To create a loading screen, create a blueprint that derives from `GameManagerLoadingScreen`.

<img alt="Select Basic Asset 🡒 Blueprint Class" src={useBaseUrl('img/game_manager_create_step_1.png')} />

In the **Pick Parent Class** dialog, expand **All Classes** and search for `GameManagerLoadingScreen`. Then, select it from the list:

<img alt="Select GameManagerLoadingScreen under All Classes, then click Select" src={useBaseUrl('img/loading_screen_create_step_1.png')} />

## Implementing a loading screen

By default, your loading screen blueprint won't actually do anything when it's time to show a loading screen. You need to add event handlers for the following events:

- **Show Loading Screen**: Called when the loading screen should be shown, with the initial task that is being worked on. Usually you'll create a UMG widget and add it to the viewport here.
- **Update Loading Screen Status**: Called when the loading screen moves onto the next queued up task. Rather than hiding and showing the loading screen between each task, instead this event is called as the loading screen progresses through tasks to make things seamless for the player.
- **Hide Loading Screen**: Called when there are no more tasks in the queue that require a loading screen to be shown. Usually you will remove a UMG widget from the viewport here.

<img alt="Add the events to your loading screen blueprint" src={useBaseUrl('img/loading_screen_create_step_2.png')} />

How you actually show the loading screen to the user is up to you. The most common way is to use a fullscreen UMG widget, but you can actually do anything you like in response to the loading screen events. This allows you build advanced loading screens with animations and 3D effects if you want.