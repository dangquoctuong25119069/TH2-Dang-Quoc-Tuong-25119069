## Bài 1 — quan sát và kết luận
Trong mô hình, đo khoảng giữa hai lần đổi trạng thái LED (mục tiêu 500 ms):

| Cách tạo nhịp | Không thêm tải | Chặn loop 800 ms mỗi vòng |
|---|---|---|
| `delay()` | Luân phiên khoảng 500 / 7500 ms | Luân phiên khoảng 500 / 8300 ms |
| `millis()` | Khoảng 500 ms | Khoảng 800 ms |
| Hardware timer | 500 ms | 500 ms |

**Quan sát:** Các lệnh delay nối tiếp làm LED tắt lâu; ADC và heartbeat cũng phải
chờ hết các bước trước. Với millis, các công việc giữ được lịch riêng khi loop nhẹ,
nhưng đổi LED bị trễ khi loop bận. Trong mô hình timer, sự kiện vẫn đến mỗi 500 ms,
không mất mẫu trong các lần chạy đã kiểm tra.

**Kết luận:** millis phù hợp để tổ chức nhiều công việc không chặn nhau, nhưng vẫn
phụ thuộc tốc độ phục vụ của loop. Timer ngắt phù hợp cho sự kiện cần giữ nhịp khi
loop bận; ISR phải ngắn và chuyển việc in Serial về loop.

## Bài 2 — quan sát và kết luận
Qua 3 lượt khởi động mô phỏng, trước khi tăng: biến RTC là **0 → 1 → 2**,
biến RAM thường luôn **0**; sau tăng, biến RAM thường luôn **1**.
Cả hai ví dụ đều yêu cầu timer đánh thức sau **10.000.000 µs (10 s)** và tắt LED
trước khi gọi deep sleep. Ví dụ `sleep_blink` yêu cầu ngủ sau khoảng **5,3 s** kể từ
đầu setup: 0,3 s khởi tạo và 5 s hoạt động.

**Kết luận:** Chương trình được tổ chức để khởi tạo lại ở setup sau deep sleep và
lưu bộ đếm qua RTC_DATA_ATTR.