import { createFileRoute } from "@tanstack/react-router";
import { Banknote, CreditCard, Wallet, Loader2 } from "lucide-react";
import { useState } from "react";
import { formatVnd } from "@/lib/pos-data";
import { cn } from "@/lib/utils";
import { useOrder, usePayOrder } from "@/lib/hooks";

export const Route = createFileRoute("/cashier/payment")({
  component: Payment,
});

function Payment() {
  const [method, setMethod] = useState<"cash" | "card" | "ewallet">("cash");
  const [received, setReceived] = useState(0);
  const [selectedOrderId, setSelectedOrderId] = useState<string | null>(null);

  const { data: order, isLoading } = useOrder(selectedOrderId || "");
  const payOrder = usePayOrder();

  const total = order?.lines.reduce((s, l) => s + l.qty * l.price, 0) || 0;
  const change = Math.max(0, received - total);

  const methods = [
    { id: "cash" as const, label: "Tiền mặt", icon: Banknote },
    { id: "card" as const, label: "Thẻ ngân hàng", icon: CreditCard },
    { id: "ewallet" as const, label: "Ví điện tử", icon: Wallet },
  ];

  const handlePay = () => {
    if (!selectedOrderId) return;
    payOrder.mutate(
      { id: selectedOrderId, method },
      {
        onSuccess: () => {
          setSelectedOrderId(null);
          setReceived(0);
        },
      }
    );
  };

  if (!selectedOrderId) {
    return (
      <div>
        <h1 className="text-xl font-bold mb-4">Thanh toán</h1>
        <p className="text-sm text-muted-foreground mb-4">
          Vui lòng chọn order từ danh sách để thanh toán
        </p>
        <a
          href="/cashier/orders"
          className="inline-flex items-center justify-center h-10 px-4 rounded-md bg-cashier text-cashier-foreground font-semibold hover:opacity-90"
        >
          Xem danh sách order
        </a>
      </div>
    );
  }

  return (
    <div>
      <h1 className="text-xl font-bold">Thanh toán</h1>
      {order && (
        <p className="text-sm text-muted-foreground">
          Order {order.id} - {order.table} · {order.time}
        </p>
      )}

      {isLoading ? (
        <div className="flex items-center justify-center h-64">
          <Loader2 className="w-8 h-8 animate-spin text-muted-foreground" />
        </div>
      ) : order ? (
        <div className="grid grid-cols-2 gap-6 mt-6">
          <div className="bg-card border rounded-xl p-5">
            <h2 className="font-semibold mb-3">Thông tin order</h2>
            {order.lines.map((l) => (
              <div
                key={l.itemId}
                className="grid grid-cols-[1fr_40px_90px] py-2 text-sm border-b"
              >
                <span>{l.name}</span>
                <span className="text-muted-foreground">x{l.qty}</span>
                <span className="text-right font-semibold">
                  {formatVnd(l.qty * l.price)}
                </span>
              </div>
            ))}
            {order.note && (
              <div className="mt-3 text-sm">
                <div className="text-muted-foreground">Ghi chú:</div>
                <div>{order.note}</div>
              </div>
            )}
            <div className="flex justify-between items-center mt-5 pt-3 border-t">
              <span className="text-sm text-muted-foreground">Tạm tính</span>
              <span className="font-semibold">{formatVnd(total)}</span>
            </div>
            <div className="flex justify-between items-center mt-2">
              <span className="font-semibold">Tổng tiền</span>
              <span className="font-bold text-2xl text-cashier">
                {formatVnd(total)}
              </span>
            </div>
          </div>

          <div className="bg-card border rounded-xl p-5">
            <h2 className="font-semibold mb-3">Phương thức thanh toán</h2>
            <div className="space-y-2">
              {methods.map((m) => {
                const Icon = m.icon;
                return (
                  <button
                    key={m.id}
                    onClick={() => setMethod(m.id)}
                    className={cn(
                      "w-full flex items-center gap-3 px-4 py-3 rounded-md border-2 text-sm font-medium transition-colors",
                      method === m.id
                        ? "border-cashier bg-cashier/5"
                        : "border-border hover:bg-muted"
                    )}
                  >
                    <Icon className="w-5 h-5" /> {m.label}
                  </button>
                );
              })}
            </div>

            {method === "cash" && (
              <>
                <label className="block text-xs font-medium text-muted-foreground mt-5">
                  Khách đưa
                </label>
                <input
                  type="number"
                  value={received}
                  onChange={(e) => setReceived(Number(e.target.value))}
                  className="mt-1 w-full h-11 px-3 rounded-md border bg-background text-right font-semibold"
                />
                <div className="flex justify-between items-center mt-4 p-3 rounded-md bg-success/10">
                  <span className="font-medium text-success">Tiền thối</span>
                  <span className="font-bold text-xl text-success">
                    {formatVnd(change)}
                  </span>
                </div>
              </>
            )}

            <button
              onClick={handlePay}
              disabled={payOrder.isPending}
              className="mt-5 w-full h-11 rounded-md bg-cashier text-cashier-foreground font-semibold hover:opacity-90 disabled:opacity-50 flex items-center justify-center gap-2"
            >
              {payOrder.isPending ? (
                <>
                  <Loader2 className="w-4 h-4 animate-spin" /> Đang xử lý...
                </>
              ) : (
                "Xác nhận thanh toán"
              )}
            </button>
            <button
              onClick={() => setSelectedOrderId(null)}
              className="mt-2 w-full h-11 rounded-md border font-semibold hover:bg-muted"
            >
              Hủy bỏ
            </button>
          </div>
        </div>
      ) : (
        <div className="text-center py-12 text-muted-foreground">
          Order không tìm thấy
        </div>
      )}
    </div>
  );
}
