import { createFileRoute } from "@tanstack/react-router";
import { formatVnd } from "@/lib/pos-data";
import { useOrders } from "@/lib/hooks";
import { Loader2 } from "lucide-react";

export const Route = createFileRoute("/cashier/orders")({
  component: OrdersPage,
});

function OrdersPage() {
  const { data: orders = [], isLoading } = useOrders();

  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Danh sách order</h1>
      {isLoading ? (
        <div className="flex items-center justify-center h-64">
          <Loader2 className="w-8 h-8 animate-spin text-muted-foreground" />
        </div>
      ) : orders.length === 0 ? (
        <div className="text-center py-12 text-muted-foreground">
          Chưa có order nào
        </div>
      ) : (
        <div className="bg-card border rounded-xl divide-y">
          {orders.map((o) => {
            const total = o.lines.reduce((s, l) => s + l.qty * l.price, 0);
            return (
              <div key={o.id} className="p-4 flex items-center gap-4">
                <div className="font-bold text-cashier w-16">{o.id}</div>
                <div className="w-20 text-sm">{o.table}</div>
                <div className="text-sm text-muted-foreground w-16">{o.time}</div>
                <div className="flex-1 text-sm text-muted-foreground truncate">
                  {o.lines.map((l) => `${l.qty}× ${l.name}`).join(", ")}
                </div>
                <div className="font-semibold">{formatVnd(total)}</div>
                <span className="px-2.5 py-1 rounded-full text-xs font-medium bg-warning/20 text-warning-foreground">
                  {o.status === "new"
                    ? "Mới"
                    : o.status === "cooking"
                      ? "Đang chế biến"
                      : o.status === "done"
                        ? "Hoàn thành"
                        : "Đã thanh toán"}
                </span>
              </div>
            );
          })}
        </div>
      )}
    </div>
  );
}
