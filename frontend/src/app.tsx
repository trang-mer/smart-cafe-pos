import {
  createRouter as createTanStackRouter,
  HeadContent,
  Link,
  Outlet,
  Scripts,
  createRootRoute,
  createRoute,
  createRouter,
} from "@tanstack/react-router";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { HeadContent } from "@tanstack/react-start/client";
import "./styles.css";

export const routeTree = {
  root: createRootRoute({
    component: () => (
      <div>
        <Outlet />
        <Scripts />
      </div>
    ),
    head: () => (
      <>
        <title>Smart Cafe POS</title>
        <HeadContent />
      </>
    ),
  }),
  index: createRootRoute({
    getParentRoute: () => routeTree.root,
    component: () => <div>Welcome to Smart Cafe POS</div>,
  }),
};

const rootRoute = createRootRoute({});
