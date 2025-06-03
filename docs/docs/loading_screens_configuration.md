---
id: loading_screens_configuration
title: Set the loading screen for your project
description: How to set the loading screen your project should use.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

You need to set the loading screen your project will use in the project configuration.

## Set the loading screen

Open **Project Settings** and locate **Game 🡒 Game Management Framework**. Then set **Loading Screen** to the name of the loading screen blueprint you just created:

<img alt="Set the Loading Screen configuration" src={useBaseUrl('img/loading_screen_config_step_1.png')} />

## Include the loading screen in packaging

You may need to include the loading screen in the asset manager configuration if you find it's not being packaged in builds.

Under **Game 🡒 Asset Manager 🡒 Primary Asset Types to Scan**, add the following entry:

<img alt="Tell the Unreal Engine Asset Manager to include all loading screens in your built project" src={useBaseUrl('img/loading_screen_config_step_2.png')} />
