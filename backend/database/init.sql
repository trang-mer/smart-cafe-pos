-- Smart Cafe POS - Initialize Default Data
-- PostgreSQL

-- ============================================
-- Initialize Menu Items
-- ============================================

INSERT INTO menu_items (name, price, category, emoji, bg_class, available) VALUES
('Espresso', 25000, 'coffee', '☕', 'from-amber-900 to-amber-700', TRUE),
('Americano', 30000, 'coffee', '☕', 'from-stone-800 to-stone-600', TRUE),
('Latte', 40000, 'coffee', '🥛', 'from-amber-200 to-amber-400', TRUE),
('Cappuccino', 40000, 'coffee', '☕', 'from-amber-300 to-amber-500', TRUE),
('Mocha', 45000, 'coffee', '🍫', 'from-amber-800 to-amber-600', TRUE),
('Caramel Macchiato', 45000, 'coffee', '🍮', 'from-orange-300 to-amber-500', TRUE),
('Matcha Latte', 40000, 'tea', '🍵', 'from-green-300 to-green-500', TRUE),
('Trà đào', 35000, 'tea', '🍑', 'from-orange-200 to-pink-300', TRUE),
('Trà vải', 35000, 'tea', '🍒', 'from-pink-200 to-rose-300', TRUE),
('Chocolate đá xay', 50000, 'blended', '🍫', 'from-amber-900 to-amber-700', TRUE),
('Cookies', 20000, 'cake', '🍪', 'from-amber-300 to-amber-500', TRUE),
('Tiramisu', 45000, 'cake', '🍰', 'from-amber-200 to-amber-400', TRUE)
ON CONFLICT DO NOTHING;

-- ============================================
-- Initialize Tables
-- ============================================

INSERT INTO tables (name, status) 
SELECT 'Bàn ' || i, 'available'
FROM generate_series(1, 10) AS i
ON CONFLICT DO NOTHING;

-- ============================================
-- Initialize Customers
-- ============================================

INSERT INTO customers (name, phone, visits, total_spent) VALUES
('Nguyễn Văn A', '0901234567', 24, 2400000),
('Trần Thị B', '0907654321', 18, 1800000),
('Lê Văn C', '0912345678', 12, 980000),
('Phạm Thị D', '0934567890', 8, 720000)
ON CONFLICT DO NOTHING;
