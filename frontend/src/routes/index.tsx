import { createFileRoute, Link } from "@tanstack/react-router";
import { Coffee, User, ChefHat, BarChart3, Eye } from "lucide-react";
import { useState } from "react";
import { cn } from "@/lib/utils";

export const Route = createFileRoute("/")({
  component: Login,
  head: () => ({ meta: [{ title: "Smart Cafe POS — Đăng nhập" }] }),
});

type Role = "cashier" | "kitchen" | "manager";

function Login() {
  const [role, setRole] = useState<Role>("cashier");
  const [showPw, setShowPw] = useState(false);
  const targets: Record<Role, string> = {
    cashier: "/cashier",
    kitchen: "/kitchen",
    manager: "/manager",
  };

  const roleCards: {
    id: Role;
    label: string;
    icon: typeof User;
    tone: string;
  }[] = [
    { id: "cashier", label: "CASHIER", icon: User, tone: "cashier" },
    { id: "kitchen", label: "KITCHEN", icon: ChefHat, tone: "kitchen" },
    { id: "manager", label: "MANAGER", icon: BarChart3, tone: "manager" },
  ];

  return (
    <div className="min-h-screen grid lg:grid-cols-2 bg-background">
      <div className="relative hidden lg:flex flex-col justify-between p-10 bg-gradient-to-br from-amber-50 to-amber-100">
        <div className="flex items-center gap-3">
          <div className="w-11 h-11 rounded-xl bg-primary text-primary-foreground grid place-items-center">
            <Coffee className="w-6 h-6" />
          </div>
          <div>
            <div className="font-bold tracking-wide text-primary">
              SMART CAFE
            </div>
            <div className="text-xs text-muted-foreground">POS SYSTEM</div>
          </div>
        </div>
        <div className="flex-1 grid place-items-center">
          <div className="text-[10rem] leading-none select-none">☕</div>
        </div>
        <div>
          <h2 className="text-2xl font-bold text-primary">Chào mừng bạn!</h2>
          <p className="text-sm text-muted-foreground mt-1">
            Đăng nhập để tiếp tục sử dụng hệ thống
          </p>
          <div className="text-xs text-muted-foreground mt-6">
            © 2024 Smart Cafe POS
          </div>
        </div>
      </div>

      <div className="flex items-center justify-center p-8">
        <div className="w-full max-w-md">
          <h1 className="text-3xl font-bold text-center">Đăng nhập</h1>
          <p className="text-center text-muted-foreground mt-1">
            Chọn vai trò để đăng nhập
          </p>

          <div className="grid grid-cols-3 gap-3 mt-8">
            {roleCards.map(({ id, label, icon: Icon, tone }) => (
              <button
                key={id}
                onClick={() => setRole(id)}
                className={cn(
                  "rounded-xl border-2 p-4 flex flex-col items-center gap-2 transition-all",
                  role === id
                    ? "border-current shadow-md scale-105"
                    : "border-border hover:border-muted-foreground/40",
                )}
                style={{
                  color: role === id ? `var(--color-${tone})` : undefined,
                }}
              >
                <div
                  className="w-12 h-12 rounded-lg grid place-items-center"
                  style={{
                    backgroundColor: `color-mix(in oklab, var(--color-${tone}) 18%, transparent)`,
                    color: `var(--color-${tone})`,
                  }}
                >
                  <Icon className="w-6 h-6" />
                </div>
                <span className="text-xs font-semibold">{label}</span>
              </button>
            ))}
          </div>

          <div className="mt-6 space-y-4">
            <div>
              <label className="text-sm font-medium">Tên đăng nhập</label>
              <input
                className="mt-1.5 w-full h-11 px-3 rounded-md border bg-background"
                placeholder="Nhập tên đăng nhập"
                defaultValue="admin"
              />
            </div>
            <div>
              <label className="text-sm font-medium">Mật khẩu</label>
              <div className="relative mt-1.5">
                <input
                  type={showPw ? "text" : "password"}
                  className="w-full h-11 px-3 pr-10 rounded-md border bg-background"
                  placeholder="Nhập mật khẩu"
                  defaultValue="password"
                />
                <button
                  type="button"
                  onClick={() => setShowPw((v) => !v)}
                  className="absolute right-2 top-1/2 -translate-y-1/2 p-2 text-muted-foreground"
                >
                  <Eye className="w-4 h-4" />
                </button>
              </div>
            </div>
            <label className="flex items-center gap-2 text-sm text-muted-foreground">
              <input type="checkbox" className="rounded" /> Ghi nhớ đăng nhập
            </label>
            <Link
              to={targets[role]}
              className="block w-full h-11 rounded-md font-semibold grid place-items-center text-primary-foreground"
              style={{ backgroundColor: `var(--color-${role})` }}
            >
              Đăng nhập
            </Link>
            <div className="text-center text-sm text-muted-foreground">
              <a href="#" className="hover:text-foreground">
                Quên mật khẩu?
              </a>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
