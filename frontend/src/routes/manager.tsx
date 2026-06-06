import { createFileRoute } from "@tanstack/react-router";
import {
  LayoutDashboard,
  DollarSign,
  ClipboardList,
  UtensilsCrossed,
  Users,
  UsersRound,
  FileText,
  Settings,
  TrendingUp,
  Loader2,
} from "lucide-react";
import {
  LineChart,
  Line,
  BarChart,
  Bar,
  PieChart,
  Pie,
  Cell,
  XAxis,
  YAxis,
  CartesianGrid,
  ResponsiveContainer,
  Tooltip,
} from "recharts";
import { PosShell } from "@/components/pos/PosShell";
import { formatVnd } from "@/lib/pos-data";
import {
  useRevenueStats,
  useTopItems,
  useOrderStats,
  useRevenueByHour,
} from "@/lib/hooks";

export const Route = createFileRoute("/manager")({ component: ManagerPage });

function ManagerPage() {
  const nav = [
    {
      to: "/manager",
      label: "Tổng quan",
      icon: <LayoutDashboard className="w-4 h-4" />,
    },
    {
      to: "/manager",
      label: "Doanh thu",
      icon: <DollarSign className="w-4 h-4" />,
    },
    {
      to: "/manager",
      label: "Order",
      icon: <ClipboardList className="w-4 h-4" />,
    },
    {
      to: "/manager",
      label: "Menu",
      icon: <UtensilsCrossed className="w-4 h-4" />,
    },
    { to: "/manager", label: "Nhân viên", icon: <Users className="w-4 h-4" /> },
    {
      to: "/manager",
      label: "Khách hàng",
      icon: <UsersRound className="w-4 h-4" />,
    },
    {
      to: "/manager",
      label: "Báo cáo",
      icon: <FileText className="w-4 h-4" />,
    },
    {
      to: "/manager",
      label: "Cài đặt",
      icon: <Settings className="w-4 h-4" />,
    },
  ];

  return (
    <PosShell role="manager" nav={nav}>
      <Dashboard />
    </PosShell>
  );
}

function Dashboard() {
  const { data: revenue = [], isLoading: revenueLoading } = useRevenueStats("week");
  const { data: topItems = [], isLoading: topItemsLoading } = useTopItems();
  const { data: orderStats, isLoading: orderStatsLoading } = useOrderStats();
  const { data: revenueByHour = [], isLoading: revenueByHourLoading } = useRevenueByHour();

  const todayRevenue = revenue.length > 0 ? revenue[revenue.length - 1].value : 0;
  const totalOrders = orderStats?.total || 0;

  const kpis = [
    {
      label: "Doanh thu hôm nay",
      value: formatVnd(todayRevenue),
      delta: "+12.5%",
    },
    { label: "Order hôm nay", value: totalOrders.toString(), delta: "+8.3%" },
    { label: "Khách hàng", value: "98", delta: "+5.2%" },
    { label: "Đơn trung bình", value: "98.125đ", delta: "+3.1%" },
  ];

  const orderStatusBreakdown = orderStats
    ? [
        { name: "Mới", value: orderStats.new, color: "var(--color-cashier)" },
        {
          name: "Đang làm",
          value: orderStats.cooking,
          color: "var(--color-warning)",
        },
        {
          name: "Hoàn thành",
          value: orderStats.done,
          color: "var(--color-kitchen)",
        },
        {
          name: "Đã hủy",
          value: orderStats.cancelled,
          color: "var(--color-destructive)",
        },
      ]
    : [];

  const chartData = revenue.map((r) => ({ d: r.date, v: r.value }));
  const hourData = revenueByHour.map((r) => ({
    h: r.hour,
    v: r.value,
  }));

  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Tổng quan</h1>

      <div className="grid grid-cols-4 gap-4">
        {kpis.map((k) => (
          <div key={k.label} className="bg-card border rounded-xl p-4">
            <div className="text-xs text-muted-foreground">{k.label}</div>
            <div className="text-2xl font-bold mt-1 text-manager">
              {k.value}
            </div>
            <div className="text-xs text-success mt-1 flex items-center gap-1">
              <TrendingUp className="w-3 h-3" /> {k.delta}{" "}
              <span className="text-muted-foreground">so với hôm qua</span>
            </div>
          </div>
        ))}
      </div>

      <div className="grid grid-cols-[1fr_360px] gap-4 mt-4">
        <div className="bg-card border rounded-xl p-4">
          <div className="flex items-center mb-3">
            <h2 className="font-semibold">Doanh thu 7 ngày qua</h2>
            <select className="ml-auto text-xs h-8 px-2 rounded border bg-background">
              <option>7 ngày</option>
              <option>30 ngày</option>
            </select>
          </div>
          {revenueLoading ? (
            <div className="flex items-center justify-center h-64">
              <Loader2 className="w-8 h-8 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="h-64">
              <ResponsiveContainer width="100%" height="100%">
                <LineChart data={chartData}>
                  <CartesianGrid
                    strokeDasharray="3 3"
                    stroke="var(--color-border)"
                  />
                  <XAxis
                    dataKey="d"
                    stroke="var(--color-muted-foreground)"
                    fontSize={11}
                  />
                  <YAxis
                    stroke="var(--color-muted-foreground)"
                    fontSize={11}
                    tickFormatter={(v) => `${v / 1000000}tr`}
                  />
                  <Tooltip
                    formatter={(v: number) => formatVnd(v)}
                    contentStyle={{
                      background: "var(--color-card)",
                      border: "1px solid var(--color-border)",
                      borderRadius: 8,
                    }}
                  />
                  <Line
                    type="monotone"
                    dataKey="v"
                    stroke="var(--color-manager)"
                    strokeWidth={2.5}
                    dot={{ r: 4 }}
                  />
                </LineChart>
              </ResponsiveContainer>
            </div>
          )}
        </div>

        <div className="bg-card border rounded-xl p-4">
          <h2 className="font-semibold mb-3">Top món bán chạy</h2>
          {topItemsLoading ? (
            <div className="flex items-center justify-center h-48">
              <Loader2 className="w-6 h-6 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="space-y-2">
              {topItems.map((it, i) => (
                <div key={it.name} className="flex items-center gap-3 text-sm">
                  <span className="w-5 text-muted-foreground">{i + 1}</span>
                  <span className="flex-1">{it.name}</span>
                  <span className="font-semibold">{it.count}</span>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

      <div className="grid grid-cols-2 gap-4 mt-4">
        <div className="bg-card border rounded-xl p-4">
          <h2 className="font-semibold mb-3">Order theo trạng thái</h2>
          {orderStatsLoading ? (
            <div className="flex items-center justify-center h-52">
              <Loader2 className="w-6 h-6 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="flex items-center gap-6">
              <div className="relative w-44 h-44">
                <ResponsiveContainer width="100%" height="100%">
                  <PieChart>
                    <Pie
                      data={orderStatusBreakdown}
                      dataKey="value"
                      innerRadius={55}
                      outerRadius={80}
                      paddingAngle={2}
                    >
                      {orderStatusBreakdown.map((e, i) => (
                        <Cell key={i} fill={e.color} />
                      ))}
                    </Pie>
                  </PieChart>
                </ResponsiveContainer>
                <div className="absolute inset-0 grid place-items-center pointer-events-none">
                  <div className="text-center">
                    <div className="text-2xl font-bold">{totalOrders}</div>
                    <div className="text-xs text-muted-foreground">
                      Tổng order
                    </div>
                  </div>
                </div>
              </div>
              <div className="space-y-2 text-sm">
                {orderStatusBreakdown.map((s) => (
                  <div key={s.name} className="flex items-center gap-2">
                    <span
                      className="w-3 h-3 rounded-sm"
                      style={{ background: s.color }}
                    />
                    <span>{s.name}</span>
                    <span className="text-muted-foreground ml-2">
                      {s.value} ({Math.round((s.value / totalOrders) * 100)}%)
                    </span>
                  </div>
                ))}
              </div>
            </div>
          )}
        </div>

        <div className="bg-card border rounded-xl p-4">
          <h2 className="font-semibold mb-3">Doanh thu theo giờ</h2>
          {revenueByHourLoading ? (
            <div className="flex items-center justify-center h-52">
              <Loader2 className="w-6 h-6 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="h-52">
              <ResponsiveContainer width="100%" height="100%">
                <BarChart data={hourData}>
                  <CartesianGrid
                    strokeDasharray="3 3"
                    stroke="var(--color-border)"
                  />
                  <XAxis
                    dataKey="h"
                    stroke="var(--color-muted-foreground)"
                    fontSize={10}
                  />
                  <YAxis
                    stroke="var(--color-muted-foreground)"
                    fontSize={10}
                    tickFormatter={(v) => `${v / 1000000}tr`}
                  />
                  <Tooltip
                    formatter={(v: number) => formatVnd(v)}
                    contentStyle={{
                      background: "var(--color-card)",
                      border: "1px solid var(--color-border)",
                      borderRadius: 8,
                    }}
                  />
                  <Bar
                    dataKey="v"
                    fill="var(--color-manager)"
                    radius={[4, 4, 0, 0]}
                  />
                </BarChart>
              </ResponsiveContainer>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
