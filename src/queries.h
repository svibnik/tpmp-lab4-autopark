#ifndef QUERIES_H
#define QUERIES_H

void get_orders_by_driver_period(const char* lastname, const char* start_date, const char* end_date, const char* current_username, const char* current_role);
void get_car_stats(const char* car_number);
void get_all_drivers_stats(void);
void get_driver_with_fewest_trips(void);
void get_car_with_max_mileage(void);

#endif