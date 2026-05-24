export type Category = "all" | "coffee" | "tea" | "blended" | "drinks" | "cake";

export interface MenuItem {
  id: string;
  name: string;
  price: number;
  category: Exclude<Category, "all">;
  emoji: string;
  bg: string;
}

export const menu: MenuItem[] = [
  {
    id: "espresso",
    name: "Espresso",
    price: 25000,
    category: "coffee",
    emoji: "☕",
    bg: "from-amber-900 to-amber-700",
  },
  {
    id: "americano",
    name: "Americano",
    price: 30000,
    category: "coffee",
    emoji: "☕",
    bg: "from-stone-800 to-stone-600",
  },
  {
    id: "latte",
    name: "Latte",
    price: 40000,
    category: "coffee",
    emoji: "🥛",
    bg: "from-amber-200 to-amber-400",
  },
  {
    id: "cappuccino",
    name: "Cappuccino",
    price: 40000,
    category: "coffee",
    emoji: "☕",
    bg: "from-amber-300 to-amber-500",
  },
  {
    id: "mocha",
    name: "Mocha",
    price: 45000,
    category: "coffee",
    emoji: "🍫",
    bg: "from-amber-800 to-amber-600",
  },
  {
    id: "caramel",
    name: "Caramel Macchiato",
    price: 45000,
    category: "coffee",
    emoji: "🍮",
    bg: "from-orange-300 to-amber-500",
  },
  {
    id: "matcha",
    name: "Matcha Latte",
    price: 40000,
    category: "tea",
    emoji: "🍵",
    bg: "from-green-300 to-green-500",
  },
  {
    id: "peach",
    name: "Trà đào",
    price: 35000,
    category: "tea",
    emoji: "🍑",
    bg: "from-orange-200 to-pink-300",
  },
  {
    id: "lychee",
    name: "Trà vải",
    price: 35000,
    category: "tea",
    emoji: "🍒",
    bg: "from-pink-200 to-rose-300",
  },
  {
    id: "chocoblend",
    name: "Chocolate đá xay",
    price: 50000,
    category: "blended",
    emoji: "🍫",
    bg: "from-amber-900 to-amber-700",
  },
  {
    id: "cookies",
    name: "Cookies",
    price: 20000,
    category: "cake",
    emoji: "🍪",
    bg: "from-amber-300 to-amber-500",
  },
  {
    id: "tiramisu",
    name: "Tiramisu",
    price: 45000,
    category: "cake",
    emoji: "🍰",
    bg: "from-amber-200 to-amber-400",
  },
];

export const categories: { id: Category; label: string }[] = [
  { id: "all", label: "Tất cả" },
  { id: "coffee", label: "Cà phê" },
  { id: "tea", label: "Trà" },
  { id: "blended", label: "Đá xay" },
  { id: "drinks", label: "Nước ép" },
  { id: "cake", label: "Bánh" },
];

export type OrderStatus = "new" | "cooking" | "done" | "paid";
export type OrderPriority = "priority" | "normal";

export interface OrderLine {
  itemId: string;
  name: string;
  qty: number;
  price: number;
}
export interface Order {
  id: string;
  table: string;
  time: string;
  priority: OrderPriority;
  lines: OrderLine[];
  note?: string;
  status: OrderStatus;
}

export const orders: Order[] = [
  {
    id: "#23",
    table: "Bàn 03",
    time: "14:32",
    priority: "priority",
    status: "cooking",
    lines: [
      { itemId: "latte", name: "Latte", qty: 2, price: 40000 },
      { itemId: "matcha", name: "Matcha Latte", qty: 1, price: 40000 },
      { itemId: "cookies", name: "Cookies", qty: 1, price: 20000 },
    ],
    note: "Ít đá, thêm đường",
  },
  {
    id: "#22",
    table: "Bàn 05",
    time: "14:30",
    priority: "normal",
    status: "new",
    lines: [
      { itemId: "americano", name: "Americano", qty: 1, price: 30000 },
      { itemId: "cookies", name: "Cookies", qty: 2, price: 20000 },
    ],
  },
  {
    id: "#21",
    table: "Bàn 02",
    time: "14:28",
    priority: "normal",
    status: "new",
    lines: [
      { itemId: "caramel", name: "Caramel Macchiato", qty: 1, price: 45000 },
      { itemId: "tiramisu", name: "Tiramisu", qty: 1, price: 45000 },
    ],
  },
];

export const formatVnd = (n: number) => n.toLocaleString("vi-VN") + "đ";

export const revenue7d = [
  { d: "18/05", v: 6200000 },
  { d: "19/05", v: 7400000 },
  { d: "20/05", v: 5900000 },
  { d: "21/05", v: 8100000 },
  { d: "22/05", v: 9800000 },
  { d: "23/05", v: 11200000 },
  { d: "24/05", v: 12560000 },
];

export const topItems = [
  { name: "Latte", count: 56 },
  { name: "Americano", count: 42 },
  { name: "Matcha Latte", count: 35 },
  { name: "Caramel Macchiato", count: 28 },
  { name: "Mocha", count: 21 },
];

export const orderStatusBreakdown = [
  { name: "Mới", value: 25, color: "var(--color-cashier)" },
  { name: "Đang làm", value: 60, color: "var(--color-warning)" },
  { name: "Hoàn thành", value: 40, color: "var(--color-kitchen)" },
  { name: "Đã hủy", value: 3, color: "var(--color-destructive)" },
];

export const revenueByHour = Array.from({ length: 17 }, (_, i) => {
  const hour = 6 + i;
  const base = Math.sin((i - 4) / 3) * 1500 + 2500;
  return {
    h: `${hour.toString().padStart(2, "0")}h`,
    v: Math.max(300, Math.round(base + Math.random() * 800)) * 1000,
  };
});
