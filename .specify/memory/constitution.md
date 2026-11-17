<!--
Sync Impact Report:
- Version change: 0.0.0 (template) → 1.0.0 (ratified)
- List of modified principles:
  - PRINCIPLE_1: Created "I. Tương thích Arduino IDE & Cấu trúc Module hóa (Arduino IDE Compatibility & Modular Architecture)"
  - PRINCIPLE_2: Created "II. Lập trình không chặn với millis() (Non-blocking Execution)"
  - PRINCIPLE_3: Created "III. Tập trung hóa cấu hình trong config.h (Centralized Configuration)"
  - PRINCIPLE_4: Created "IV. Quản lý bộ nhớ & An toàn RAM (Memory Management & Static Allocation)"
  - PRINCIPLE_5: Created "V. Tự động phục hồi kết nối (Resilient Connectivity & Auto-reconnect)"
  - PRINCIPLE_6: Added "VI. Chuẩn mực tài liệu & Chú thích mã nguồn (Vietnamese Documentation & Commenting)"
- Added sections:
  - Tiêu chuẩn kỹ thuật phần cứng & Phần mềm (Hardware & Software Technical Standards)
  - Quy trình phát triển & Kiểm định chất lượng (Development Workflow & Quality Gates)
- Removed sections: None
- Follow-up TODOs: None
-->

# ESP32 test_spec_kit Constitution

## Core Principles

### I. Tương thích Arduino IDE & Cấu trúc Module hóa (Arduino IDE Compatibility & Modular Architecture)
- **Quy tắc bắt buộc**: Mã nguồn BẮT BUỘC tương thích 100% với Arduino IDE sử dụng ESP32 Arduino Core phiên bản 3.x. File chính BẮT BUỘC đặt tên là `test_spec_kit.ino`. Toàn bộ logic thu thập cảm biến và logic truyền thông kết nối BẮT BUỘC phải được tách thành các module `.h` và `.cpp` độc lập đặt cùng cấp trong thư mục gốc của dự án. File `.ino` chỉ giữ vai trò điều phối vòng lặp cấp cao và khởi tạo ngoại vi.
- **Tính kiểm thử & Đảm bảo**: Toàn bộ dự án phải biên dịch thành công trực tiếp thông qua Arduino IDE hoặc công cụ `arduino-cli` với board ESP32 mà không yêu cầu thêm thao tác cấu hình build phức tạp.

### II. Lập trình không chặn với millis() (Non-blocking Execution)
- **Quy tắc bắt buộc**: Toàn bộ mã nguồn thực thi định kỳ trong vòng lặp `loop()` và các module BẮT BUỘC phải áp dụng cơ chế non-blocking dựa trên bộ đếm thời gian `millis()`. TUYỆT ĐỐI KHÔNG sử dụng hàm `delay()` hoặc các vòng lặp chờ khóa (busy-wait loops) trong luồng thực thi chính.
- **Tính kiểm thử & Đảm bảo**: Quét toàn bộ codebase (static check) không phát hiện bất kỳ lệnh `delay()` nào trong `loop()`. Thời gian thực thi một chu kỳ của `loop()` BẮT BUỘC duy trì dưới 10ms để đảm bảo tính thời gian thực và không gây kích hoạt Software Watchdog Timer (WDT).

### III. Tập trung hóa cấu hình trong config.h (Centralized Configuration)
- **Quy tắc bắt buộc**: DUY NHẤT một file `config.h` được phép định nghĩa toàn bộ chân GPIO, thông số kết nối mạng WiFi (SSID, Password), địa chỉ máy chủ/Broker (IP, Port của KEPServerEX), cùng các ngưỡng thời gian (interval, timeout). TUYỆT ĐỐI KHÔNG hard-code bất kỳ tham số phần cứng, mật khẩu hoặc địa chỉ mạng nào trong file `.ino` hoặc các file module `.cpp`.
- **Tính kiểm thử & Đảm bảo**: Rà soát mã nguồn tự động hoặc code review: mọi hằng số mạng và chân GPIO chỉ được xuất hiện dưới dạng macro hoặc hằng số `constexpr` trong `config.h`.

### IV. Quản lý bộ nhớ & An toàn RAM (Memory Management & Static Allocation)
- **Quy tắc bắt buộc**: Nhằm tránh phân mảnh bộ nhớ Heap (heap fragmentation) dẫn đến sập hệ thống (panic/crash), toàn bộ thao tác xử lý chuỗi và đóng gói dữ liệu trong vòng lặp chính BẮT BUỘC ưu tiên sử dụng mảng ký tự tĩnh `char[]` với kích thước xác định trước và sử dụng các hàm an toàn như `snprintf()`. TUYỆT ĐỐI KHÔNG sử dụng đối tượng `String` động trong `loop()` hoặc các hàm chạy định kỳ.
- **Tính kiểm thử & Đảm bảo**: Giám sát dung lượng Heap khả dụng (`ESP.getFreeHeap()` và `ESP.getMinFreeHeap()`) sau 24 giờ vận hành liên tục không được suy giảm liên tục (zero memory leak). Không tồn tại đối tượng `String` trong các hàm chu kỳ.

### V. Tự động phục hồi kết nối (Resilient Connectivity & Auto-reconnect)
- **Quy tắc bắt buộc**: Thiết bị BẮT BUỘC trang bị cơ chế tự động kết nối lại (Auto-reconnect) độc lập và không chặn khi mất kết nối WiFi hoặc mất kết nối tới máy chủ công nghiệp KEPServerEX. Quá trình kiểm tra và kết nối lại BẮT BUỘC không được làm treo ESP32, không kích hoạt WDT Reset, và không làm gián đoạn việc đọc cảm biến cục bộ.
- **Tính kiểm thử & Đảm bảo**: Kiểm thử ngắt kết nối mạng hoặc tắt máy chủ KEPServerEX: ESP32 vẫn tiếp tục đọc cảm biến theo chu kỳ và tự động tái lập phiên truyền thông thành công ngay khi mạng hoặc máy chủ hoạt động trở lại.

### VI. Chuẩn mực tài liệu & Chú thích mã nguồn (Vietnamese Documentation & Commenting)
- **Quy tắc bắt buộc**: Toàn bộ ghi chú giải thích mã nguồn (comments), tài liệu đặc tả kỹ thuật, hướng dẫn cài đặt và log gỡ lỗi nội bộ BẮT BUỘC phải viết bằng Tiếng Việt rõ ràng, chuẩn ngữ pháp và đúng thuật ngữ kỹ thuật chuyên ngành.
- **Tính kiểm thử & Đảm bảo**: Đánh giá qua Code Review: mỗi file mã nguồn BẮT BUỘC có khối mô tả chức năng (header doc) bằng Tiếng Việt, các hàm và biến phức tạp phải có chú thích Tiếng Việt đi kèm.

## Tiêu chuẩn kỹ thuật phần cứng & Phần mềm (Hardware & Software Technical Standards)

- **Vi điều khiển đích**: ESP32 Series (Espressif Systems), ESP32 Arduino Core v3.x.
- **Môi trường phát triển**: Arduino IDE 2.x hoặc `arduino-cli`.
- **Cấu trúc tệp tin bắt buộc**:
  - `test_spec_kit.ino`: Tệp phác thảo chính, chứa `setup()` và `loop()`.
  - `config.h`: Tệp cấu hình tập trung cho toàn bộ chân cắm và tham số hệ thống.
  - Các cặp `[tên_module].h` và `[tên_module].cpp` đặt tại thư mục gốc cho từng thành phần (ví dụ: `sensor.h`/`.cpp`, `communication.h`/`.cpp`).
- **Giao thức công nghiệp**: Tích hợp với KEPServerEX thông qua các chuẩn truyền thông nhúng tiêu chuẩn (IoT Gateway, MQTT, Modbus TCP hoặc REST/HTTP), đáp ứng nghiêm ngặt tính sẵn sàng cao và khả năng tự phục hồi.

## Quy trình phát triển & Kiểm định chất lượng (Development Workflow & Quality Gates)

- **Gate 1 - Thẩm định kiến trúc & Cấu hình**: Kiểm tra file `config.h` đã bao quát đầy đủ chân GPIO và thông số mạng; không có cấu hình phân tán.
- **Gate 2 - Thẩm định biên dịch & Cú pháp**: Biên dịch thành công với ESP32 Core 3.x, không có cảnh báo nghiêm trọng (warnings-as-errors cho các lỗi tràn mảng buffer).
- **Gate 3 - Thẩm định an toàn Non-blocking & Bộ nhớ**:
  - Không có lệnh `delay()` trong vòng lặp chính.
  - Sử dụng 100% `snprintf()` và buffer tĩnh cho định dạng chuỗi tuần hoàn.
- **Gate 4 - Thẩm định kiểm thử ngắt kết nối (Fault-Injection Test)**: Thử nghiệm ngắt kết nối WiFi và KEPServerEX trong môi trường mô phỏng/thực tế, kiểm tra hành vi tự phục hồi của thiết bị mà không bị reset ngoài ý muốn.
- **Gate 5 - Rà soát tài liệu**: Đảm bảo tất cả mã nguồn và tài liệu liên quan được viết bằng Tiếng Việt.

## Governance

Hiến pháp này là văn bản quy chuẩn tối cao định hướng toàn bộ hoạt động thiết kế, triển khai mã nguồn và đánh giá chất lượng của dự án.

- **Hiệu lực thi hành**: Mọi đề xuất mã nguồn (Pull Request, commit) và tài liệu kỹ thuật phát sinh đều BẮT BUỘC tuân thủ đầy đủ 6 nguyên tắc cốt lõi trên.
- **Quy trình sửa đổi (Amendment Procedure)**: Mọi thay đổi đối với hiến pháp này phải được thảo luận, ghi nhận lý do kỹ thuật và đánh giá tác động trước khi cập nhật.
- **Chính sách phiên bản**: Phiên bản hiến pháp tuân thủ chuẩn Semantic Versioning (`MAJOR.MINOR.PATCH`):
  - **MAJOR**: Thay đổi hoặc bãi bỏ các nguyên tắc cốt lõi làm mất tính tương thích ngược với chuẩn cũ.
  - **MINOR**: Bổ sung nguyên tắc mới hoặc mở rộng hướng dẫn kỹ thuật chi tiết.
  - **PATCH**: Hiệu chỉnh câu chữ, sửa lỗi chính tả hoặc làm rõ định nghĩa.
- **Tài liệu tham chiếu**: Toàn bộ quy trình sinh mã và tài liệu đặc tả trong Spec Kit phải đối chiếu trực tiếp với tệp `.specify/memory/constitution.md` này.

**Version**: 1.0.0 | **Ratified**: 2026-11-17 | **Last Amended**: 2026-11-17
