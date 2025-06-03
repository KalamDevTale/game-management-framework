module.exports = {
  docs: [
    "welcome",
    {
      type: "category",
      label: "Game Managers",
      items: ['game_managers_creating', 'game_managers_configuration', 'game_managers_accessing'],
    },
    {
      type: "category",
      label: "Async Tasks",
      items: ['async_tasks_creating', 'async_tasks_scheduling'],
    },
    {
      type: "category",
      label: "Loading Screens",
      items: ['loading_screens_creating', 'loading_screens_configuration', 'loading_screens_tasks'],
    },
    "changelog",
  ],
};
