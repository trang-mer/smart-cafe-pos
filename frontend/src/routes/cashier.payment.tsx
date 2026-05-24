import { createFileRoute } from "@tanstack/react-router";
import { Banknote, CreditCard, Wallet } from "lucide-react";
import { useState } from "react";
import { formatVnd } from "@/lib/pos-data";
import { cn } from "@/lib/utils";

export const Route = createFileRoute("/cashier/payment")({
  component: Payment,
});

function Payment() {
  const [method, setMethod] = useState<"cash" | "card" | "ewallet">("cash");
  const [received, setReceived] = useState(200000);
  const lines = [
    { name: "Latte", qty: 2, price: 40000 },
    { name: "Matcha Latte", qty: 1, price: 40000 },
    { name: "Cookies", qty: 1, price: 20000 },
  ];
  const total = lines.reduce((s, l) => s + l.qty * l.price, 0);
  const change = Math.max(0, received - total);

  const methods = [
    { id: "cash" as const, label: "Tiền mặt", icon: Banknote },
    { id: "card" as const, label: "Thẻ ngân hàng", icon: CreditCard },
    { id: "ewallet" as const, label: "Ví điện tử", icon: Wallet },
  ];

  return (
    <div>
      <h1 className="text-xl font-bold">Thanh toán</h1>
      <p className="text-sm text-muted-foreground">
        Order #23 - Bàn 03 · 14:32 - 24/05/2024
      </p>

      <div className="grid grid-cols-2 gap-6 mt-6">
        <div className="bg-card border rounded-xl p-5">
          <h2 className="font-semibold mb-3">Thông tin order</h2>
          {lines.map((l) => (
            <div
              key={l.name}
              className="grid grid-cols-[1fr_40px_90px] py-2 text-sm border-b"
            >
              <span>{l.name}</span>
              <span className="text-muted-foreground">x{l.qty}</span>
              <span className="text-right font-semibold">
                {formatVnd(l.qty * l.price)}
              </span>
            </div>
          ))}
          <div className="mt-3 text-sm">
            <div className="text-muted-foreground">Ghi chú:</div>
            <div>Ít đá, thêm đường</div>
          </div>
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
                      : "border-border hover:bg-muted",
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

          <button className="mt-5 w-full h-11 rounded-md bg-cashier text-cashier-foreground font-semibold hover:opacity-90">
            Xác nhận thanh toán
          </button>
          <button className="mt-2 w-full h-11 rounded-md border font-semibold hover:bg-muted">
            Hủy bỏ
          </button>
        </div>
      </div>
    </div>
  );
}
