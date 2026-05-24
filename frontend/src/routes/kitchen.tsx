import { createFileRoute } from "@tanstack/react-router";
import {
  ClipboardList,
  ChefHat,
  CheckCircle2,
  ListChecks,
  BarChart3,
} from "lucide-react";
import { useState } from "react";
import { PosShell } from "@/components/pos/PosShell";
import { orders as initialOrders, type Order } from "@/lib/pos-data";
import { cn } from "@/lib/utils";

export const Route = createFileRoute("/kitchen")({ component: KitchenPage });

function KitchenPage() {
  const nav = [
    {
      to: "/kitchen",
      label: "Order mới",
      icon: <ClipboardList className="w-4 h-4" />,
    },
    {
      to: "/kitchen",
      label: "Đang chế biến",
      icon: <ChefHat className="w-4 h-4" />,
    },
    {
      to: "/kitchen",
      label: "Đã hoàn thành",
      icon: <CheckCircle2 className="w-4 h-4" />,
    },
    {
      to: "/kitchen",
      label: "Tất cả order",
      icon: <ListChecks className="w-4 h-4" />,
    },
    {
      to: "/kitchen",
      label: "Thống kê",
      icon: <BarChart3 className="w-4 h-4" />,
    },
  ];

  return (
    <PosShell role="kitchen" nav={nav} showBell>
      <Board />
    </PosShell>
  );
}

function Board() {
  const [tab, setTab] = useState<"all" | "priority" | "normal">("all");
  const [orders, setOrders] = useState<Order[]>(initialOrders);
  const [detailId, setDetailId] = useState<string | null>(null);

  const visible = orders.filter((o) =>
    tab === "all" ? true : o.priority === tab,
  );
  const detail = orders.find((o) => o.id === detailId);

  const start = (id: string) =>
    setOrders((prev) =>
      prev.map((o) => (o.id === id ? { ...o, status: "cooking" } : o)),
    );

  return (
    <div>
      <h1 className="text-xl font-bold">Order mới</h1>

      <div className="flex gap-2 mt-4">
        <TabBtn
          active={tab === "all"}
          onClick={() => setTab("all")}
          label="Tất cả"
          count={orders.length}
        />
        <TabBtn
          active={tab === "priority"}
          onClick={() => setTab("priority")}
          label="Ưu tiên"
          count={orders.filter((o) => o.priority === "priority").length}
          tone="warning"
        />
        <TabBtn
          active={tab === "normal"}
          onClick={() => setTab("normal")}
          label="Thường"
          count={orders.filter((o) => o.priority === "normal").length}
        />
      </div>

      <div className="space-y-3 mt-4">
        {visible.map((o) => (
          <div key={o.id} className="bg-card border rounded-xl p-4">
            <div className="flex items-center gap-3">
              <div className="font-bold text-lg">{o.id}</div>
              <div className="font-semibold">{o.table}</div>
              <div className="text-sm text-muted-foreground">{o.time}</div>
              <div className="ml-auto">
                {o.priority === "priority" ? (
                  <span className="px-2.5 py-1 rounded-full text-xs font-bold bg-destructive text-destructive-foreground">
                    ƯU TIÊN
                  </span>
                ) : (
                  <span className="px-2.5 py-1 rounded-full text-xs font-medium bg-muted text-muted-foreground">
                    THƯỜNG
                  </span>
                )}
              </div>
            </div>

            <div className="mt-3 space-y-1 text-sm">
              {o.lines.map((l, i) => (
                <div key={i}>
                  {l.qty} × {l.name}
                </div>
              ))}
            </div>

            <div className="flex items-end gap-3 mt-3">
              {o.note && (
                <div className="text-xs text-muted-foreground italic flex-1">
                  {o.note}
                </div>
              )}
              <div className="ml-auto flex gap-2">
                <button
                  onClick={() => start(o.id)}
                  className="px-4 h-9 rounded-md bg-kitchen text-kitchen-foreground text-sm font-semibold hover:opacity-90"
                >
                  {o.status === "cooking" ? "Hoàn thành" : "Bắt đầu"}
                </button>
                <button
                  onClick={() => setDetailId(o.id)}
                  className="px-4 h-9 rounded-md border text-sm font-semibold hover:bg-muted"
                >
                  Chi tiết
                </button>
              </div>
            </div>
          </div>
        ))}
      </div>

      {detail && (
        <OrderDetailModal order={detail} onClose={() => setDetailId(null)} />
      )}
    </div>
  );
}

function TabBtn({
  active,
  onClick,
  label,
  count,
  tone,
}: {
  active: boolean;
  onClick: () => void;
  label: string;
  count: number;
  tone?: string;
}) {
  return (
    <button
      onClick={onClick}
      className={cn(
        "px-4 h-9 rounded-md text-sm font-medium border flex items-center gap-2",
        active
          ? "bg-kitchen text-kitchen-foreground border-kitchen"
          : "bg-card hover:bg-muted",
      )}
    >
      {label}
      <span
        className={cn(
          "px-1.5 py-0.5 rounded text-xs font-bold",
          active
            ? "bg-white/20"
            : tone === "warning"
              ? "bg-destructive text-destructive-foreground"
              : "bg-muted",
        )}
      >
        {count}
      </span>
    </button>
  );
}

function OrderDetailModal({
  order,
  onClose,
}: {
  order: Order;
  onClose: () => void;
}) {
  const steps = ["Order mới", "Đang chế biến", "Hoàn thành", "Đã thanh toán"];
  const currentStep =
    order.status === "new"
      ? 0
      : order.status === "cooking"
        ? 1
        : order.status === "done"
          ? 2
          : 3;

  return (
    <div
      className="fixed inset-0 bg-black/40 grid place-items-center z-50 p-4"
      onClick={onClose}
    >
      <div
        className="bg-card rounded-xl shadow-2xl w-full max-w-3xl max-h-[90vh] overflow-auto"
        onClick={(e) => e.stopPropagation()}
      >
        <div className="p-5 border-b flex items-center">
          <h2 className="font-bold text-lg">Chi tiết order {order.id}</h2>
          <button
            onClick={onClose}
            className="ml-auto text-xl text-muted-foreground hover:text-foreground"
          >
            ×
          </button>
        </div>
        <div className="grid grid-cols-2 gap-6 p-5">
          <div>
            <h3 className="font-semibold mb-3">Thông tin chung</h3>
            <Info label="Mã order" value={order.id} />
            <Info label="Bàn" value={order.table} />
            <Info label="Thời gian" value={`24/05/2024 - ${order.time}`} />
            <Info label="Thu ngân" value="Hoàng Anh" />
            <Info
              label="Trạng thái"
              value={
                <span className="px-2 py-0.5 rounded text-xs font-medium bg-warning/20">
                  ĐANG CHẾ BIẾN
                </span>
              }
            />
            {order.note && (
              <div className="mt-3">
                <div className="text-xs text-muted-foreground">Ghi chú</div>
                <div className="text-sm mt-1">{order.note}</div>
              </div>
            )}
          </div>

          <div>
            <h3 className="font-semibold mb-3">Danh sách món</h3>
            <div className="text-xs text-muted-foreground grid grid-cols-[1fr_60px_80px_80px] pb-2 border-b">
              <span>Món</span>
              <span className="text-center">SL</span>
              <span className="text-right">Đơn giá</span>
              <span className="text-right">Thành tiền</span>
            </div>
            {order.lines.map((l, i) => (
              <div
                key={i}
                className="grid grid-cols-[1fr_60px_80px_80px] py-2 text-sm border-b"
              >
                <span>{l.name}</span>
                <span className="text-center">{l.qty}</span>
                <span className="text-right">
                  {l.price.toLocaleString("vi-VN")}đ
                </span>
                <span className="text-right font-semibold">
                  {(l.qty * l.price).toLocaleString("vi-VN")}đ
                </span>
              </div>
            ))}
          </div>
        </div>

        <div className="p-5 border-t">
          <h3 className="font-semibold mb-4">Lịch sử trạng thái</h3>
          <div className="flex items-center">
            {steps.map((s, i) => (
              <div
                key={s}
                className="flex-1 flex flex-col items-center relative"
              >
                <div
                  className={cn(
                    "w-7 h-7 rounded-full grid place-items-center text-xs font-bold border-2",
                    i <= currentStep
                      ? "bg-kitchen border-kitchen text-kitchen-foreground"
                      : "bg-card border-border text-muted-foreground",
                  )}
                >
                  {i < currentStep ? "✓" : i + 1}
                </div>
                <div
                  className={cn(
                    "text-xs mt-2 font-medium",
                    i <= currentStep
                      ? "text-foreground"
                      : "text-muted-foreground",
                  )}
                >
                  {s}
                </div>
                <div className="text-[10px] text-muted-foreground">
                  {i === 0 ? "14:32 24/05" : i === 1 ? "14:35 24/05" : ""}
                </div>
                {i < steps.length - 1 && (
                  <div
                    className={cn(
                      "absolute top-3.5 left-1/2 w-full h-0.5",
                      i < currentStep ? "bg-kitchen" : "bg-border",
                    )}
                  />
                )}
              </div>
            ))}
          </div>
        </div>

        <div className="p-4 border-t flex justify-end">
          <button
            onClick={onClose}
            className="px-6 h-10 rounded-md bg-kitchen text-kitchen-foreground font-semibold hover:opacity-90"
          >
            Đóng
          </button>
        </div>
      </div>
    </div>
  );
}

function Info({ label, value }: { label: string; value: React.ReactNode }) {
  return (
    <div className="flex justify-between py-1.5 text-sm">
      <span className="text-muted-foreground">{label}</span>
      <span className="font-medium">{value}</span>
    </div>
  );
}
