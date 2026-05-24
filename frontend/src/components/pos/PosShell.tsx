import { Link, useLocation } from "@tanstack/react-router";
import { Menu, Bell, LogOut } from "lucide-react";
import { type ReactNode } from "react";
import { cn } from "@/lib/utils";

export type Role = "cashier" | "kitchen" | "manager";

const roleConfig: Record<
  Role,
  { label: string; sub: string; bar: string; user: string }
> = {
  cashier: {
    label: "Thu ngân",
    sub: "Hoàng Anh",
    bar: "bg-cashier text-cashier-foreground",
    user: "HA",
  },
  kitchen: {
    label: "Bếp",
    sub: "Minh Hùng",
    bar: "bg-kitchen text-kitchen-foreground",
    user: "MH",
  },
  manager: {
    label: "Quản lý",
    sub: "Nguyễn Nam",
    bar: "bg-manager text-manager-foreground",
    user: "NN",
  },
};

export interface NavItem {
  to: string;
  label: string;
  icon: ReactNode;
}

export function PosShell({
  role,
  nav,
  children,
  showBell,
}: {
  role: Role;
  nav: NavItem[];
  children: ReactNode;
  showBell?: boolean;
}) {
  const cfg = roleConfig[role];
  const location = useLocation();

  return (
    <div className="min-h-screen flex flex-col bg-background">
      <header
        className={cn("h-16 flex items-center px-4 gap-3 shadow-sm", cfg.bar)}
      >
        <Menu className="w-5 h-5 opacity-90" />
        <div className="font-bold tracking-wide">SMART CAFE POS</div>
        <div className="ml-auto flex items-center gap-3">
          {showBell && <Bell className="w-5 h-5 opacity-90" />}
          <div className="text-right text-sm leading-tight">
            <div className="opacity-90">{cfg.label}</div>
            <div className="font-semibold">{cfg.sub}</div>
          </div>
          <div className="w-9 h-9 rounded-full bg-white/20 grid place-items-center font-semibold">
            {cfg.user}
          </div>
        </div>
      </header>

      <div className="flex flex-1">
        <aside className="w-56 border-r bg-card py-4">
          <nav className="flex flex-col gap-1 px-2">
            {nav.map((item) => {
              const active = location.pathname === item.to;
              return (
                <Link
                  key={item.to}
                  to={item.to}
                  className={cn(
                    "flex items-center gap-3 px-3 py-2.5 rounded-md text-sm font-medium transition-colors",
                    active
                      ? `bg-${role}/10 text-${role}`
                      : "text-muted-foreground hover:bg-muted hover:text-foreground",
                  )}
                  style={
                    active
                      ? {
                          backgroundColor: `color-mix(in oklab, var(--color-${role}) 12%, transparent)`,
                          color: `var(--color-${role})`,
                        }
                      : undefined
                  }
                >
                  {item.icon}
                  {item.label}
                </Link>
              );
            })}
            <Link
              to="/"
              className="flex items-center gap-3 px-3 py-2.5 rounded-md text-sm font-medium text-muted-foreground hover:bg-muted hover:text-foreground mt-4"
            >
              <LogOut className="w-4 h-4" /> Đăng xuất
            </Link>
          </nav>
        </aside>

        <main className="flex-1 p-6 overflow-auto">{children}</main>
      </div>
    </div>
  );
}
