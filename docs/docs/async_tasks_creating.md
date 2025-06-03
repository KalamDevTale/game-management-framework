---
id: async_tasks_creating
title: Creating an asynchronous task
description: How to define a new asynchronous task in your project.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

Asynchronous tasks in Game Management Framework allow you to call asynchronous (or _latent_) operations from synchronous code.

A good example is downloading user profile pictures - you might be implementing the `Get Brush` callback in a UMG widget, and you need to return the user profile picture for the widget. But at this point, you don't have the profile picture downloaded (you might only have the user ID). You can't call `Download Image` from this callback, and even if you could, you'd want the result to be cached.

That's where asynchronous tasks come in. You can ask them to be scheduled from synchronous code (like a `Get Brush` callback), and then you can do different things based on whether or not they've completed their work.

## Creating the blueprint

To create an asynchronous task, create a blueprint that derives from `GameManagerTask`.

<img alt="Select Basic Asset 🡒 Blueprint Class" src={useBaseUrl('img/game_manager_create_step_1.png')} />

In the **Pick Parent Class** dialog, expand **All Classes** and search for `GameManagerTask`. Then, select it from the list:

<img alt="Select GameManagerTask under All Classes, then click Select" src={useBaseUrl('img/async_task_creating_step_2.png')} />

## Handle the "Scheduled" event

Once you have the blueprint editor open for your new task, you need to add the **Scheduled** event and call the **Mark Task as Complete** when your task is done.

:::caution
You must call **Mark Task as Complete** at some point after the Scheduled event occurs, even if the task fails! If you don't call it, then the task will never be ready for synchronous code.
:::

<img alt="Handle the Scheduled event and call Mark Task as Complete at some point" src={useBaseUrl('img/async_task_creating_step_3.png')} />

## Enabling the Tick event

*Available Since:* 1.25.3

If you need to perform custom ticking inside your asynchronous task, you can enable the Tick event by turning on **Enable Tick** in the Class Defaults for your asynchronous task. By default, the tick event is turned off for asynchronous events for performance reasons.

:::caution
Latent blueprint nodes such as **Delay** do not run if the game is paused (e.g. via the **Set Game Paused** blueprint node). If you need to perform timing-based delays while the game is paused, you will need to turn on the Tick event and implement a delay manually.
:::