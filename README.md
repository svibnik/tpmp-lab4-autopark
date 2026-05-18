# Автопарк - Система управления

## Описание
Консольное приложение для управления автопарком, водителями и заказами.

## Установка

### Linux / WSL

```  
sudo apt-get install sqlite3 libsqlite3-dev libcunit1-dev
make all
sqlite3 autopark.db < database/schema.sql
```  

### Запуск
```  
./bin/autopark
```  

### Тестовые учётные записи

| Роль | Логин | Пароль |
|------|-------|--------|
| Менеджер | admin | admin123 |
| Водитель | ivanov | ivanov123 |
| Водитель | petrov | petrov123 |

### Запуск тестов
```  
make test
```  

## Репозиторий с кодом и CI/CD
[https://github.com/svibnik/tpmp-lab4-autopark](https://github.com/svibnik/tpmp-lab4-autopark)