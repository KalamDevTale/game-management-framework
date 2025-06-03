---
id: async_tasks_scheduling
title: Scheduling an asynchronous task
description: How to schedule a task from synchronous code.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

When you want to use your asynchronous tasks, use the **Schedule Game Manager Task** blueprint node from the action dropdown:

<img alt="Locate the Schedule Game Manager Task in the blueprint dropdown" src={useBaseUrl('img/async_task_scheduling_step_2.png')} />

For **Class**, find and select the game manager task you want to schedule.

For **Task Id**, this is the unique ID of the task. Because the synchronous code might run repeatedly while the task is still in progress, this ID is used to ensure that the task is only scheduled once, and that the result is cached once it's complete. Usually this should be some unique hash or ID that represents the input parameters of the task.

If you are calling **Schedule Game Manager Task** outside of a [Game Manager](game_managers_creating), you will need to provide a game manager reference to the **Self** parameter. This is because asynchronous tasks are scheduled and cached at a game manager level.

:::info
If you add variables to your asynchronous task and mark them as **Expose on Spawn**, they will appear on the **Schedule Game Manager Task** node for you to set.
:::

Once you've added the node for scheduling the task, you can perform different behaviour based on whether or not the task result is **Ready**. Tasks become ready when the asynchronous task calls **Mark Task as Complete**.

:::caution
The synchronous code must call the **Schedule Task** node over multiple frames in order for **Ready** to fire.
:::

<img alt="Example of using the schedule blueprint node" src={useBaseUrl('img/async_task_scheduling_step_1.png')} />