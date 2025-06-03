---
id: loading_screens_tasks
title: Enqueue tasks and show the loading screen
description: How to enqueue asynchronous tasks and show the loading screen while those tasks are running
---

import useBaseUrl from '@docusaurus/useBaseUrl';

Now that you have your loading screen configured, you can queue up asynchronous tasks to be performed while a loading screen is present.

The loading screen framework leverages the same task system that the task scheduler does, except in this case you use **Enqueue Game Manager Task** instead of **Schedule Game Manager Task**.

<img alt="Queue up tasks" src={useBaseUrl('img/loading_screen_enqueue_step_1.png')} />

As with **Schedule Game Manager Task**, you will need a reference to a game manager to queue up tasks. When you queue up a task, you should check the **Show Loading Screen** option.

**Enqueue Game Manager Task** returns immediately (when the task has been queued up). There is no output execution pin for when the task completes.

:::info
You can also queue up tasks to run in the background without a loading screen, by leaving the **Show Loading Screen** option unticked. This can be useful if you have tasks that need to complete in sequence, even when you don't want to show a loading screen. In contrast, if you schedule tasks with **Schedule Game Manager Task** they will always run in parallel, regardless of when they were scheduled.
:::