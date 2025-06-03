const configCommon = {
  getFooter: function () {
    return {
      style: 'dark',
      copyright: `Copyright © ${new Date().getFullYear()} Redpoint Games.`,
    };
  },
}

module.exports = {
  title: 'Game Management Framework',
  tagline: 'Unreal Engine plugin for building subsystems in blueprints, asynchronous tasks in synchronous code, and loading screens.',
  url: 'https://docs.redpoint.games',
  baseUrl: '/game-management-framework/',
  favicon: 'favicon.png',
  organizationName: 'redpointgames',
  projectName: 'game-management-framework',
  themeConfig: {
    navbar: {
      title: 'Game Management Framework',
      logo: {
        alt: 'Unreal Engine',
        src: 'img/GMFB.png',
        srcDark: 'img/GMFW.png',
      },
      items: [
        {
          to: 'docs/',
          activeBasePath: 'docs/welcome',
          label: 'Documentation',
          position: 'left',
        },
        {
          href: 'https://src.redpoint.games/redpointgames/examples/',
          label: 'Example Project',
          position: 'right',
        },
        {
          href: 'https://redpoint.games/support',
          label: 'Support',
          position: 'right',
        },
      ],
    },
    footer: configCommon.getFooter(),
  },
  presets: [
    [
      '@docusaurus/preset-classic',
      {
        docs: {
          sidebarPath: require.resolve('./sidebars.js'),
        },
        theme: {
          customCss: require.resolve('./src/css/custom.css'),
        },
      },
    ],
  ],
};
