-- Таблица автомобилей
CREATE TABLE IF NOT EXISTS cars (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    number TEXT NOT NULL UNIQUE,
    brand TEXT NOT NULL,
    mileage_at_purchase INTEGER NOT NULL,
    capacity INTEGER NOT NULL
);

-- Таблица водителей
CREATE TABLE IF NOT EXISTS drivers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    tab_number TEXT NOT NULL UNIQUE,
    lastname TEXT NOT NULL,
    category TEXT NOT NULL,
    experience INTEGER NOT NULL,
    address TEXT,
    birth_year INTEGER
);

-- Таблица заказов
CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    date TEXT NOT NULL,
    driver_lastname TEXT NOT NULL,
    car_number TEXT NOT NULL,
    mileage INTEGER NOT NULL,
    cargo_mass INTEGER NOT NULL,
    cost REAL NOT NULL,
    FOREIGN KEY (driver_lastname) REFERENCES drivers(lastname),
    FOREIGN KEY (car_number) REFERENCES cars(number)
);

-- Таблица пользователей системы
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    login TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL
);

-- Таблица зарплаты водителей (для функции 5)
CREATE TABLE IF NOT EXISTS driver_salary (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    driver_id INTEGER NOT NULL,
    start_date TEXT NOT NULL,
    end_date TEXT NOT NULL,
    total_amount REAL NOT NULL,
    FOREIGN KEY (driver_id) REFERENCES drivers(id)
);

-- Начальные данные
INSERT OR IGNORE INTO cars (number, brand, mileage_at_purchase, capacity) VALUES
('A001AA', 'MAN', 120000, 5000),
('B002BB', 'Mercedes', 85000, 8000),
('C003CC', 'Volvo', 45000, 6000);

INSERT OR IGNORE INTO drivers (tab_number, lastname, category, experience, address, birth_year) VALUES
('T001', 'Ivanov', 'C', 15, 'Lenina 10', 1980),
('T002', 'Petrov', 'B', 8, 'Sovetskaya 5', 1985),
('T003', 'Sidorov', 'C', 20, 'Nezavisimosti 15', 1975);

INSERT OR IGNORE INTO orders (date, driver_lastname, car_number, mileage, cargo_mass, cost) VALUES
('2024-01-15', 'Ivanov', 'A001AA', 150, 3000, 15000),
('2024-01-20', 'Petrov', 'B002BB', 200, 5000, 25000),
('2024-01-25', 'Sidorov', 'C003CC', 120, 4000, 20000);

INSERT OR IGNORE INTO users (login, password_hash, role) VALUES
('ivanov', 'ivanov123', 'driver'),
('petrov', 'petrov123', 'driver'),
('admin', 'admin123', 'manager');