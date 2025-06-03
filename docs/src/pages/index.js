import React from 'react';
import classnames from 'classnames';
import Layout from '@theme/Layout';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import useBaseUrl from '@docusaurus/useBaseUrl';
import styles from './styles.module.css';

function Home() {
  const context = useDocusaurusContext();
  const {siteConfig = {}} = context;
  return (
    <Layout
      title={`${siteConfig.title} for Unreal Engine`}
      description="Documentation for the Game Management Framework plugin for Unreal Engine">
      <header className={classnames('hero hero--primary', styles.heroBanner)}>
        <div className="container">
          <h1 className="hero__title">{siteConfig.title}</h1>
          <p className="hero__subtitle">{siteConfig.tagline}</p>
          <div className={styles.buttons}>
            <Link
              className={classnames(
                'button button--secondary button--lg',
                styles.getStarted,
              )}
              style={{marginRight: '1em'}}
              to={"https://www.unrealengine.com/marketplace/en-US/product/631d5ee26def4ec69565285b9889798a"}>
              Buy on Marketplace
            </Link>
            <Link
              className={classnames(
                'button button--secondary button--lg',
                styles.getStarted,
              )}
              to={useBaseUrl('docs/')}>
              Get Started
            </Link>
          </div>
        </div>
      </header>
      <main>
          <section>
            <div className="container">
              <div className="row">
                <div className="col-12">
                <p>Game Management Framework makes it easy to manage asynchronous tasks, loading screens and game-wide logic in your Unreal Engine game.</p>
                <h2>Game Managers</h2>
                <p>Game Managers act as subsystems, except you can create them in blueprints and you can configure them through Project Settings.</p>
                <a href="img/game_manager_demo.png" target="_blank"><img src="img/game_manager_demo.png" /></a>
                <h2>Asynchronous Tasks</h2>
                <p>Ever wished you could call Download Image from inside a UMG Get Brush callback to download user avatars on-demand? Game Management Framework allows you to create asynchronous tasks and then use them from synchronous code, letting you handle the result differently based on whether or not the task has completed.</p>
                <a href="img/async_demo.png" target="_blank"><img src="img/async_demo.png" /></a>
                <h2>Loading Screens</h2>
                <p>Leveraging the asynchronous tasks feature, you can also show a loading screen while tasks are running.</p>
                <a href="img/loading_screen_demo.png" target="_blank"><img src="img/loading_screen_demo.png" /></a>
                </div>
              </div>
            </div>
          </section>
      </main>
    </Layout>
  );
}

export default Home;
