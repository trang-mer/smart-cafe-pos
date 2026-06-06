export type Category = "all" | "coffee" | "tea" | "blended" | "drinks" | "cake";

export interface MenuItem {
  id: number;
  name: string;
  price: number;
  category: Exclude<Category, "all">;
  emoji: string;
  bg: string;
  available?: boolean;
}

export interface MenuCategory {
  id: Category;
  label: string;
}

export const categories: MenuCategory[] = [
  { id: "all", label: "Tất cả" },
  { id: "coffee", label: "Cà phê" },
  { id: "tea", label: "Trà" },
  { id: "blended", label: "Đá xay" },
  { id: "drinks", label: "Nước ép" },
  { id: "cake", label: "Bánh" },
];

export type OrderStatus = "new" | "cooking" | "done" | "paid" | "cancelled";
export type OrderPriority = "priority" | "normal";

export interface OrderLine {
  itemId: string;
  name: string;
  qty: number;
  price: number;
}

export interface Order {
  id: string;
  orderId: number;
  table: string;
  tableNumber: number;
  time: string;
  priority: OrderPriority;
  items: OrderLine[];
  lines: OrderLine[];
  note?: string;
  status: OrderStatus;
  total?: number;
  createdAt?: string;
}

export interface Customer {
  id: string;
  name: string;
  phone: string;
  email?: string;
  visits: number;
  spent: number;
  createdAt?: string;
  lastVisit?: string;
}

export interface Table {
  id: number;
  name: string;
  status: "available" | "occupied" | "reserved";
  currentOrderId?: number;
}

export const formatVnd = (n: number) => n.toLocaleString("vi-VN") + "đ";

export const API_BASE = "http://localhost:8080/api";
