export type Category = "all" | "coffee" | "tea" | "blended" | "drinks" | "cake";

export interface MenuItem {
  id: string;
  name: string;
  price: number;
  category: Exclude<Category, "all">;
  emoji: string;
  bg: string;
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
  total?: number;
}

export interface Customer {
  id: string;
  name: string;
  phone: string;
  visits: number;
  spent: number;
}

export interface Table {
  id: string;
  name: string;
  status: "available" | "occupied" | "reserved";
}

export const formatVnd = (n: number) => n.toLocaleString("vi-VN") + "đ";

export const API_BASE = "http://localhost:8080/api";

export interface ApiResponse<T> {
  success: boolean;
  data?: T;
  error?: string;
}
