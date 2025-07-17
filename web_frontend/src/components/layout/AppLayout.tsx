import React, { ReactNode } from 'react';
import Sidebar from '../ui/Sidebar';
import Header from '../ui/Header';
import './AppLayout.css';

interface AppLayoutProps {
  children: ReactNode;
}

const AppLayout: React.FC<AppLayoutProps> = ({ children }) => {
  return (
    <div className="app-layout">
      <Sidebar />
      <div className="main-content">
        <Header />
        <main className="content">
          {children}
        </main>
      </div>
    </div>
  );
};

export default AppLayout;