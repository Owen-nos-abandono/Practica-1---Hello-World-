#include <stdio.h>          // Biblioteca estándar de C: entrada/salida (printf, fflush, etc.)
#include <inttypes.h>       // Define macros de formato portables para enteros (PRIu32, etc.)

#include "sdkconfig.h"      // Archivo de configuración generado por '[idf.py](http://idf.py/) menuconfig'
#include "freertos/FreeRTOS.h"  // Núcleo del sistema operativo FreeRTOS usado por ESP-IDF
#include "freertos/task.h"      // Funciones de control de tareas (vTaskDelay, etc.)
#include "esp_chip_info.h"      // Información sobre el chip (núcleos, características, revisión)
#include "esp_flash.h"          // Funciones para acceder a la memoria flash
#include "esp_system.h"         // Funciones generales del sistema (reinicio, heap, etc.)

/*

- Función principal de la aplicación.
- ESP-IDF no utiliza 'main()' como en programas estándar de C.
- En su lugar, crea una tarea principal que ejecuta 'app_main()' al iniciar.
*/
void app_main(void)
{
// --- Mensaje de bienvenida ---
printf("Hello world!\n");
    
    // --- Variables para almacenar información del chip y de la memoria flash ---
    esp_chip_info_t chip_info;  // Estructura que guarda la información del chip
    uint32_t flash_size;        // Variable para almacenar el tamaño de la memoria flash (en bytes)
    
    // --- Obtener la información del chip actual ---
    esp_chip_info(&chip_info);  // Rellena la estructura 'chip_info' con datos del hardware
    
    // --- Imprimir información general del chip ---
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
    CONFIG_IDF_TARGET,                          
    chip_info.cores,                            
    (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",   
    (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",              
    (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",             
    (chip_info.features & CHIP_FEATURE_IEEE802154)                   
    ? ", 802.15.4 (Zigbee/Thread)" : "");
    
    // --- Calcular versión del silicio ---
    // El campo 'revision' combina versión mayor y menor en un solo entero (ej. 101 = v1.1)
    unsigned major_rev = chip_info.revision / 100;   
    unsigned minor_rev = chip_info.revision % 100;   
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    
    // --- Obtener tamaño de la memoria flash ---
    // 'esp_flash_get_size' devuelve ESP_OK si la lectura fue correcta.
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
    printf("Get flash size failed\n");  
    return;                             
    }
    
    // --- Imprimir tamaño y tipo de flash ---
    printf("%" PRIu32 "MB %s flash\n",
    flash_size / (uint32_t)(1024 * 1024),       
    (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    // Indica si la flash es integrada o externa al chip
    
    // --- Mostrar la cantidad mínima de memoria heap libre ---
    printf("Minimum free heap size: %" PRIu32 " bytes\n",
    esp_get_minimum_free_heap_size());
    // Esta función devuelve la cantidad mínima de heap libre observada desde el arranque
    
    // --- Bucle de cuenta regresiva antes del reinicio ---
    for (int i = 10; i >= 0; i--) {
    printf("Restarting in %d seconds...\n", i);          // Muestra segundos restantes
    vTaskDelay(1000 / portTICK_PERIOD_MS);               // Espera 1 segundo (1000 ms)
    // Nota: 'vTaskDelay' suspende esta tarea durante el número indicado de ticks.
    //       'portTICK_PERIOD_MS' convierte milisegundos en ticks según la frecuencia de FreeRTOS.
    }
    
    // --- Reinicio del chip ---
    printf("Restarting now.\n");  // Mensaje final antes del reinicio
    fflush(stdout);               // Fuerza el envío inmediato del buffer de salida (por UART)
    esp_restart();                // Reinicia el SoC (software reset)
    // Nota: 'esp_restart()' no retorna; el sistema se reinicia y se ejecuta el bootloader nuevamente.
    }
