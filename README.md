# Smart Cafe POS - Hệ thống quản lý quán cafe

## 1. Tổng quan

Dự án là một hệ thống POS (Point of Sale) cho quán cafe với 3 vai trò chính:

- **Cashier (Thu ngân)**: Tạo order, quản lý thanh toán, xem lịch sử
- **Kitchen (Bếp)**: Nhận order, cập nhật trạng thái chế biến
- **Manager (Quản lý)**: Xem báo cáo doanh thu, thống kê

### Công nghệ sử dụng

- **Frontend**: React + TypeScript + TailwindCSS + TanStack Router + TanStack Query
- **Backend**: C++ (TCP Server + HTTP API Server)
- **Database**: PostgreSQL
- **Communication**: HTTP REST API + TCP Socket

---

## 2. Tính năng chính

### 2.1 Quản lý Order
- Tạo order mới với nhiều món
- Cập nhật trạng thái order (Mới, Đang nấu, Hoàn thành, Đã thanh toán, Đã hủy)
- Thanh toán order (Tiền mặt, Thẻ, Ví điện tử)
- Hủy order
- Xem lịch sử order

### 2.2 Quản lý Menu
- Danh sách món theo danh mục (Cà phê, Trà, Đá xay, Nước ép, Bánh)
- Bật/tắt trạng thái sẵn sàng của món

### 2.3 Quản lý Bàn
- Theo dõi trạng thái bàn (Trống, Có khách, Đã đặt)
- Gán order vào bàn

### 2.4 Quản lý Khách hàng
- Lưu thông tin khách hàng (Tên, Số điện thoại, Email)
- Theo dõi số lần ghé thăm và tổng chi tiêu
- Tìm kiếm khách hàng

### 2.5 Thống kê & Báo cáo
- Doanh thu theo ngày/tuần/tháng
- Top món bán chạy
- Thống kê order theo trạng thái
- Doanh thu theo giờ

---

## 3. Cài đặt

### 3.1 Yêu cầu hệ thống

- **Frontend**: Node.js 18+
- **Backend**: C++17 compiler (g++, clang++)
- **Database**: PostgreSQL 13+

### 3.2 Cài đặt PostgreSQL

1. Cài đặt PostgreSQL: https://www.postgresql.org/download/

2. Tạo database:
```bash
psql -U postgres
CREATE DATABASE smart_cafe_pos;
\q
```

3. Chạy schema:
```bash
cd backend/database
psql -U postgres -d smart_cafe_pos -f schema.sql
psql -U postgres -d smart_cafe_pos -f init.sql
```

### 3.3 Cài đặt Backend

**Windows:**
```bash
cd backend

g++ -std=c++17 -o bin/server.exe src/main.cpp src/Server.cpp src/ClientHandler.cpp src/OrderManager.cpp src/MenuManager.cpp src/TableManager.cpp src/CustomerManager.cpp src/StatsManager.cpp src/Database.cpp src/ApiServer.cpp src/logger.cpp -lws2_32 -lpq
```

**Linux/Mac:**
```bash
cd backend

g++ -std=c++17 -o bin/server src/main.cpp src/Server.cpp src/ClientHandler.cpp src/OrderManager.cpp src/MenuManager.cpp src/TableManager.cpp src/CustomerManager.cpp src/StatsManager.cpp src/Database.cpp src/ApiServer.cpp src/logger.cpp -lpthread -lpq
```

**Chạy server:**
```bash
./bin/server.exe    # Windows
./bin/server        # Linux/Mac
```

Server chạy:
- HTTP API: `http://localhost:8080`
- TCP Socket: `localhost:8081`

### 3.4 Cài đặt Frontend

```bash
cd frontend
npm install
npm run dev
```

Frontend chạy tại `http://localhost:5173`

---

## 4. API Endpoints

### Health Check
```
GET /api/health
```

### Menu
```
GET /api/menu                    # Lấy tất cả món
GET /api/menu?category=coffee    # Lấy món theo danh mục
```

### Orders
```
GET /api/orders                  # Lấy tất cả order
GET /api/orders/{id}             # Lấy order theo ID
GET /api/orders?status=new       # Lấy order theo trạng thái
POST /api/orders                 # Tạo order mới
PATCH /api/orders/{id}           # Cập nhật order
DELETE /api/orders/{id}          # Xóa order
```

### Tables
```
GET /api/tables                  # Lấy tất cả bàn
PATCH /api/tables                # Cập nhật bàn
```

### Customers
```
GET /api/customers               # Lấy tất cả khách hàng
GET /api/customers/{id}          # Lấy khách hàng theo ID
GET /api/customers?q=keyword     # Tìm kiếm khách hàng
POST /api/customers              # Tạo khách hàng mới
```

### Stats
```
GET /api/stats                   # Lấy tất cả thống kê
GET /api/stats?type=revenue&period=week
GET /api/stats?type=top-items&limit=5
GET /api/stats?type=orders
GET /api/stats?type=revenue-by-hour
```

---

## 5. Cấu trúc Database

### Tables

| Table | Mô tả |
|-------|-------|
| `menu_items` | Danh sách món trong menu |
| `tables` | Thông tin bàn |
| `customers` | Thông tin khách hàng |
| `orders` | Đơn hàng |
| `order_items` | Chi tiết các món trong order |
| `daily_stats` | Thống kê doanh thu theo ngày |
| `hourly_stats` | Thống kê doanh thu theo giờ |
| `item_stats` | Thống kê số lượng món đã bán |

---

## 6. Kiến trúc hệ thống

```
┌─────────────┐     HTTP/REST      ┌─────────────┐
│   Frontend  │ ────────────────> │  HTTP API   │
│   (React)   │ <───────────────── │  Server     │
└─────────────┘                    └──────┬──────┘
                                          │
                                    ┌─────▼─────┐
                                    │ Database   │
                                    │(PostgreSQL)│
                                    └───────────┘

┌─────────────┐     TCP Socket     ┌─────────────┐
│   Clients   │ ────────────────> │  TCP Server │
│   (Native) │ <───────────────── │             │
└─────────────┘                    └─────────────┘
```

---

## 7. Các bước tiếp theo

- [ ] Thêm authentication/authorization
- [ ] WebSocket cho real-time updates
- [ ] Deploy frontend + backend
- [ ] Thêm tính năng quản lý kho
- [ ] Tích hợp thanh toán online

---

## Tác giả

Copyright (c) 2026 Đinh Huyền Trang. MIT License.
