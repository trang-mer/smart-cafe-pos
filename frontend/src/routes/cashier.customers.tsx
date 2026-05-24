import { createFileRoute } from "@tanstack/react-router";

export const Route = createFileRoute("/cashier/customers")({
  component: CustomersPage,
});

const customers = [
  { name: "Nguyễn Văn A", phone: "0901234567", visits: 24, spent: 2400000 },
  { name: "Trần Thị B", phone: "0907654321", visits: 18, spent: 1800000 },
  { name: "Lê Văn C", phone: "0912345678", visits: 12, spent: 980000 },
  { name: "Phạm Thị D", phone: "0934567890", visits: 8, spent: 720000 },
];

function CustomersPage() {
  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Khách hàng</h1>
      <div className="bg-card border rounded-xl overflow-hidden">
        <div className="grid grid-cols-4 px-4 py-3 text-xs font-medium text-muted-foreground border-b">
          <span>Tên</span>
          <span>Điện thoại</span>
          <span>Lượt</span>
          <span className="text-right">Tổng chi</span>
        </div>
        {customers.map((c) => (
          <div
            key={c.phone}
            className="grid grid-cols-4 px-4 py-3 text-sm border-b last:border-0"
          >
            <span className="font-medium">{c.name}</span>
            <span className="text-muted-foreground">{c.phone}</span>
            <span>{c.visits}</span>
            <span className="text-right font-semibold">
              {c.spent.toLocaleString("vi-VN")}đ
            </span>
          </div>
        ))}
      </div>
    </div>
  );
}
