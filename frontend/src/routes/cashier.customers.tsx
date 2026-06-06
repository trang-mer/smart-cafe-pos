import { createFileRoute } from "@tanstack/react-router";
import { formatVnd } from "@/lib/pos-data";
import { useCustomers } from "@/lib/hooks";
import { Loader2, Search } from "lucide-react";
import { useState } from "react";

export const Route = createFileRoute("/cashier/customers")({
  component: CustomersPage,
});

function CustomersPage() {
  const [search, setSearch] = useState("");
  const { data: customers = [], isLoading } = useCustomers();

  const filtered = customers.filter(
    (c) =>
      c.name.toLowerCase().includes(search.toLowerCase()) ||
      c.phone.includes(search)
  );

  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Khách hàng</h1>

      <div className="relative mb-4">
        <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-4 h-4 text-muted-foreground" />
        <input
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          placeholder="Tìm kiếm khách hàng..."
          className="w-full h-11 pl-10 pr-3 rounded-md border bg-card"
        />
      </div>

      {isLoading ? (
        <div className="flex items-center justify-center h-64">
          <Loader2 className="w-8 h-8 animate-spin text-muted-foreground" />
        </div>
      ) : filtered.length === 0 ? (
        <div className="text-center py-12 text-muted-foreground">
          {search ? "Không tìm thấy khách hàng" : "Chưa có khách hàng nào"}
        </div>
      ) : (
        <div className="bg-card border rounded-xl overflow-hidden">
          <div className="grid grid-cols-4 px-4 py-3 text-xs font-medium text-muted-foreground border-b">
            <span>Tên</span>
            <span>Điện thoại</span>
            <span>Lượt</span>
            <span className="text-right">Tổng chi</span>
          </div>
          {filtered.map((c) => (
            <div
              key={c.id}
              className="grid grid-cols-4 px-4 py-3 text-sm border-b last:border-0 hover:bg-muted/50 cursor-pointer"
            >
              <span className="font-medium">{c.name}</span>
              <span className="text-muted-foreground">{c.phone}</span>
              <span>{c.visits}</span>
              <span className="text-right font-semibold">
                {formatVnd(c.spent)}
              </span>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
