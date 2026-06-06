import { createFileRoute } from "@tanstack/react-router";
import { formatVnd } from "@/lib/pos-data";
import { useOrders } from "@/lib/hooks";
import { Loader2 } from "lucide-react";

export const Route = createFileRoute("/cashier/history")({
  component: HistoryPage,
});

function HistoryPage() {
  const { data: orders = [], isLoading } = useOrders("paid");

  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Lịch sử order</h1>
      {isLoading ? (
        <div className="flex items-center justify-center h-64">
          <Loader2 className="w-8 h-8 animate-spin text-muted-foreground" />
        </div>
      ) : orders.length === 0 ? (
        <div className="text-center py-12 text-muted-foreground">
          Chưa có lịch sử order
        </div>
      ) : (
        <div className="bg-card border rounded-xl divide-y">
          {orders.map((r) => (
            <div key={r.id} className="p-4 flex items-center gap-4 text-sm">
              <span className="font-bold text-cashier w-16">{r.id}</span>
              <span className="w-24">{r.table}</span>
              <span className="text-muted-foreground w-16">{r.time}</span>
              <span className="flex-1">Tiền mặt</span>
              <span className="font-semibold">
                {formatVnd(r.lines.reduce((s, l) => s + l.qty * l.price, 0))}
              </span>
              <span className="px-2.5 py-1 rounded-full text-xs font-medium bg-success/15 text-success">
                Đã thanh toán
              </span>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
