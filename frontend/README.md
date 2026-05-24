# Smart Cafe POS - Frontend

Hệ thống POS (Point of Sale) cho quán cafe với 3 vai trò: Thu ngân, Bếp, và Quản lý.

## Tech Stack

- **React 18** - UI framework
- **TanStack Router** - Routing
- **TanStack Start** - SSR framework
- **TanStack Query** - Data fetching
- **Tailwind CSS v4** - Styling
- **Vite 7** - Build tool

## Cấu trúc thư mục

```
src/
├── components/     # UI components (shadcn/ui)
├── routes/         # Page routes
├── lib/            # Utilities & data
├── hooks/          # Custom React hooks
├── main.tsx        # Entry point
├── app.tsx         # App component
└── styles.css      # Global styles
```

## Cài đặt

```bash
# Cài dependencies
npm install

# Chạy development server
npm run dev

# Build production
npm run build

# Preview production build
npm run preview
```

## Scripts

| Command | Mô tả |
|---------|-------|
| `npm run dev` | Chạy dev server tại http://localhost:3000 |
| `npm run build` | Build production bundle |
| `npm run preview` | Preview production build |

## Routes

- `/` - Trang chủ
- `/cashier` - Giao diện thu ngân
  - `/cashier/menu` - Menu đồ uống
  - `/cashier/new` - Tạo order mới
  - `/cashier/orders` - Danh sách orders
  - `/cashier/payment` - Thanh toán
  - `/cashier/history` - Lịch sử giao dịch
  - `/cashier/customers` - Quản lý khách hàng
- `/kitchen` - Giao diện bếp
- `/manager` - Giao diện quản lý (thống kê)

## Development

Dev server chạy với Hot Module Replacement (HMR) - code changes sẽ tự động refresh mà không mất state.

## Build

Production build được tối ưu hóa với code splitting và tree shaking.
