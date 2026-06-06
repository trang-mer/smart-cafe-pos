import type {
  MenuItem,
  Order,
  Customer,
  Table,
  OrderStatus,
  ApiResponse,
  OrderLine,
} from "./pos-data";

const API_BASE = "http://localhost:8080/api";

async function request<T>(
  endpoint: string,
  options: RequestInit = {}
): Promise<ApiResponse<T>> {
  try {
    const response = await fetch(`${API_BASE}${endpoint}`, {
      headers: {
        "Content-Type": "application/json",
        ...options.headers,
      },
      ...options,
    });

    const data = await response.json();

    if (!response.ok) {
      return { success: false, error: data.message || "Request failed" };
    }

    return { success: true, data };
  } catch (error) {
    return {
      success: false,
      error: error instanceof Error ? error.message : "Network error",
    };
  }
}

export const menuApi = {
  getAll: () => request<MenuItem[]>("/menu"),

  getByCategory: (category: string) =>
    request<MenuItem[]>(`/menu?category=${category}`),
};

export const orderApi = {
  getAll: () => request<Order[]>("/orders"),

  getById: (id: string) => request<Order>(`/orders/${id}`),

  getByStatus: (status: OrderStatus) =>
    request<Order[]>(`/orders?status=${status}`),

  create: (data: {
    table: string;
    lines: Omit<OrderLine, "name" | "price">[];
    note?: string;
    priority?: "priority" | "normal";
  }) =>
    request<Order>("/orders", {
      method: "POST",
      body: JSON.stringify(data),
    }),

  updateStatus: (id: string, status: OrderStatus) =>
    request<Order>(`/orders/${id}/status`, {
      method: "PATCH",
      body: JSON.stringify({ status }),
    }),

  pay: (id: string, method: "cash" | "card" | "ewallet") =>
    request<Order>(`/orders/${id}/pay`, {
      method: "POST",
      body: JSON.stringify({ method }),
    }),

  cancel: (id: string) =>
    request<void>(`/orders/${id}`, { method: "DELETE" }),
};

export const tableApi = {
  getAll: () => request<Table[]>("/tables"),

  updateStatus: (id: string, status: Table["status"]) =>
    request<Table>(`/tables/${id}`, {
      method: "PATCH",
      body: JSON.stringify({ status }),
    }),
};

export const customerApi = {
  getAll: () => request<Customer[]>("/customers"),

  getById: (id: string) => request<Customer>(`/customers/${id}`),

  search: (query: string) =>
    request<Customer[]>(`/customers/search?q=${encodeURIComponent(query)}`),
};

export const statsApi = {
  getRevenue: (period: "day" | "week" | "month") =>
    request<{ date: string; value: number }[]>(`/stats/revenue?period=${period}`),

  getTopItems: (limit = 5) =>
    request<{ name: string; count: number }[]>(`/stats/top-items?limit=${limit}`),

  getOrderStats: () =>
    request<{
      total: number;
      new: number;
      cooking: number;
      done: number;
      cancelled: number;
    }>("/stats/orders"),

  getRevenueByHour: () =>
    request<{ hour: string; value: number }[]>("/stats/revenue-by-hour"),
};
