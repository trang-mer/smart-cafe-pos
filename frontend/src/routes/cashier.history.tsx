import { createFileRoute } from "@tanstack/react-router";
import { formatVnd } from "@/lib/pos-data";

export const Route = createFileRoute("/cashier/history")({
  component: HistoryPage,
});

const rows = [
  {
    id: "#20",
    table: "Bàn 01",
    time: "14:10",
    total: 120000,
    method: "Tiền mặt",
  },
  {
    id: "#19",
    table: "Mang đi",
    time: "13:55",
    total: 75000,
    method: "Ví điện tử",
  },
  {
    id: "#18",
    table: "Bàn 04",
    time: "13:40",
    total: 210000,
    method: "Thẻ ngân hàng",
  },
  {
    id: "#17",
    table: "Bàn 02",
    time: "13:22",
    total: 95000,
    method: "Tiền mặt",
  },
];

function HistoryPage() {
  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Lịch sử order</h1>
      <div className="bg-card border rounded-xl divide-y">
        {rows.map((r) => (
          <div key={r.id} className="p-4 flex items-center gap-4 text-sm">
            <span className="font-bold text-cashier w-16">{r.id}</span>
            <span className="w-24">{r.table}</span>
            <span className="text-muted-foreground w-16">{r.time}</span>
            <span className="flex-1">{r.method}</span>
            <span className="font-semibold">{formatVnd(r.total)}</span>
            <span className="px-2.5 py-1 rounded-full text-xs font-medium bg-success/15 text-success">
              Đã thanh toán
            </span>
          </div>
        ))}
      </div>
    </div>
  );
}
