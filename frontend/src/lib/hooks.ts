import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import {
  menuApi,
  orderApi,
  tableApi,
  customerApi,
  statsApi,
  inventoryApi,
} from "./api";

export function useMenu() {
  return useQuery({
    queryKey: ["menu"],
    queryFn: async () => {
      const res = await menuApi.getAll();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useOrders(status?: string) {
  return useQuery({
    queryKey: ["orders", status],
    queryFn: async () => {
      const res = status ? await orderApi.getByStatus(status) : await orderApi.getAll();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useOrder(id: string) {
  return useQuery({
    queryKey: ["order", id],
    queryFn: async () => {
      const res = await orderApi.getById(id);
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
    enabled: !!id,
  });
}

export function useCreateOrder() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: (data: {
      table: string;
      lines: Omit<{ itemId: string; name: string; qty: number; price: number }, "name" | "price">[];
      note?: string;
      priority?: "priority" | "normal";
    }) => orderApi.create(data),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["orders"] });
      queryClient.invalidateQueries({ queryKey: ["stats"] });
    },
  });
}

export function useUpdateOrderStatus() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({ id, status }: { id: string; status: string }) =>
      orderApi.updateStatus(id, status),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["orders"] });
      queryClient.invalidateQueries({ queryKey: ["stats"] });
    },
  });
}

export function usePayOrder() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({ id, method }: { id: string; method: "cash" | "card" | "ewallet" }) =>
      orderApi.pay(id, method),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["orders"] });
      queryClient.invalidateQueries({ queryKey: ["stats"] });
    },
  });
}

export function useTables() {
  return useQuery({
    queryKey: ["tables"],
    queryFn: async () => {
      const res = await tableApi.getAll();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useCustomers() {
  return useQuery({
    queryKey: ["customers"],
    queryFn: async () => {
      const res = await customerApi.getAll();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useStats() {
  return useQuery({
    queryKey: ["stats"],
    queryFn: async () => {
      const res = await statsApi.getAll();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useRevenueStats(period: "day" | "week" | "month") {
  return useQuery({
    queryKey: ["stats", "revenue", period],
    queryFn: async () => {
      const res = await statsApi.getRevenue(period);
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useTopItems() {
  return useQuery({
    queryKey: ["stats", "topItems"],
    queryFn: async () => {
      const res = await statsApi.getTopItems();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useOrderStats() {
  return useQuery({
    queryKey: ["stats", "orders"],
    queryFn: async () => {
      const res = await statsApi.getOrderStats();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useRevenueByHour() {
  return useQuery({
    queryKey: ["stats", "revenueByHour"],
    queryFn: async () => {
      const res = await statsApi.getRevenueByHour();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useIngredients() {
  return useQuery({
    queryKey: ["ingredients"],
    queryFn: async () => {
      const res = await inventoryApi.getAll();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useIngredientsByCategory(category: string) {
  return useQuery({
    queryKey: ["ingredients", category],
    queryFn: async () => {
      const res = await inventoryApi.getByCategory(category);
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useLowStockIngredients() {
  return useQuery({
    queryKey: ["ingredients", "lowStock"],
    queryFn: async () => {
      const res = await inventoryApi.getLowStock();
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
  });
}

export function useInventoryTransactions(ingredientId: number) {
  return useQuery({
    queryKey: ["inventoryTransactions", ingredientId],
    queryFn: async () => {
      const res = await inventoryApi.getTransactions(ingredientId);
      if (!res.success) throw new Error(res.error);
      return res.data!;
    },
    enabled: !!ingredientId,
  });
}

export function useCreateIngredient() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: (data: {
      name: string;
      unit: string;
      quantity: number;
      minStock: number;
      costPerUnit: number;
      category: string;
    }) => inventoryApi.create(data),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["ingredients"] });
    },
  });
}

export function useUpdateIngredient() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({ id, data }: { id: number; data: Record<string, unknown> }) =>
      inventoryApi.update(id, data),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["ingredients"] });
    },
  });
}

export function useDeleteIngredient() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: (id: number) => inventoryApi.delete(id),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["ingredients"] });
    },
  });
}

export function useImportStock() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({
      id,
      quantity,
      unitPrice,
      note,
    }: {
      id: number;
      quantity: number;
      unitPrice: number;
      note?: string;
    }) => inventoryApi.import(id, quantity, unitPrice, note),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["ingredients"] });
    },
  });
}

export function useExportStock() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({
      id,
      quantity,
      note,
    }: {
      id: number;
      quantity: number;
      note?: string;
    }) => inventoryApi.export(id, quantity, note),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["ingredients"] });
    },
  });
}
