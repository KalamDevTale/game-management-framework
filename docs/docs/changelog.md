---
id: changelog
title: Changelog
description: Release notes for Game Management Framework.
---

This document describes the changes made in each release.

### 2024.11.29

This release fixes a crash that could occur when cooking projects.

### 2024.11.26

This release adds support for Unreal Engine 5.5 and fixes several issues:

- Added support for Unreal Engine 5.5.
- Fixed an issue where multiple loading screens would be created for enqueued tasks, instead of the first loading screen being re-used as tasks complete.
- Improved the behaviour of refreshing blueprint graphs and auto-completion when the configuration of game managers is changed in Project Settings.

### 2024.04.25

This release adds support for Unreal Engine 5.4.

### 2023.09.08

This release adds support for Unreal Engine 5.3.

### 2023.05.14

This release adds support for Unreal Engine 5.2.

### 2022.11.22

This release adds support for Unreal Engine 5.1.

### 2022.04.07

This release adds support for Unreal Engine 5.0.

### 2022.02.08

This release removed the "EnabledByDefault" flag in the `.uplugin` file, which fixed issues when using the plugin from source control.

### 2021.08.21

This release adds support for Unreal Engine 4.27.

### 2021.01.13

This release adds support for Unreal Engine 4.26.

### 2020.09.18

- Updated to the new versioning scheme.
- Updated copyright headers in the source code.
- Fixed category errors that would appear when using Game Management Framework from the Marketplace in a C++ project.
- Fixed a few compilation warnings.

### 1.25.4 (2nd July)

Fixed an issue which prevented blueprints from executing correctly when running a Standalone Game from inside the editor (via the Play dropdown).

### 1.25.3 (28th June)

Asynchronous tasks can now have a Tick event turned on. Refer to [the documentation](async_tasks_creating#enabling-the-tick-event) on how to use this feature.

### 1.25.2

Updated the plugin so the logo is visible in the Plugins window.

### 1.25.1

Initial release.