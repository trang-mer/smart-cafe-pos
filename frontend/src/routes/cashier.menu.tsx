import { createFileRoute } from "@tanstack/react-router";
import { categories, formatVnd, menu } from "@/lib/pos-data";
import { cn } from "@/lib/utils";

export const Route = createFileRoute("/cashier/menu")({ component: MenuPage });

function MenuPage() {
  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Menu</h1>
      {categories
        .filter((c) => c.id !== "all")
        .map((c) => {
          const items = menu.filter((m) => m.category === c.id);
          if (!items.length) return null;
          return (
            <div key={c.id} className="mb-6">
              <h2 className="font-semibold mb-2">{c.label}</h2>
              <div className="grid grid-cols-4 gap-3">
                {items.map((m) => (
                  <div
                    key={m.id}
                    className="bg-card rounded-xl border overflow-hidden"
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
                      <div className="text-sm font-semibold">{m.name}</div>
                      <div className="text-xs text-cashier font-bold">
                        {formatVnd(m.price)}
                      </div>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          );
        })}
    </div>
  );
}
