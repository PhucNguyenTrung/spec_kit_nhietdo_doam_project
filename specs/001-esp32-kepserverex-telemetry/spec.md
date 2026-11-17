# Feature Specification: ESP32 Temperature & Humidity Telemetry to KEPServerEX

**Feature Branch**: `001-esp32-kepserverex-telemetry`

**Created**: 2026-11-17

**Status**: Draft

**Input**: User description: "Xây dựng firmware cho ESP32 thực hiện thu thập dữ liệu nhiệt độ và độ ẩm từ cảm biến định kỳ, sau đó đẩy dữ liệu thời gian thực sang phần mềm KEPServerEX trên máy tính/server. Hệ thống có LED báo trạng thái kết nối mạng/server, tự động phục hồi kết nối khi rớt mạng và xuất thông tin chẩn đoán qua Serial Monitor."

## Clarifications

### Session 2026-11-17
- Q: Hệ thống nên sử dụng giao thức truyền thông nào để đẩy dữ liệu thời gian thực từ ESP32 sang phần mềm KEPServerEX? (FR-002) → A: Giao thức HTTP REST (ESP32 đóng vai trò HTTP Client gửi yêu cầu HTTP POST kèm JSON payload trực tiếp đến KEPServerEX IoT Gateway REST Server Agent).
- Q: Dự án dự định sử dụng loại cảm biến nhiệt độ và độ ẩm cụ thể nào để kết nối với ESP32? (FR-001) → A: Cảm biến Sensirion SHT31-D qua giao thức truyền thông I2C (chân SDA và SCL, địa chỉ I2C mặc định 0x44).
- Q: Định dạng bản tin JSON HTTP POST gửi đến KEPServerEX IoT Gateway REST Server nên theo cấu trúc nào? (FR-002) → A: Định dạng mảng tag chuẩn của KEPServerEX IoT Gateway: `[{"id":"<Tag_Name>","v":<Value>}]`.
- Q: Hệ thống nên sử dụng cấu hình phần cứng đèn LED chỉ báo trạng thái như thế nào? (FR-006) → A: Sử dụng 2 đèn LED đơn sắc độc lập: 1 LED chỉ báo kết nối WiFi (nhấp nháy khi đang kết nối, sáng liên tục khi đã có mạng) và 1 LED chỉ báo kết nối máy chủ KEPServerEX (chớp nháy xác nhận mỗi lần gửi dữ liệu HTTP thành công, tắt/cảnh báo khi gửi thất bại hoặc mất liên lạc).
- Q: KEPServerEX IoT Gateway REST Server có yêu cầu cơ chế xác thực nào khi ESP32 gửi HTTP POST không? (FR-002) → A: Không yêu cầu xác thực (No Auth / None), bản tin HTTP POST gửi trực tiếp trong mạng LAN công nghiệp OT cô lập không cần Header Authorization.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Thu thập và đẩy dữ liệu môi trường thời gian thực lên KEPServerEX (Priority: P1)

Là người vận hành hệ thống giám sát SCADA / IoT công nghiệp, tôi muốn thiết bị ESP32 tự động thu thập định kỳ các thông số nhiệt độ và độ ẩm từ cảm biến môi trường và gửi trực tiếp theo thời gian thực tới phần mềm KEPServerEX trên máy chủ, để các ứng dụng cấp cao (SCADA, MES, Dashboard) có thể giám sát liên tục tình trạng môi trường nhà xưởng.

**Why this priority**: Đây là giá trị cốt lõi (MVP) của toàn bộ dự án. Nếu không có chức năng thu thập và truyền dữ liệu này, thiết bị không hoàn thành mục tiêu giám sát.

**Independent Test**: Kết nối cảm biến với bo mạch ESP32, cấu hình địa chỉ mạng và thông số KEPServerEX. Khởi động thiết bị và quan sát trên giao diện KEPServerEX (hoặc OPC Quick Client); các giá trị nhiệt độ (°C) và độ ẩm (%RH) phải được cập nhật định kỳ đúng chu kỳ và hiển thị chất lượng dữ liệu tốt (Good Quality).

**Acceptance Scenarios**:

1. **Given** Thiết bị đã kết nối thành công với mạng WiFi và máy chủ KEPServerEX, **When** Đến chu kỳ lấy mẫu định kỳ (mặc định 5 giây), **Then** Thiết bị đọc dữ liệu từ cảm biến và truyền thành công gói tin chứa giá trị nhiệt độ và độ ẩm đến KEPServerEX; dữ liệu trên KEPServerEX được cập nhật tức thời với nhãn thời gian tương ứng.
2. **Given** Cảm biến gặp sự cố phần cứng hoặc bị ngắt kết nối vật lý, **When** Đến chu kỳ lấy mẫu dữ liệu, **Then** Thiết bị phát hiện trạng thái lỗi cảm biến, không gửi dữ liệu rác làm sai lệch hệ thống giám sát, và phát cảnh báo lỗi cảm biến qua cổng Serial.

---

### User Story 2 - Tự động phục hồi kết nối mạng và máy chủ khi gặp sự cố gián đoạn (Priority: P2)

Là kỹ sư bảo trì và vận hành hệ thống, tôi muốn thiết bị có khả năng tự động phát hiện mất kết nối WiFi hoặc mất liên lạc với KEPServerEX và tự động phục hồi kết nối khi hạ tầng sẵn sàng trở lại mà không cần sự can thiệp thủ công hay phải khởi động lại (reset) thiết bị tại hiện trường.

**Why this priority**: Thiết bị IoT trong môi trường công nghiệp bắt buộc phải có khả năng tự phục hồi (Self-Healing) để đảm bảo độ tin cậy và tính liên tục của dữ liệu mà không làm tốn chi phí nhân công vận hành.

**Independent Test**: Trong khi thiết bị đang truyền dữ liệu bình thường, chủ động tắt điểm phát sóng WiFi hoặc tắt dịch vụ KEPServerEX trong một khoảng thời gian, sau đó kích hoạt lại; kiểm tra thiết bị tự động khôi phục kết nối và tiếp tục truyền dữ liệu bình thường mà không bị treo hay khởi động lại ngoài ý muốn.

**Acceptance Scenarios**:

1. **Given** Thiết bị đang vận hành bình thường, **When** Điểm truy cập WiFi bị mất tín hiệu, **Then** Thiết bị nhận biết mất kết nối mạng, chuyển sang cơ chế thử kết nối lại định kỳ theo cách không chặn (non-blocking) và vẫn duy trì chu kỳ đọc cảm biến cục bộ.
2. **Given** Mạng WiFi đã hoạt động trở lại, **When** Đến lượt kiểm tra kết nối định kỳ của thiết bị, **Then** Thiết bị tự động kết nối lại WiFi thành công và tái lập kết nối tới KEPServerEX để tiếp tục truyền dữ liệu.
3. **Given** Kết nối WiFi hoạt động tốt nhưng phần mềm KEPServerEX bị đóng hoặc máy chủ tắt, **When** Thiết bị gửi dữ liệu thất bại, **Then** Thiết bị đánh dấu trạng thái mất kết nối máy chủ, kích hoạt cơ chế thử kết nối lại định kỳ mà không làm tắc nghẽn luồng xử lý chính.

---

### User Story 3 - Chỉ báo trực quan trạng thái vận hành qua 2 đèn LED riêng biệt (Priority: P3)

Là kỹ thuật viên lắp đặt và kiểm tra tại hiện trường, tôi muốn quan sát 2 đèn LED độc lập trên thiết bị (một LED cho WiFi và một LED cho KEPServerEX) để nhận biết trực quan ngay lập tức trạng thái của từng phân hệ (WiFi đang kết nối/đã kết nối, KEPServerEX truyền tin thành công/lỗi) mà không cần phải mang máy tính cắm dây kiểm tra.

**Why this priority**: Cung cấp phản hồi thị giác trực quan, tách biệt rõ ràng giữa sự cố tầng mạng WiFi và sự cố tầng ứng dụng máy chủ KEPServerEX, giúp việc khoanh vùng lỗi tại hiện trường diễn ra tức thì.

**Independent Test**: Quan sát chu kỳ chớp tắt của 2 đèn LED trong các kịch bản: WiFi đang kết nối (LED WiFi nhấp nháy), WiFi kết nối thành công (LED WiFi sáng liên tục), gửi gói tin HTTP thành công lên KEPServerEX (LED KEPServerEX chớp nháy xác nhận), và khi giả lập lỗi KEPServerEX (LED KEPServerEX tắt hoặc chớp cảnh báo lỗi).

**Acceptance Scenarios**:

1. **Given** Thiết bị đang tìm kiếm và thiết lập kết nối mạng WiFi, **When** Quá trình kết nối đang diễn ra, **Then** Đèn LED WiFi nhấp nháy đều đặn với tần số định trước (ví dụ 500ms).
2. **Given** Thiết bị đã kết nối thành công vào mạng WiFi, **When** Kết nối WiFi ổn định, **Then** Đèn LED WiFi duy trì trạng thái sáng liên tục.
3. **Given** Thiết bị gửi thành công gói tin dữ liệu qua HTTP POST tới KEPServerEX, **When** Máy chủ phản hồi mã trạng thái HTTP 200 OK, **Then** Đèn LED KEPServerEX phát một xung chớp sáng ngắn xác nhận truyền dữ liệu thành công.
4. **Given** Thiết bị không thể gửi dữ liệu tới KEPServerEX (mất kết nối máy chủ hoặc lỗi HTTP), **When** Chu kỳ truyền tin thất bại, **Then** Đèn LED KEPServerEX chuyển sang trạng thái tắt hoặc nhấp nháy cảnh báo lỗi.

---

### User Story 4 - Xuất thông tin chẩn đoán qua Serial Monitor (Priority: P4)

Là kỹ sư tích hợp hệ thống, tôi muốn theo dõi các thông điệp chẩn đoán chi tiết qua cổng nối tiếp (Serial Monitor) để xem thông tin địa chỉ IP, cường độ tín hiệu WiFi (RSSI), dữ liệu cảm biến đo được và nhật ký lỗi truyền thông trong quá trình tích hợp và thử nghiệm.

**Why this priority**: Hỗ trợ đắc lực cho công tác phát triển, kiểm thử, tích hợp ban đầu và tìm nguyên nhân gốc rễ (root cause analysis) khi phát sinh sự cố.

**Independent Test**: Kết nối ESP32 với máy tính qua cổng giao tiếp nối tiếp, mở phần mềm giám sát cổng nối tiếp (Serial Monitor) ở tốc độ baud tiêu chuẩn và kiểm tra nội dung log xuất ra khi thiết bị chạy.

**Acceptance Scenarios**:

1. **Given** Thiết bị đang kết nối với máy tính qua cổng nối tiếp, **When** Các sự kiện hệ thống diễn ra (khởi tạo, kết nối mạng, lấy mẫu, truyền tin), **Then** Thiết bị xuất các thông điệp Tiếng Việt rõ ràng lên màn hình chứa địa chỉ IP, RSSI, giá trị nhiệt độ, độ ẩm và kết quả gửi tin.
2. **Given** Xảy ra lỗi đọc cảm biến hoặc lỗi truyền tin tới KEPServerEX, **When** Sự cố xuất hiện, **Then** Thiết bị xuất thông báo lỗi chi tiết bằng Tiếng Việt kèm mã lỗi/nguyên nhân chẩn đoán mà không làm tràn ngập dữ liệu rác trên cổng nối tiếp.

---

### Edge Cases

- **Cảm biến bị mất kết nối hoặc hỏng hóc**: Khi chân tín hiệu cảm biến bị lỏng hoặc phần tử cảm biến bị lỗi, hàm đọc dữ liệu trả về giá trị không hợp lệ. Hệ thống phải nhận diện lỗi, không gửi các giá trị rác (như 0.0 hoặc giá trị âm bất thường) lên KEPServerEX làm sai lệch dữ liệu lịch sử, đồng thời thông báo lỗi qua đèn LED và cổng Serial.
- **Mạng chập chờn liên tục (Network Flapping)**: Tín hiệu WiFi bị ngắt quãng và có lại liên tục trong khoảng thời gian ngắn. Hệ thống phải có khoảng thời gian trễ bảo vệ (retry backoff interval), tránh việc tạo ra các luồng tái kết nối dồn dập làm cạn kiệt tài nguyên mạng hoặc gây tràn bộ nhớ.
- **Máy chủ KEPServerEX đóng kết nối bất ngờ**: KEPServerEX khởi động lại hoặc ngắt phiên kết nối giữa chừng khi ESP32 đang truyền gói tin. Hệ thống phải xử lý hết thời gian chờ (timeout) an toàn, giải phóng phiên kết nối cũ và tự động kết nối lại ở chu kỳ tiếp theo mà không gây treo vi điều khiển.
- **Giá trị đo môi trường ngoài dải cho phép**: Nhiệt độ hoặc độ ẩm đo được vượt quá ngưỡng vật lý của cảm biến (ví dụ độ ẩm > 100% hoặc nhiệt độ > 80°C do nhiễu điện). Hệ thống phải lọc và ghi nhận cảnh báo giá trị bất thường.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Hệ thống BẮT BUỘC định kỳ thu thập giá trị nhiệt độ (độ chính xác ±0.2°C) và độ ẩm không khí (độ chính xác ±2% RH) từ cảm biến Sensirion SHT31-D thông qua giao thức I2C (địa chỉ mặc định 0x44) theo chu kỳ có thể cấu hình được.
- **FR-002**: Hệ thống BẮT BUỘC định dạng dữ liệu viễn trắc dưới dạng mảng JSON chuẩn của KEPServerEX IoT Gateway (`[{"id":"<Tag_Nhiet_Do>","v":<Gia_Tri>},{"id":"<Tag_Do_Am>","v":<Gia_Tri>}]`) và truyền tải thời gian thực tới KEPServerEX thông qua giao thức HTTP REST (yêu cầu HTTP POST gửi trực tiếp không cần xác thực Header Authorization trong mạng LAN nội bộ đến KEPServerEX IoT Gateway REST Server Agent theo URL endpoint và cổng dịch vụ đã cấu hình).
- **FR-003**: Hệ thống BẮT BUỘC kiểm tra tính toàn vẹn và hợp lệ của dữ liệu cảm biến trước khi truyền; nếu dữ liệu không hợp lệ, hệ thống phải giữ trạng thái an toàn, không đẩy dữ liệu lỗi lên máy chủ và kích hoạt cảnh báo lỗi cảm biến.
- **FR-004**: Hệ thống BẮT BUỘC tự động kiểm tra trạng thái kết nối mạng WiFi và thực hiện kết nối lại tự động (Auto-reconnect) theo phương thức không chặn (non-blocking) khi bị rớt mạng, không làm đóng băng chu kỳ đọc cảm biến cục bộ.
- **FR-005**: Hệ thống BẮT BUỘC tự động tái lập phiên truyền thông với phần mềm KEPServerEX khi máy chủ phục hồi sau sự cố mất liên lạc.
- **FR-006**: Hệ thống BẮT BUỘC điều khiển 2 đèn LED chỉ báo trạng thái phần cứng độc lập: một đèn LED báo trạng thái WiFi (nhấp nháy khi đang kết nối, sáng liên tục khi đã kết nối thành công) và một đèn LED báo trạng thái truyền thông KEPServerEX (chớp nháy xác nhận mỗi lần gửi dữ liệu HTTP thành công, tắt hoặc báo lỗi khi truyền tin thất bại).
- **FR-007**: Hệ thống BẮT BUỘC xuất dữ liệu chẩn đoán hệ thống định kỳ và thông báo các sự kiện quan trọng (địa chỉ IP, cường độ sóng RSSI, giá trị cảm biến, kết quả truyền tin, cảnh báo sự cố) ra cổng nối tiếp (Serial Monitor) bằng ngôn ngữ Tiếng Việt rõ ràng.
- **FR-008**: Hệ thống BẮT BUỘC quy hoạch toàn bộ cấu hình phần cứng (chân GPIO cảm biến, chân GPIO LED), thông số mạng WiFi (SSID, mật khẩu), địa chỉ máy chủ KEPServerEX (IP, cổng dịch vụ) và các chu kỳ định thời (chu kỳ đọc cảm biến, chu kỳ gửi dữ liệu, chu kỳ kết nối lại) tập trung tại một tệp cấu hình duy nhất (`config.h`).
- **FR-009**: Hệ thống BẮT BUỘC thực thi toàn bộ logic định kỳ bằng kỹ thuật không chặn (non-blocking) sử dụng bộ đếm thời gian hệ thống, tuyệt đối không dùng các hàm dừng chờ khóa CPU trong vòng lặp chính nhằm đảm bảo khả năng đáp ứng thời gian thực và tránh kích hoạt Watchdog Timer.

### Key Entities

- **TelemetryData (Dữ liệu viễn trắc môi trường)**: Đại diện cho bản tin dữ liệu đo lường thu thập tại một thời điểm, bao gồm: nhiệt độ môi trường (°C), độ ẩm không khí (%RH), trạng thái hợp lệ của dữ liệu (Hợp lệ / Lỗi cảm biến), và chuỗi JSON đóng gói theo chuẩn KEPServerEX IoT Gateway (`[{"id":"<Tag_Temp>","v":<val>},{"id":"<Tag_Hum>","v":<val>}]`).
- **DeviceNetworkState (Trạng thái truyền thông mạng)**: Đại diện cho tình trạng kết nối mạng của thiết bị, bao gồm: trạng thái kết nối WiFi (Đang kết nối / Đã kết nối / Mất kết nối), địa chỉ IP được cấp, cường độ tín hiệu sóng WiFi (RSSI tính theo dBm), trạng thái liên lạc với máy chủ KEPServerEX (Trực tuyến / Ngoại tuyến / Đang thử kết nối lại).
- **SystemConfiguration (Cấu hình hệ thống)**: Chứa toàn bộ các tham số thiết lập cho thiết bị, bao gồm: phân bổ chân phần cứng (GPIO I2C SDA, GPIO I2C SCL, địa chỉ I2C SHT31-D, GPIO LED WiFi, GPIO LED KEPServerEX), cấu hình điểm truy cập WiFi (SSID, Password), cấu hình KEPServerEX REST Server (IP/Host máy chủ, Cổng HTTP, REST Endpoint URL), và các ngưỡng chu kỳ định thời (chu kỳ lấy mẫu, chu kỳ đẩy dữ liệu, khoảng thời gian thử kết nối lại).

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Dữ liệu nhiệt độ và độ ẩm được cập nhật chính xác lên phần mềm KEPServerEX theo đúng chu kỳ cấu hình (mặc định 5 giây) với độ trễ truyền dữ liệu dưới 1 giây trong môi trường mạng nội bộ ổn định.
- **SC-002**: Tỷ lệ truyền gói tin dữ liệu thành công đạt tối thiểu 99% trong quá trình vận hành liên tục 24 giờ trong điều kiện mạng bình thường.
- **SC-003**: Khi xảy ra sự cố mất tín hiệu mạng WiFi hoặc máy chủ KEPServerEX tạm ngưng hoạt động, thiết bị tự động phát hiện trong vòng dưới 10 giây và tự động khôi phục kết nối thành công trong vòng dưới 15 giây sau khi hạ tầng mạng/máy chủ hoạt động trở lại.
- **SC-004**: Thiết bị hoạt động ổn định liên tục, không xảy ra hiện tượng treo vi điều khiển (zero hang/freeze), không kích hoạt Watchdog Timer Reset và duy trì dung lượng bộ nhớ khả dụng ổn định (không rò rỉ bộ nhớ) sau 24 giờ thử nghiệm.
- **SC-005**: Kỹ thuật viên hiện trường có thể nhận biết chính xác trạng thái kết nối của thiết bị thông qua tín hiệu đèn LED chỉ thị trong vòng dưới 3 giây quan sát.
- **SC-006**: Toàn bộ thông điệp nhật ký chẩn đoán xuất ra cổng nối tiếp hiển thị bằng Tiếng Việt chuẩn mực, giúp kỹ sư phát hiện và xác định chính xác nguyên nhân lỗi trong vòng 30 giây khi có sự cố.

## Assumptions

- **Hạ tầng mạng WiFi**: Thiết bị hoạt động trong phạm vi phủ sóng của mạng WiFi băng tần 2.4GHz với cường độ tín hiệu đạt mức khả dụng (RSSI tối thiểu -75 dBm); máy chủ KEPServerEX có thể tiếp cận được từ mạng nội bộ này.
- **Cảm biến môi trường**: Sử dụng cảm biến nhiệt độ và độ ẩm Sensirion SHT31-D giao tiếp qua I2C (địa chỉ 0x44) được cấp nguồn 3.3V từ ESP32 và kết nối đúng chân SDA/SCL quy định trong tệp cấu hình `config.h`.
- **Phần mềm KEPServerEX**: Phần mềm KEPServerEX đã được cấu hình sẵn sàng với IoT Gateway REST Server Agent (lắng nghe các yêu cầu HTTP POST tại endpoint và cổng xác định, chế độ mở không yêu cầu xác thực - No Auth) để tiếp nhận dữ liệu thời gian thực từ thiết bị trong mạng nội bộ.
- **Nguồn cấp thiết bị**: Bo mạch ESP32 được cấp nguồn điện một chiều ổn định (5V qua cổng microUSB/Type-C hoặc nguồn công nghiệp 3.3V/5V có lọc nhiễu) đảm bảo đủ dòng điện khi kích hoạt truyền phát sóng vô tuyến WiFi.
