import { createFileRoute, Outlet, redirect } from "@tanstack/react-router";
import {
  ClipboardList,
  List,
  UtensilsCrossed,
  CreditCard,
  History,
  Users,
} from "lucide-react";
import { PosShell } from "@/components/pos/PosShell";

export const Route = createFileRoute("/cashier")({
  component: CashierLayout,
  beforeLoad: ({ location }) => {
    if (location.pathname === "/cashier")
      throw redirect({ to: "/cashier/new" });
  },
});

function CashierLayout() {
  const nav = [
    {
      to: "/cashier/new",
      label: "Tạo order",
      icon: <ClipboardList className="w-4 h-4" />,
    },
    {
      to: "/cashier/orders",
      label: "Danh sách order",
      icon: <List className="w-4 h-4" />,
    },
    {
      to: "/cashier/menu",
      label: "Menu",
      icon: <UtensilsCrossed className="w-4 h-4" />,
    },
    {
      to: "/cashier/payment",
      label: "Thanh toán",
      icon: <CreditCard className="w-4 h-4" />,
    },
    {
      to: "/cashier/history",
      label: "Lịch sử",
      icon: <History className="w-4 h-4" />,
    },
    {
      to: "/cashier/customers",
      label: "Khách hàng",
      icon: <Users className="w-4 h-4" />,
    },
  ];
  return (
    <PosShell role="cashier" nav={nav}>
      <Outlet />
    </PosShell>
  );
}
