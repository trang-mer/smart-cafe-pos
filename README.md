# Smart Cafe POS — Server/Client TCP

## 1. Tổng quan

Dự án là một hệ thống **TCP Server–Client** viết bằng C++ thuần, dùng cho quản lý quán cafe. Server lắng nghe kết nối từ nhiều Client, phân quyền theo vai trò, và cho phép gửi/nhận tin nhắn giữa các Client qua Server.

- **Server**: chạy trên cổng `8080`, chấp nhận nhiều kết nối đồng thời qua multi-threading.
- **Client**: kết nối TCP đến Server, gửi lệnh theo vai trò (CASHIER / KITCHEN / MANAGER).
- **Hỗ trợ đa nền tảng**: Windows (Winsock2) và Unix/Linux (BSD socket).

---

## 2. Cấu trúc thư mục

```
backend/
├── include/               # File header (.h)
│   ├── Server.h           # Định nghĩa class Server
│   ├── ClientHandler.h    # Định nghĩa class ClientHandler
│   └── logger.h           # Định nghĩa class Logger
├── src/                   # File triển khai (.cpp)
│   ├── main.cpp           # Điểm khởi chạy của Server
│   ├── Server.cpp
│   ├── ClientHandler.cpp
│   └── logger.cpp
├── client/                # Source code Client
│   └── client.cpp
└── bin/                   # File thực thi sau khi build
    ├── server.exe
    └── client.exe
```

---

## 3. Vai trò hệ thống

Hệ thống định nghĩa 4 vai trò (enum `ClientRole`):

| Vai trò    | Mô tả                              |
|------------|-------------------------------------|
| `UNKNOWN`  | Chưa xác định (mặc định khi kết nối) |
| `CASHIER`  | Thu ngân — gửi đơn hàng             |
| `KITCHEN`  | Bếp — nhận đơn, cập nhật trạng thái |
| `MANAGER`  | Quản lý — xem báo cáo               |

---

## 4. Chi tiết từng thành phần

### 4.1. Server (`Server.h` / `Server.cpp`)

**Khởi tạo & lắng nghe:**
- `Server(port)` — khởi tạo socket server trên cổng truyền vào (mặc định `8080`).
- `start()` — khởi tạo Winsock2 (Windows), tạo socket, bind, listen. Trả về `true` nếu thành công.
- `run()` — vòng lặp `accept()` chờ Client kết nối. Mỗi Client mới được giao một thread riêng chạy `ClientHandler`.

**Quản lý Client:**
- `addClient()` — thêm Client vào danh sách, mặc định vai trò `UNKNOWN`.
- `removeClient()` — xoá Client khỏi danh sách khi ngắt kết nối.
- `setClientRole()` / `getClientRole()` — cập nhật hoặc đọc vai trò của một Client.
- Danh sách Client được bảo vệ bởi `std::mutex` để đảm bảo thread-safety.

**Gửi tin nhắn:**
- `sendToClient()` — gửi tin nhắn đến một Client cụ thể.
- `sendToRole()` — broadcast tin nhắn đến tất cả Client cùng vai trò.
- `sendToRoles()` — broadcast đến nhiều vai trò cùng lúc.
- `parseRole()` / `roleToString()` — chuyển đổi giữa chuỗi `"CASHIER"`/`"KITCHEN"`/`"MANAGER"` và enum `ClientRole`.

**Dọn dẹp:**
- `stop()` — đóng socket, gọi `WSACleanup()` (Windows) hoặc `close()` (Unix).

---

### 4.2. ClientHandler (`ClientHandler.h` / `ClientHandler.cpp`)

- **Khởi tạo:** nhận `SOCKET` của Client và con trỏ `Server` để tương tác.
- **`handle()` — vòng lặp chính:**
  1. `recv()` chờ dữ liệu từ Client (buffer 1024 bytes).
  2. Nếu `bytesReceived <= 0` → Client ngắt kết nối → thoát vòng lặp.
  3. `processMessage()` xử lý nội dung nhận được.
  4. Gọi `server->broadcastMessage()` gửi tin đến tất cả Client khác.
  5. Khi thoát: gọi `server->removeClient()` rồi đóng socket.

> **Lưu ý:** hiện tại `ClientHandler::processMessage()` chưa triển khai — đây là nơi cần mở rộng xử lý logic theo vai trò (ORDER, STATUS, REPORT…).

---

### 4.3. Logger (`logger.h` / `logger.cpp`)

- **`Logger::info(message)`** — in ra `stdout` với định dạng `[INFO][<thời gian>] <message>`.
- **`Logger::error(message)`** — in ra `stderr` với định dạng `[ERROR][<thời gian>] <message>`.
- Thời gian được lấy qua `ctime_s()` (Windows-safe), tự động loại bỏ ký tự xuống dòng cuối.

---

### 4.4. Client (`client.cpp`)

**Kết nối:**
- Chấp nhận tham số dòng lệnh: `client.exe <IP> <port>` (mặc định `127.0.0.1:8080`).
- Khởi tạo Winsock2 → tạo socket → `connect()` đến Server.

**Nhận tin (thread riêng):**
- `receiveMessages()` — chạy trên thread phụ, liên tục `recv()`, in tin nhắn nhận được ra màn hình.
- Dùng `std::atomic<bool> running` để đồng bộ trạng thái kết thúc.

**Gửi tin:**
- Người dùng nhập vai trò: nhập `CASHIER` / `KITCHEN` / `MANAGER` → gửi lệnh `ROLE <role>`.
- Sau đó nhập lệnh theo vai trò:
  - `CASHIER`: `ORDER table=1 items=Latte x2`
  - `KITCHEN`: `STATUS order=1 cooking`
  - `MANAGER`: `REPORT daily revenue checked`
- Gõ `/quit` để ngắt kết nối và thoát.

**Dọn dẹp:** đóng socket, `WSACleanup()`, join thread.

---

## 5. Cách build & chạy

### Build (trên Windows)

```bash
# Build Server
g++ -std=c++17 -o backend/bin/server.exe backend/src/main.cpp backend/src/Server.cpp backend/src/ClientHandler.cpp backend/src/logger.cpp -lws2_32 -lpthread

# Build Client
g++ -std=c++17 -o backend/bin/client.exe backend/client/client.cpp -lws2_32 -lpthread
```

### Chạy Server

```bash
backend/bin/server.exe
# Server started on port 8080
```

### Chạy Client (nhiều cửa sổ terminal)

```bash
# Terminal 1 — thu ngân
backend/bin/client.exe 127.0.0.1 8080
# Nhập: CASHIER

# Terminal 2 — bếp
backend/bin/client.exe 127.0.0.1 8080
# Nhập: KITCHEN

# Terminal 3 — quản lý
backend/bin/client.exe 127.0.0.1 8080
# Nhập: MANAGER
```

---

## 6. Protocol mặc định (hiện tại)

Tin nhắn được truyền dạng **plain text** qua TCP:

```
ROLE <CASHIER|KITCHEN|MANAGER>
ORDER table=1 items=Latte x2
STATUS order=1 cooking
REPORT daily revenue checked
```

> **Mở rộng trong tương lai:** cần triển khai `broadcastMessage()` trong `Server.cpp` (hiện chưa thấy định nghĩa) và hoàn thiện `processMessage()` trong `ClientHandler` để xử lý logic nghiệp vụ đầy đủ.

---

## 7. Tác giả

Copyright (c) 2026 Đinh Huyền Trang. Phát hành theo MIT License.
