# Monitoreo_DO_CtrlMotor_SendDataHTTP
 This files are a routine for a Tilapia monitoring system. It uses a Dissolved Oxigen, DC Motor controlled by a water level sensor, and it sends data to Thingspeak to test HTTP bus.

 I'm trying to use RTOS library to memory management, but i have some issues:

 1. "ESP_UNIDAS" FILE IS NOT WORKING WELL. 
 When I try to make it works, that error from heap memory appears, and I can't solve it, I try to use some Heap Management from IDF EXPRESSIF oficial page https://my-esp-idf.readthedocs.io/en/latest/api-reference/system/mem_alloc.html So, that was the reason why I'm using two ESP32 instead of one.
