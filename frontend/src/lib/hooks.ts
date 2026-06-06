import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import {
  menuApi,
  orderApi,
  tableApi,
  customerApi,
  statsApi,
} from "./api";
import type { OrderStatus, OrderLine, Table } from "./pos-data";

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

export function useOrders(status?: OrderStatus) {
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
      lines: Omit<OrderLine, "name" | "price">[];
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
    mutationFn: ({ id, status }: { id: string; status: OrderStatus }) =>
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
