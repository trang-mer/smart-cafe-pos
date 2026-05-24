import { createFileRoute } from "@tanstack/react-router";
import { Search, Trash2 } from "lucide-react";
import { useMemo, useState } from "react";
import { categories, formatVnd, menu, type Category } from "@/lib/pos-data";
import { cn } from "@/lib/utils";

export const Route = createFileRoute("/cashier/new")({ component: NewOrder });

interface Line {
  id: string;
  name: string;
  qty: number;
  price: number;
}

function NewOrder() {
  const [cat, setCat] = useState<Category>("all");
  const [q, setQ] = useState("");
  const [table, setTable] = useState("Bàn 03");
  const [note, setNote] = useState("Ít đá, thêm đường");
  const [lines, setLines] = useState<Line[]>([
    { id: "latte", name: "Latte", qty: 2, price: 40000 },
    { id: "matcha", name: "Matcha Latte", qty: 1, price: 40000 },
    { id: "cookies", name: "Cookies", qty: 1, price: 20000 },
  ]);

  const filtered = useMemo(
    () =>
      menu.filter(
        (m) =>
          (cat === "all" || m.category === cat) &&
          m.name.toLowerCase().includes(q.toLowerCase()),
      ),
    [cat, q],
  );

  const total = lines.reduce((s, l) => s + l.qty * l.price, 0);

  const addItem = (id: string) => {
    const m = menu.find((x) => x.id === id)!;
    setLines((prev) => {
      const ex = prev.find((l) => l.id === id);
      if (ex)
        return prev.map((l) => (l.id === id ? { ...l, qty: l.qty + 1 } : l));
      return [...prev, { id, name: m.name, qty: 1, price: m.price }];
    });
  };
  const remove = (id: string) =>
    setLines((prev) => prev.filter((l) => l.id !== id));

  return (
    <div className="grid grid-cols-[1fr_360px] gap-6">
      <div>
        <h1 className="text-xl font-bold mb-4">Tạo order mới</h1>

        <div className="relative">
          <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-4 h-4 text-muted-foreground" />
          <input
            value={q}
            onChange={(e) => setQ(e.target.value)}
            placeholder="Tìm kiếm món..."
            className="w-full h-11 pl-10 pr-3 rounded-md border bg-card"
          />
        </div>

        <div className="flex gap-2 mt-4 flex-wrap">
          {categories.map((c) => (
            <button
              key={c.id}
              onClick={() => setCat(c.id)}
              className={cn(
                "px-4 h-9 rounded-full text-sm font-medium border transition-colors",
                cat === c.id
                  ? "bg-cashier text-cashier-foreground border-cashier"
                  : "bg-card hover:bg-muted",
              )}
            >
              {c.label}
            </button>
          ))}
        </div>

        <div className="grid grid-cols-4 gap-4 mt-5">
          {filtered.map((m) => (
            <button
              key={m.id}
              onClick={() => addItem(m.id)}
              className="bg-card rounded-xl overflow-hidden border hover:shadow-md hover:-translate-y-0.5 transition-all text-left"
            >
              <div
                className={cn(
                  "aspect-square grid place-items-center text-5xl bg-gradient-to-br",
                  m.bg,
                )}
              >
                {m.emoji}
              </div>
              <div className="p-3">
                <div className="text-sm font-semibold truncate">{m.name}</div>
                <div className="text-xs text-cashier font-bold mt-0.5">
                  {formatVnd(m.price)}
                </div>
              </div>
            </button>
          ))}
        </div>
      </div>

      <aside className="bg-card rounded-xl border p-4 h-fit sticky top-6">
        <h2 className="font-bold">Thông tin order</h2>

        <label className="block text-xs font-medium text-muted-foreground mt-4">
          Số bàn
        </label>
        <select
          value={table}
          onChange={(e) => setTable(e.target.value)}
          className="mt-1 w-full h-10 px-3 rounded-md border bg-background"
        >
          {["Bàn 01", "Bàn 02", "Bàn 03", "Bàn 04", "Bàn 05", "Mang đi"].map(
            (t) => (
              <option key={t}>{t}</option>
            ),
          )}
        </select>

        <div className="mt-4">
          <div className="grid grid-cols-[1fr_40px_70px_24px] text-xs font-medium text-muted-foreground pb-2 border-b">
            <span>Món</span>
            <span className="text-center">SL</span>
            <span className="text-right">Thành tiền</span>
            <span />
          </div>
          {lines.map((l) => (
            <div
              key={l.id}
              className="grid grid-cols-[1fr_40px_70px_24px] items-center py-2 text-sm border-b"
            >
              <span>{l.name}</span>
              <span className="text-center">{l.qty}</span>
              <span className="text-right font-semibold">
                {formatVnd(l.qty * l.price)}
              </span>
              <button
                onClick={() => remove(l.id)}
                className="text-destructive hover:opacity-70"
              >
                <Trash2 className="w-4 h-4" />
              </button>
            </div>
          ))}
        </div>

        <label className="block text-xs font-medium text-muted-foreground mt-4">
          Ghi chú
        </label>
        <textarea
          value={note}
          onChange={(e) => setNote(e.target.value)}
          rows={2}
          className="mt-1 w-full px-3 py-2 rounded-md border bg-background text-sm resize-none"
        />

        <div className="flex justify-between items-center mt-4 pt-3 border-t">
          <span className="text-sm text-muted-foreground">Tạm tính</span>
          <span className="font-bold text-lg">{formatVnd(total)}</span>
        </div>

        <button className="mt-3 w-full h-11 rounded-md bg-cashier text-cashier-foreground font-semibold hover:opacity-90">
          Thanh toán
        </button>
      </aside>
    </div>
  );
}
