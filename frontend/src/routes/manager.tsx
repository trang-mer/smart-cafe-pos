import { createFileRoute } from "@tanstack/react-router";
import {
  LayoutDashboard,
  DollarSign,
  ClipboardList,
  UtensilsCrossed,
  Users,
  UsersRound,
  FileText,
  Settings,
  TrendingUp,
  Loader2,
  Package,
  AlertTriangle,
  Plus,
  Minus,
  Search,
} from "lucide-react";
import {
  LineChart,
  Line,
  BarChart,
  Bar,
  PieChart,
  Pie,
  Cell,
  XAxis,
  YAxis,
  CartesianGrid,
  ResponsiveContainer,
  Tooltip,
} from "recharts";
import { PosShell } from "@/components/pos/PosShell";
import { formatVnd } from "@/lib/pos-data";
import {
  useRevenueStats,
  useTopItems,
  useOrderStats,
  useRevenueByHour,
  useIngredients,
  useLowStockIngredients,
  useInventoryTransactions,
  useImportStock,
  useExportStock,
  useCreateIngredient,
  useUpdateIngredient,
} from "@/lib/hooks";
import { useState } from "react";
import { Button } from "@/components/ui/button";
import {
  Dialog,
  DialogContent,
  DialogHeader,
  DialogTitle,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Textarea } from "@/components/ui/textarea";
import { ScrollArea } from "@/components/ui/scroll-area";

export const Route = createFileRoute("/manager")({ component: ManagerPage });

function ManagerPage() {
  const [activeTab, setActiveTab] = useState("overview");

  const nav = [
    {
      to: "/manager",
      label: "Tổng quan",
      icon: <LayoutDashboard className="w-4 h-4" />,
      tab: "overview",
    },
    {
      to: "/manager",
      label: "Doanh thu",
      icon: <DollarSign className="w-4 h-4" />,
      tab: "revenue",
    },
    {
      to: "/manager",
      label: "Order",
      icon: <ClipboardList className="w-4 h-4" />,
      tab: "orders",
    },
    {
      to: "/manager",
      label: "Menu",
      icon: <UtensilsCrossed className="w-4 h-4" />,
      tab: "menu",
    },
    { to: "/manager", label: "Nhân viên", icon: <Users className="w-4 h-4" />, tab: "staff" },
    {
      to: "/manager",
      label: "Khách hàng",
      icon: <UsersRound className="w-4 h-4" />,
      tab: "customers",
    },
    {
      to: "/manager",
      label: "Kho",
      icon: <Package className="w-4 h-4" />,
      tab: "inventory",
    },
    {
      to: "/manager",
      label: "Báo cáo",
      icon: <FileText className="w-4 h-4" />,
      tab: "reports",
    },
    {
      to: "/manager",
      label: "Cài đặt",
      icon: <Settings className="w-4 h-4" />,
      tab: "settings",
    },
  ];

  return (
    <PosShell role="manager" nav={nav}>
      <Tabs value={activeTab} onValueChange={setActiveTab} className="flex-1">
        <TabsList className="hidden">
          <TabsTrigger value="overview">Tổng quan</TabsTrigger>
          <TabsTrigger value="inventory">Kho</TabsTrigger>
        </TabsList>
        <TabsContent value="overview">
          <Dashboard />
        </TabsContent>
        <TabsContent value="inventory">
          <InventoryPage />
        </TabsContent>
      </Tabs>
    </PosShell>
  );
}

function Dashboard() {
  const { data: revenue = [], isLoading: revenueLoading } = useRevenueStats("week");
  const { data: topItems = [], isLoading: topItemsLoading } = useTopItems();
  const { data: orderStats, isLoading: orderStatsLoading } = useOrderStats();
  const { data: revenueByHour = [], isLoading: revenueByHourLoading } = useRevenueByHour();

  const todayRevenue = revenue.length > 0 ? revenue[revenue.length - 1].value : 0;
  const totalOrders = orderStats?.total || 0;

  const kpis = [
    {
      label: "Doanh thu hôm nay",
      value: formatVnd(todayRevenue),
      delta: "+12.5%",
    },
    { label: "Order hôm nay", value: totalOrders.toString(), delta: "+8.3%" },
    { label: "Khách hàng", value: "98", delta: "+5.2%" },
    { label: "Đơn trung bình", value: "98.125đ", delta: "+3.1%" },
  ];

  const orderStatusBreakdown = orderStats
    ? [
        { name: "Mới", value: orderStats.new, color: "var(--color-cashier)" },
        {
          name: "Đang làm",
          value: orderStats.cooking,
          color: "var(--color-warning)",
        },
        {
          name: "Hoàn thành",
          value: orderStats.done,
          color: "var(--color-kitchen)",
        },
        {
          name: "Đã hủy",
          value: orderStats.cancelled,
          color: "var(--color-destructive)",
        },
      ]
    : [];

  const chartData = revenue.map((r) => ({ d: r.date, v: r.value }));
  const hourData = revenueByHour.map((r) => ({
    h: r.hour,
    v: r.value,
  }));

  return (
    <div>
      <h1 className="text-xl font-bold mb-4">Tổng quan</h1>

      <div className="grid grid-cols-4 gap-4">
        {kpis.map((k) => (
          <div key={k.label} className="bg-card border rounded-xl p-4">
            <div className="text-xs text-muted-foreground">{k.label}</div>
            <div className="text-2xl font-bold mt-1 text-manager">
              {k.value}
            </div>
            <div className="text-xs text-success mt-1 flex items-center gap-1">
              <TrendingUp className="w-3 h-3" /> {k.delta}{" "}
              <span className="text-muted-foreground">so với hôm qua</span>
            </div>
          </div>
        ))}
      </div>

      <div className="grid grid-cols-[1fr_360px] gap-4 mt-4">
        <div className="bg-card border rounded-xl p-4">
          <div className="flex items-center mb-3">
            <h2 className="font-semibold">Doanh thu 7 ngày qua</h2>
            <select className="ml-auto text-xs h-8 px-2 rounded border bg-background">
              <option>7 ngày</option>
              <option>30 ngày</option>
            </select>
          </div>
          {revenueLoading ? (
            <div className="flex items-center justify-center h-64">
              <Loader2 className="w-8 h-8 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="h-64">
              <ResponsiveContainer width="100%" height="100%">
                <LineChart data={chartData}>
                  <CartesianGrid
                    strokeDasharray="3 3"
                    stroke="var(--color-border)"
                  />
                  <XAxis
                    dataKey="d"
                    stroke="var(--color-muted-foreground)"
                    fontSize={11}
                  />
                  <YAxis
                    stroke="var(--color-muted-foreground)"
                    fontSize={11}
                    tickFormatter={(v) => `${v / 1000000}tr`}
                  />
                  <Tooltip
                    formatter={(v: number) => formatVnd(v)}
                    contentStyle={{
                      background: "var(--color-card)",
                      border: "1px solid var(--color-border)",
                      borderRadius: 8,
                    }}
                  />
                  <Line
                    type="monotone"
                    dataKey="v"
                    stroke="var(--color-manager)"
                    strokeWidth={2.5}
                    dot={{ r: 4 }}
                  />
                </LineChart>
              </ResponsiveContainer>
            </div>
          )}
        </div>

        <div className="bg-card border rounded-xl p-4">
          <h2 className="font-semibold mb-3">Top món bán chạy</h2>
          {topItemsLoading ? (
            <div className="flex items-center justify-center h-48">
              <Loader2 className="w-6 h-6 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="space-y-2">
              {topItems.map((it, i) => (
                <div key={it.name} className="flex items-center gap-3 text-sm">
                  <span className="w-5 text-muted-foreground">{i + 1}</span>
                  <span className="flex-1">{it.name}</span>
                  <span className="font-semibold">{it.count}</span>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

      <div className="grid grid-cols-2 gap-4 mt-4">
        <div className="bg-card border rounded-xl p-4">
          <h2 className="font-semibold mb-3">Order theo trạng thái</h2>
          {orderStatsLoading ? (
            <div className="flex items-center justify-center h-52">
              <Loader2 className="w-6 h-6 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="flex items-center gap-6">
              <div className="relative w-44 h-44">
                <ResponsiveContainer width="100%" height="100%">
                  <PieChart>
                    <Pie
                      data={orderStatusBreakdown}
                      dataKey="value"
                      innerRadius={55}
                      outerRadius={80}
                      paddingAngle={2}
                    >
                      {orderStatusBreakdown.map((e, i) => (
                        <Cell key={i} fill={e.color} />
                      ))}
                    </Pie>
                  </PieChart>
                </ResponsiveContainer>
                <div className="absolute inset-0 grid place-items-center pointer-events-none">
                  <div className="text-center">
                    <div className="text-2xl font-bold">{totalOrders}</div>
                    <div className="text-xs text-muted-foreground">
                      Tổng order
                    </div>
                  </div>
                </div>
              </div>
              <div className="space-y-2 text-sm">
                {orderStatusBreakdown.map((s) => (
                  <div key={s.name} className="flex items-center gap-2">
                    <span
                      className="w-3 h-3 rounded-sm"
                      style={{ background: s.color }}
                    />
                    <span>{s.name}</span>
                    <span className="text-muted-foreground ml-2">
                      {s.value} ({Math.round((s.value / totalOrders) * 100)}%)
                    </span>
                  </div>
                ))}
              </div>
            </div>
          )}
        </div>

        <div className="bg-card border rounded-xl p-4">
          <h2 className="font-semibold mb-3">Doanh thu theo giờ</h2>
          {revenueByHourLoading ? (
            <div className="flex items-center justify-center h-52">
              <Loader2 className="w-6 h-6 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <div className="h-52">
              <ResponsiveContainer width="100%" height="100%">
                <BarChart data={hourData}>
                  <CartesianGrid
                    strokeDasharray="3 3"
                    stroke="var(--color-border)"
                  />
                  <XAxis
                    dataKey="h"
                    stroke="var(--color-muted-foreground)"
                    fontSize={10}
                  />
                  <YAxis
                    stroke="var(--color-muted-foreground)"
                    fontSize={10}
                    tickFormatter={(v) => `${v / 1000000}tr`}
                  />
                  <Tooltip
                    formatter={(v: number) => formatVnd(v)}
                    contentStyle={{
                      background: "var(--color-card)",
                      border: "1px solid var(--color-border)",
                      borderRadius: 8,
                    }}
                  />
                  <Bar
                    dataKey="v"
                    fill="var(--color-manager)"
                    radius={[4, 4, 0, 0]}
                  />
                </BarChart>
              </ResponsiveContainer>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

function InventoryPage() {
  const { data: ingredients = [], isLoading } = useIngredients();
  const { data: lowStockItems = [] } = useLowStockIngredients();

  const [searchTerm, setSearchTerm] = useState("");
  const [selectedCategory, setSelectedCategory] = useState<string>("all");
  const [showAddDialog, setShowAddDialog] = useState(false);
  const [showImportDialog, setShowImportDialog] = useState(false);
  const [showExportDialog, setShowExportDialog] = useState(false);
  const [selectedIngredient, setSelectedIngredient] = useState<number | null>(null);
  const [showHistoryDialog, setShowHistoryDialog] = useState(false);

  const [newIngredient, setNewIngredient] = useState({
    name: "",
    unit: "pcs",
    quantity: 0,
    minStock: 0,
    costPerUnit: 0,
    category: "general",
  });

  const [importData, setImportData] = useState({
    quantity: 0,
    unitPrice: 0,
    note: "",
  });

  const [exportData, setExportData] = useState({
    quantity: 0,
    note: "",
  });

  const categories = [
    "all",
    "coffee",
    "dairy",
    "sweetener",
    "tea",
    "syrup",
    "additive",
    "packaging",
    "general",
  ];

  const categoryLabels: Record<string, string> = {
    all: "Tất cả",
    coffee: "Cà phê",
    dairy: "Sữa",
    sweetener: "Đường",
    tea: "Trà",
    syrup: "Siro",
    additive: "Phụ gia",
    packaging: "Bao bì",
    general: "Chung",
  };

  const filteredIngredients = ingredients.filter((ing) => {
    const matchesSearch = ing.name.toLowerCase().includes(searchTerm.toLowerCase());
    const matchesCategory = selectedCategory === "all" || ing.category === selectedCategory;
    return matchesSearch && matchesCategory;
  });

  const createIngredient = useCreateIngredient();
  const updateIngredient = useUpdateIngredient();
  const importStock = useImportStock();
  const exportStock = useExportStock();

  const handleAddIngredient = () => {
    createIngredient.mutate(newIngredient, {
      onSuccess: () => {
        setShowAddDialog(false);
        setNewIngredient({
          name: "",
          unit: "pcs",
          quantity: 0,
          minStock: 0,
          costPerUnit: 0,
          category: "general",
        });
      },
    });
  };

  const handleImport = () => {
    if (!selectedIngredient) return;
    importStock.mutate(
      {
        id: selectedIngredient,
        ...importData,
      },
      {
        onSuccess: () => {
          setShowImportDialog(false);
          setSelectedIngredient(null);
          setImportData({ quantity: 0, unitPrice: 0, note: "" });
        },
      }
    );
  };

  const handleExport = () => {
    if (!selectedIngredient) return;
    exportStock.mutate(
      {
        id: selectedIngredient,
        ...exportData,
      },
      {
        onSuccess: () => {
          setShowExportDialog(false);
          setSelectedIngredient(null);
          setExportData({ quantity: 0, note: "" });
        },
      }
    );
  };

  return (
    <div>
      <div className="flex items-center justify-between mb-4">
        <h1 className="text-xl font-bold">Quản lý kho</h1>
        <div className="flex items-center gap-2">
          <Button
            variant="outline"
            onClick={() => setShowHistoryDialog(true)}
            className="gap-2"
          >
            <FileText className="w-4 h-4" />
            Lịch sử
          </Button>
          <Button onClick={() => setShowAddDialog(true)} className="gap-2">
            <Plus className="w-4 h-4" />
            Thêm nguyên liệu
          </Button>
        </div>
      </div>

      {lowStockItems.length > 0 && (
        <Card className="mb-4 border-amber-200 bg-amber-50">
          <CardHeader className="pb-2">
            <CardTitle className="text-amber-800 flex items-center gap-2 text-base">
              <AlertTriangle className="w-5 h-5" />
              Cảnh báo tồn kho thấp
            </CardTitle>
          </CardHeader>
          <CardContent>
            <div className="flex flex-wrap gap-2">
              {lowStockItems.map((item) => (
                <Badge key={item.id} variant="outline" className="bg-amber-100">
                  {item.name}: {item.quantity} / {item.minStock} {item.unit}
                </Badge>
              ))}
            </div>
          </CardContent>
        </Card>
      )}

      <Card>
        <CardHeader>
          <div className="flex items-center gap-4">
            <div className="relative flex-1 max-w-sm">
              <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-4 h-4 text-muted-foreground" />
              <Input
                placeholder="Tìm kiếm nguyên liệu..."
                value={searchTerm}
                onChange={(e) => setSearchTerm(e.target.value)}
                className="pl-9"
              />
            </div>
            <Select value={selectedCategory} onValueChange={setSelectedCategory}>
              <SelectTrigger className="w-40">
                <SelectValue />
              </SelectTrigger>
              <SelectContent>
                {categories.map((cat) => (
                  <SelectItem key={cat} value={cat}>
                    {categoryLabels[cat]}
                  </SelectItem>
                ))}
              </SelectContent>
            </Select>
          </div>
        </CardHeader>
        <CardContent>
          {isLoading ? (
            <div className="flex items-center justify-center h-48">
              <Loader2 className="w-8 h-8 animate-spin text-muted-foreground" />
            </div>
          ) : (
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>Tên</TableHead>
                  <TableHead>Danh mục</TableHead>
                  <TableHead className="text-right">Tồn kho</TableHead>
                  <TableHead className="text-right">Tối thiểu</TableHead>
                  <TableHead className="text-right">Giá nhập</TableHead>
                  <TableHead className="text-center">Trạng thái</TableHead>
                  <TableHead className="text-center">Thao tác</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {filteredIngredients.map((ing) => (
                  <TableRow key={ing.id}>
                    <TableCell className="font-medium">{ing.name}</TableCell>
                    <TableCell>
                      <Badge variant="outline">
                        {categoryLabels[ing.category] || ing.category}
                      </Badge>
                    </TableCell>
                    <TableCell className="text-right">
                      {ing.quantity.toLocaleString()} {ing.unit}
                    </TableCell>
                    <TableCell className="text-right">
                      {ing.minStock.toLocaleString()} {ing.unit}
                    </TableCell>
                    <TableCell className="text-right">
                      {formatVnd(ing.costPerUnit)}/{ing.unit}
                    </TableCell>
                    <TableCell className="text-center">
                      {ing.lowStock ? (
                        <Badge variant="destructive">Thấp</Badge>
                      ) : (
                        <Badge variant="secondary">OK</Badge>
                      )}
                    </TableCell>
                    <TableCell className="text-center">
                      <div className="flex items-center justify-center gap-1">
                        <Button
                          variant="ghost"
                          size="sm"
                          onClick={() => {
                            setSelectedIngredient(ing.id);
                            setShowImportDialog(true);
                          }}
                          className="gap-1"
                        >
                          <Plus className="w-3 h-3" />
                          Nhập
                        </Button>
                        <Button
                          variant="ghost"
                          size="sm"
                          onClick={() => {
                            setSelectedIngredient(ing.id);
                            setShowExportDialog(true);
                          }}
                          className="gap-1"
                        >
                          <Minus className="w-3 h-3" />
                          Xuất
                        </Button>
                      </div>
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          )}
        </CardContent>
      </Card>

      <Dialog open={showAddDialog} onOpenChange={setShowAddDialog}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Thêm nguyên liệu mới</DialogTitle>
          </DialogHeader>
          <div className="grid gap-4 py-4">
            <div className="grid gap-2">
              <Label htmlFor="name">Tên nguyên liệu</Label>
              <Input
                id="name"
                value={newIngredient.name}
                onChange={(e) =>
                  setNewIngredient({ ...newIngredient, name: e.target.value })
                }
              />
            </div>
            <div className="grid grid-cols-2 gap-4">
              <div className="grid gap-2">
                <Label htmlFor="unit">Đơn vị</Label>
                <Input
                  id="unit"
                  value={newIngredient.unit}
                  onChange={(e) =>
                    setNewIngredient({ ...newIngredient, unit: e.target.value })
                  }
                  placeholder="kg, lít, cái..."
                />
              </div>
              <div className="grid gap-2">
                <Label htmlFor="category">Danh mục</Label>
                <Select
                  value={newIngredient.category}
                  onValueChange={(v) =>
                    setNewIngredient({ ...newIngredient, category: v })
                  }
                >
                  <SelectTrigger>
                    <SelectValue />
                  </SelectTrigger>
                  <SelectContent>
                    {Object.entries(categoryLabels)
                      .filter(([k]) => k !== "all")
                      .map(([k, label]) => (
                        <SelectItem key={k} value={k}>
                          {label}
                        </SelectItem>
                      ))}
                  </SelectContent>
                </Select>
              </div>
            </div>
            <div className="grid grid-cols-3 gap-4">
              <div className="grid gap-2">
                <Label htmlFor="quantity">Số lượng</Label>
                <Input
                  id="quantity"
                  type="number"
                  value={newIngredient.quantity}
                  onChange={(e) =>
                    setNewIngredient({
                      ...newIngredient,
                      quantity: parseFloat(e.target.value) || 0,
                    })
                  }
                />
              </div>
              <div className="grid gap-2">
                <Label htmlFor="minStock">Tồn tối thiểu</Label>
                <Input
                  id="minStock"
                  type="number"
                  value={newIngredient.minStock}
                  onChange={(e) =>
                    setNewIngredient({
                      ...newIngredient,
                      minStock: parseFloat(e.target.value) || 0,
                    })
                  }
                />
              </div>
              <div className="grid gap-2">
                <Label htmlFor="costPerUnit">Giá nhập</Label>
                <Input
                  id="costPerUnit"
                  type="number"
                  value={newIngredient.costPerUnit}
                  onChange={(e) =>
                    setNewIngredient({
                      ...newIngredient,
                      costPerUnit: parseFloat(e.target.value) || 0,
                    })
                  }
                />
              </div>
            </div>
          </div>
          <div className="flex justify-end gap-2">
            <Button variant="outline" onClick={() => setShowAddDialog(false)}>
              Hủy
            </Button>
            <Button onClick={handleAddIngredient}>Thêm</Button>
          </div>
        </DialogContent>
      </Dialog>

      <Dialog open={showImportDialog} onOpenChange={setShowImportDialog}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Nhập kho</DialogTitle>
          </DialogHeader>
          <div className="grid gap-4 py-4">
            <div className="grid gap-2">
              <Label htmlFor="importQty">Số lượng nhập</Label>
              <Input
                id="importQty"
                type="number"
                value={importData.quantity}
                onChange={(e) =>
                  setImportData({
                    ...importData,
                    quantity: parseFloat(e.target.value) || 0,
                  })
                }
              />
            </div>
            <div className="grid gap-2">
              <Label htmlFor="importPrice">Giá nhập / đơn vị</Label>
              <Input
                id="importPrice"
                type="number"
                value={importData.unitPrice}
                onChange={(e) =>
                  setImportData({
                    ...importData,
                    unitPrice: parseFloat(e.target.value) || 0,
                  })
                }
              />
            </div>
            <div className="grid gap-2">
              <Label htmlFor="importNote">Ghi chú</Label>
              <Textarea
                id="importNote"
                value={importData.note}
                onChange={(e) =>
                  setImportData({ ...importData, note: e.target.value })
                }
              />
            </div>
          </div>
          <div className="flex justify-end gap-2">
            <Button variant="outline" onClick={() => setShowImportDialog(false)}>
              Hủy
            </Button>
            <Button onClick={handleImport}>Nhập kho</Button>
          </div>
        </DialogContent>
      </Dialog>

      <Dialog open={showExportDialog} onOpenChange={setShowExportDialog}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Xuất kho</DialogTitle>
          </DialogHeader>
          <div className="grid gap-4 py-4">
            <div className="grid gap-2">
              <Label htmlFor="exportQty">Số lượng xuất</Label>
              <Input
                id="exportQty"
                type="number"
                value={exportData.quantity}
                onChange={(e) =>
                  setExportData({
                    ...exportData,
                    quantity: parseFloat(e.target.value) || 0,
                  })
                }
              />
            </div>
            <div className="grid gap-2">
              <Label htmlFor="exportNote">Lý do / Ghi chú</Label>
              <Textarea
                id="exportNote"
                value={exportData.note}
                onChange={(e) =>
                  setExportData({ ...exportData, note: e.target.value })
                }
              />
            </div>
          </div>
          <div className="flex justify-end gap-2">
            <Button variant="outline" onClick={() => setShowExportDialog(false)}>
              Hủy
            </Button>
            <Button onClick={handleExport}>Xuất kho</Button>
          </div>
        </DialogContent>
      </Dialog>

      <Dialog open={showHistoryDialog} onOpenChange={setShowHistoryDialog}>
        <DialogContent className="max-w-2xl">
          <DialogHeader>
            <DialogTitle>Lịch sử nhập/xuất kho</DialogTitle>
          </DialogHeader>
          <ScrollArea className="h-96">
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>Nguyên liệu</TableHead>
                  <TableHead>Loại</TableHead>
                  <TableHead className="text-right">Số lượng</TableHead>
                  <TableHead className="text-right">Giá</TableHead>
                  <TableHead>Ghi chú</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {ingredients.slice(0, 10).map((ing) => (
                  <TableRow key={ing.id}>
                    <TableCell className="font-medium">{ing.name}</TableCell>
                    <TableCell>
                      <Badge variant="outline">{ing.category}</Badge>
                    </TableCell>
                    <TableCell className="text-right">
                      {ing.quantity.toLocaleString()} {ing.unit}
                    </TableCell>
                    <TableCell className="text-right">
                      {formatVnd(ing.costPerUnit)}
                    </TableCell>
                    <TableCell>-</TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </ScrollArea>
        </DialogContent>
      </Dialog>
    </div>
  );
}
