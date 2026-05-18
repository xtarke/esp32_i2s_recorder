/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */


/* C includes */
 #include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/unistd.h>
#include <sys/stat.h>

/* ESP includes */
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "format_wav.h"
#include "esp_log.h"

/* Project includes */
#include "wifi_sta.h"
#include "udp_client.h"

static const char *TAG = "main";

#define SPI_DMA_CHAN        SPI_DMA_CH_AUTO
#define NUM_CHANNELS        (2) // For mono recording only!
#define SD_MOUNT_POINT      "/sdcard"
#define SAMPLE_SIZE         (CONFIG_EXAMPLE_BIT_SAMPLE * 1024)
#define BYTE_RATE           (CONFIG_EXAMPLE_SAMPLE_RATE * (CONFIG_EXAMPLE_BIT_SAMPLE / 8)) * NUM_CHANNELS

// When testing SD and SPI modes, keep in mind that once the card has been
// initialized in SPI mode, it can not be reinitialized in SD mode without
// toggling power to the card.
sdmmc_host_t host = SDSPI_HOST_DEFAULT();
sdmmc_card_t *card;
i2s_chan_handle_t rx_handle = NULL;

static int32_t i2s_readraw_buff[SAMPLE_SIZE];

size_t bytes_read;
const int WAVE_HEADER_SIZE = 44;


void record_wav(uint32_t rec_time)
{
    // Use POSIX and C standard library functions to work with files.
    int flash_wr_size = 0;
    ESP_LOGI(TAG, "Opening file");

    uint32_t total_audio_bytes = CONFIG_EXAMPLE_SAMPLE_RATE * 4 * 2 * rec_time;

    //uint32_t flash_rec_time = BYTE_RATE * rec_time;
    
    uint32_t flash_rec_time = total_audio_bytes;
    
    const wav_header_t wav_header =
        WAV_HEADER_PCM_DEFAULT(total_audio_bytes, 32, CONFIG_EXAMPLE_SAMPLE_RATE, 2);

    //WAV_HEADER_PCM_DEFAULT (time, sample_bits, sample_rate, channels)

    // First check if file exists before creating a new file.
    // struct stat st;
    // if (stat(SD_MOUNT_POINT"/record.wav", &st) == 0) {
    //     // Delete it if it exists
    //     unlink(SD_MOUNT_POINT"/record.wav");
    // }

    // // Create new WAV file
    // FILE *f = fopen(SD_MOUNT_POINT"/record.wav", "a");
    // if (f == NULL) {
    //     ESP_LOGE(TAG, "Failed to open file for writing");
    //     return;
    // }

    // Write the header to the WAV file
    //fwrite(&wav_header, sizeof(wav_header), 1, f);
    udp_data_send((char *)&wav_header, sizeof(wav_header));

    int blocks = 0;
    
    // Start recording
    while (flash_wr_size < flash_rec_time) {
        // Read the RAW samples from the microphone
        if (i2s_channel_read(rx_handle, (char *)i2s_readraw_buff, SAMPLE_SIZE, &bytes_read, 1000) == ESP_OK) {
            
            udp_data_send((char *)&i2s_readraw_buff, bytes_read);

            flash_wr_size += bytes_read;

            printf("flash_wr_size %d %d\n", flash_wr_size, bytes_read);

            blocks++;
        } else {
            printf("Read Failed!\n");
        }
    }

    ESP_LOGI(TAG, "Recording done!");
    // fclose(f);
    ESP_LOGI(TAG, "File written on SDCard");

    // All done, unmount partition and disable SPI peripheral
    // esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, card);
    // ESP_LOGI(TAG, "Card unmounted");
    // Deinitialize the bus after all devices are removed
    // spi_bus_free(host.slot);
}

void setup_microphone(void)
{

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));

    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(CONFIG_EXAMPLE_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,    // some codecs may require mclk signal, this example doesn't need it
            .bclk = CONFIG_EXAMPLE_I2S_CLK_GPIO,
            .ws   = CONFIG_EXAMPLE_I2S_WS_GPIO,
            .dout = I2S_GPIO_UNUSED,
            .din  = CONFIG_EXAMPLE_I2S_DATA_GPIO, // In duplex mode, bind output and input to a same gpio can loopback internally
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };

    std_cfg.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT;
    std_cfg.slot_cfg.slot_mask      = I2S_STD_SLOT_BOTH;

//     i2s_pdm_rx_config_t pdm_rx_cfg = {
//         .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(CONFIG_EXAMPLE_SAMPLE_RATE),
//         /* The default mono slot is the left slot (whose 'select pin' of the PDM microphone is pulled down) */
// #if SOC_I2S_SUPPORTS_PDM2PCM
//         .slot_cfg = I2S_PDM_RX_SLOT_PCM_FMT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
// #else
//         .slot_cfg = I2S_PDM_RX_SLOT_RAW_FMT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
// #endif
//         .gpio_cfg = {
//             .clk = CONFIG_EXAMPLE_I2S_CLK_GPIO,
//             .din = CONFIG_EXAMPLE_I2S_DATA_GPIO,
//             .invert_flags = {
//                 .clk_inv = false,
//             },
//         },
//     };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
}

void app_main(void)
{
   
    ESP_LOGI(TAG, "UDP mic recorder");
    
    
    // Mount the SDCard for recording the audio file
    // mount_sdcard();
    // Acquire a I2S PDM channel for the PDM digital microphone
        
    /* Setup and connect WiFi STA mode with static credentials */
    setup_wifi_static_cred();

    /* Setup UDP client */
    setup_udp_client();
    
    
    setup_microphone();
    // ESP_LOGI(TAG, "Starting recording for %d seconds!", CONFIG_EXAMPLE_REC_TIME);
    
      
    
    // Start Recording
    record_wav(30);
    
    
    
    // // Stop I2S driver and destroy
    // ESP_ERROR_CHECK(i2s_channel_disable(rx_handle));
    // ESP_ERROR_CHECK(i2s_del_channel(rx_handle));
}
