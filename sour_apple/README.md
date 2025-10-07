
# SourApple

Note : Code do RapierXbox (https://github.com/RapierXbox) phát triển.


Code được lấy từ "https://github.com/RapierXbox/ESP32-Sour-Apple" và đã có sửa đổi dòng 41
```c
    randomAdvertisementData.addData(std::string((char *)packet, 17));

```
thành dòng 44

``` C
    randomAdvertisementData.addData(packet, 17);

```
để có thể chạy được trên các bộ biên dịch mới hơn. Đồng thời cũng sử dụng nút bấm BOOT trên GPIO0 để tránh spam không kiểm soát 