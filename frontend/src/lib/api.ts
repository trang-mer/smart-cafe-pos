const API_BASE = "http://localhost:8080/api";

async function request<T>(
  endpoint: string,
  options: RequestInit = {}
): Promise<{ success: boolean; data?: T; error?: string }> {
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
      return { success: false, error: data.error || "Request failed" };
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
  getAll: () => request<MenuItem[]>("menu"),
  getByCategory: (category: string) =>
    request<MenuItem[]>(`menu?category=${category}`),
};

export const orderApi = {
  getAll: () => request<Order[]>("orders"),
  getById: (id: string) => request<Order>(`orders/${id}`),
  getByStatus: (status: string) =>
    request<Order[]>(`orders?status=${status}`),
  create: (data: {
    table: string;
    lines: Omit<OrderLine, "name" | "price">[];
    note?: string;
    priority?: "priority" | "normal";
  }) =>
    request<Order>("orders", {
      method: "POST",
      body: JSON.stringify(data),
    }),
  updateStatus: (id: string, status: string) =>
    request<Order>(`orders/${id}`, {
      method: "PATCH",
      body: JSON.stringify({ status }),
    }),
  pay: (id: string, method: "cash" | "card" | "ewallet") =>
    request<Order>(`orders/${id}`, {
      method: "PATCH",
      body: JSON.stringify({ status: "done", method }),
    }),
  cancel: (id: string) =>
    request<void>(`orders/${id}`, { method: "DELETE" }),
};

export const tableApi = {
  getAll: () => request<Table[]>("tables"),
  updateStatus: (id: string, status: Table["status"]) =>
    request<Table>(`tables`, {
      method: "PATCH",
      body: JSON.stringify({ id, status }),
    }),
};

export const customerApi = {
  getAll: () => request<Customer[]>("customers"),
  getById: (id: string) => request<Customer>(`customers/${id}`),
  search: (query: string) =>
    request<Customer[]>(`customers?q=${encodeURIComponent(query)}`),
};

export const statsApi = {
  getRevenue: (period: "day" | "week" | "month") =>
    request<{ date: string; value: number }[]>(`stats?type=revenue&period=${period}`),
  getTopItems: (limit = 5) =>
    request<{ name: string; count: number }[]>(`stats?type=top-items&limit=${limit}`),
  getOrderStats: () =>
    request<{
      total: number;
      new: number;
      cooking: number;
      done: number;
      cancelled: number;
    }>(`stats?type=orders`),
  getRevenueByHour: () =>
    request<{ h: string; v: number }[]>(`stats?type=revenue-by-hour`),
  getAll: () =>
    request<{
      revenue: { date: string; value: number }[];
      topItems: { name: string; count: number }[];
      orderStats: {
        total: number;
        new: number;
        cooking: number;
        done: number;
        cancelled: number;
      };
      revenueByHour: { h: string; v: number }[];
      todayRevenue: number;
      todayOrders: number;
    }>(`stats`),
};

export const healthApi = {
  check: () => request<{ status: string }>("health"),
};

export interface Ingredient {
  id: number;
  name: string;
  unit: string;
  quantity: number;
  minStock: number;
  costPerUnit: number;
  category: string;
  lowStock: boolean;
}

export interface InventoryTransaction {
  id: number;
  ingredientId: number;
  type: "import" | "export";
  quantity: number;
  unitPrice: number;
  note: string;
  createdAt: string;
}

export const inventoryApi = {
  getAll: () => request<Ingredient[]>("inventory"),
  getById: (id: number) => request<Ingredient>(`inventory/${id}`),
  getByCategory: (category: string) =>
    request<Ingredient[]>(`inventory?category=${category}`),
  getLowStock: () => request<Ingredient[]>("inventory/low-stock"),
  getTransactions: (ingredientId: number) =>
    request<InventoryTransaction[]>(`inventory/${ingredientId}-transactions`),
  create: (data: Omit<Ingredient, "id" | "lowStock">) =>
    request<Ingredient>("inventory", {
      method: "POST",
      body: JSON.stringify(data),
    }),
  update: (id: number, data: Partial<Ingredient>) =>
    request<Ingredient>(`inventory/${id}`, {
      method: "PATCH",
      body: JSON.stringify(data),
    }),
  delete: (id: number) =>
    request<void>(`inventory/${id}`, { method: "DELETE" }),
  import: (id: number, quantity: number, unitPrice: number, note?: string) =>
    request<{ success: boolean }>("inventory", {
      method: "POST",
      body: JSON.stringify({ action: "import", id, quantity, unitPrice, note }),
    }),
  export: (id: number, quantity: number, note?: string) =>
    request<{ success: boolean }>("inventory", {
      method: "POST",
      body: JSON.stringify({ action: "export", id, quantity, note }),
    }),
};
