# Smart Cafe POS - Hệ thống quản lý quán cafe

## 1. Tổng quan

Dự án là một hệ thống POS (Point of Sale) cho quán cafe với 3 vai trò chính:

- **Cashier (Thu ngân)**: Tạo order, quản lý thanh toán, xem lịch sử
- **Kitchen (Bếp)**: Nhận order, cập nhật trạng thái chế biến
- **Manager (Quản lý)**: Xem báo cáo doanh thu, thống kê

### Công nghệ sử dụng

- **Frontend**: React + TypeScript + TailwindCSS + TanStack Router + TanStack Query
- **Backend**: C++ (TCP Server với multi-threading)
- **Communication**: TCP Socket với JSON messages

---

## 2. Cấu trúc dự án

```
smart-cafe-pos/
├── frontend/                    # React frontend (TanStack Start)
│   ├── src/
│   │   ├── components/
│   │   │   ├── pos/
│   │   │   │   └── PosShell.tsx    # Layout shell cho POS
│   │   │   └── ui/                  # UI components (shadcn/ui)
│   │   ├── routes/
│   │   │   ├── __root.tsx           # Root route
│   │   │   ├── index.tsx            # Login page
│   │   │   ├── cashier.tsx           # Cashier layout
│   │   │   ├── cashier.new.tsx       # Tạo order mới
│   │   │   ├── cashier.orders.tsx    # Danh sách order
│   │   │   ├── cashier.menu.tsx      # Xem menu
│   │   │   ├── cashier.payment.tsx   # Thanh toán
│   │   │   ├── cashier.history.tsx   # Lịch sử order
│   │   │   ├── cashier.customers.tsx # Quản lý khách hàng
│   │   │   ├── kitchen.tsx           # Trang bếp
│   │   │   └── manager.tsx           # Trang quản lý
│   │   ├── lib/
│   │   │   ├── api.ts               # API client functions
│   │   │   ├── hooks.ts             # React Query hooks
│   │   │   ├── pos-data.ts          # Type definitions
│   │   │   ├── utils.ts             # Utility functions
│   │   │   ├── error-page.ts
│   │   │   └── error-capture.ts
│   │   ├── router.tsx               # Router configuration
│   │   ├── styles.css               # Global styles + Tailwind
│   │   ├── app.tsx
│   │   └── main.tsx
│   ├── package.json
│   ├── vite.config.ts
│   └── tsconfig.json
│
├── backend/                     # C++ TCP Server
│   ├── include/
│   │   ├── Server.h             # Server class + broadcastMessage
│   │   ├── ClientHandler.h      # Client handler + processMessage
│   │   ├── Order.h              # Order data structures
│   │   ├── OrderManager.h       # Order CRUD operations
│   │   ├── MenuManager.h        # Menu management
│   │   ├── TableManager.h       # Table/Desk management
│   │   ├── CustomerManager.h   # Customer management
│   │   ├── StatsManager.h      # Statistics & reporting
│   │   └── logger.h             # Logger
│   ├── src/
│   │   ├── main.cpp             # Entry point
│   │   ├── Server.cpp           # Server implementation
│   │   ├── ClientHandler.cpp    # Full business logic
│   │   ├── OrderManager.cpp     # Order CRUD
│   │   ├── MenuManager.cpp      # Menu CRUD
│   │   ├── TableManager.cpp     # Table management
│   │   ├── CustomerManager.cpp # Customer CRUD
│   │   ├── StatsManager.cpp    # Statistics
│   │   └── logger.cpp           # Logger implementation
│   ├── client/
│   │   └── client.cpp           # Test client
│   ├── bin/                     # Compiled binaries
│   └── third_party/
│       └── nlohmann/json.hpp    # JSON library (header-only)
│
├── docs/                       # Documentation
├── .gitignore
├── README.md
└── LICENSE
```

---

## 3. Tính năng đã triển khai

### 3.1 Backend (C++)

- [x] **broadcastMessage()** - Gửi message đến tất cả clients
- [x] **broadcastMessageExcept()** - Gửi message trừ một socket
- [x] **processMessage()** - Xử lý đầy đủ các message types

#### Message Types được hỗ trợ:

| Type | Vai trò | Mô tả |
|------|---------|--------|
| `ROLE` / `LOGIN` | ALL | Đăng nhập với vai trò |
| `ORDER` / `CREATE_ORDER` | CASHIER | Tạo order mới |
| `STATUS` / `UPDATE_STATUS` | KITCHEN | Cập nhật trạng thái order |
| `PAYMENT` / `PAY` | CASHIER | Xử lý thanh toán |
| `CANCEL_ORDER` | CASHIER, MANAGER | Hủy order |
| `GET_ORDERS` | ALL | Lấy danh sách order |
| `GET_ORDER` | ALL | Lấy chi tiết order |
| `GET_MENU` | ALL | Lấy danh sách menu |
| `GET_TABLES` | ALL | Lấy danh sách bàn |
| `UPDATE_TABLE` | CASHIER | Cập nhật trạng thái bàn |
| `GET_CUSTOMERS` | ALL | Lấy danh sách khách hàng |
| `SEARCH_CUSTOMERS` | ALL | Tìm kiếm khách hàng |
| `CREATE_CUSTOMER` | CASHIER | Tạo khách hàng mới |
| `GET_STATS` | ALL | Lấy thống kê |
| `REPORT` | MANAGER | Gửi thông báo đến Kitchen/Cashier |
| `BROADCAST` | ALL | Broadcast message |
| `PING` | ALL | Health check |

#### Managers:

| Manager | Chức năng |
|---------|-----------|
| `OrderManager` | CRUD orders, status management |
| `MenuManager` | Menu items, categories, availability |
| `TableManager` | Tables/desks, occupancy status |
| `CustomerManager` | Customer profiles, visit tracking |
| `StatsManager` | Revenue, order stats, top items |

---

## 4. Cài đặt và chạy

### Frontend

```bash
cd frontend
npm install
npm run dev
```

Frontend chạy tại `http://localhost:5173`

### Backend (C++)

```bash
cd backend

# Build Server (Windows)
g++ -std=c++17 -o bin/server.exe src/main.cpp src/Server.cpp src/ClientHandler.cpp src/OrderManager.cpp src/MenuManager.cpp src/TableManager.cpp src/CustomerManager.cpp src/StatsManager.cpp src/logger.cpp -lws2_32

# Build Server (Linux/Mac)
g++ -std=c++17 -o bin/server src/main.cpp src/Server.cpp src/ClientHandler.cpp src/OrderManager.cpp src/MenuManager.cpp src/TableManager.cpp src/CustomerManager.cpp src/StatsManager.cpp src/logger.cpp -lpthread

# Run
./bin/server.exe    # Windows
./bin/server        # Linux/Mac
```

Backend chạy trên cổng `8080`

---

## 5. Protocol Examples

### Login as CASHIER
```json
{"type": "LOGIN", "role": "CASHIER", "username": "HoangAnh"}
```

### Create Order
```json
{
  "type": "ORDER",
  "tableNumber": 3,
  "items": [
    {"name": "Latte", "qty": 2, "price": 40000},
    {"name": "Matcha", "qty": 1, "price": 40000}
  ],
  "note": "Less sugar"
}
```

### Kitchen Update Status
```json
{"type": "STATUS", "orderId": 1, "status": "cooking"}
```

### Payment
```json
{"type": "PAYMENT", "orderId": 1, "method": "cash", "amount": 120000, "received": 150000}
```

### Get Statistics (Manager)
```json
{"type": "GET_STATS", "period": "week", "limit": 5}
```

### Broadcast (Manager sends to Kitchen/Cashier)
```json
{"type": "REPORT", "message": "Rush hour! Prepare 5 orders ASAP"}
```

---

## 6. Phương án Database

Hiện tại backend sử dụng **in-memory storage**. Để lưu trữ persistent:

| Phương án | Ưu điểm | Phù hợp |
|-----------|----------|---------|
| **SQLite** | Không cần server, file-based | POC, dự án nhỏ |
| **PostgreSQL** | ACID, scalable | Dự án lớn |
| **MongoDB** | Schema linh hoạt | Rapid development |

---

## 7. Các bước tiếp theo

1. [ ] Thêm authentication/authorization
2. [ ] Persistent storage (SQLite/PostgreSQL)
3. [ ] WebSocket cho real-time updates
4. [ ] Deploy frontend + backend

---

## Tác giả

Copyright (c) 2026 Đinh Huyền Trang. MIT License.
