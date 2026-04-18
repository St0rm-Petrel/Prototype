// wav_writer.c
#include "wav_writer.h"

static const char *TAG = "WAV";

// ← ИЗМЕНЕНО: буфер для 4 каналов
int16_t pcm_buffer_4ch[DMA_BUF_SIZE * 4];

// ... остальные статические функции без изменений ...

// ========== Запись 4-канальных данных ==========
esp_err_t write_wav_4ch(size_t samples_per_channel) {
    if (!file_opened || f == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    
    size_t total_samples = samples_per_channel * NUM_CHANNELS;
    size_t bytes_to_write = total_samples * sizeof(int16_t);
    size_t written = fwrite(pcm_buffer_4ch, 1, bytes_to_write, f);
    
    if (written != bytes_to_write) {
        ESP_LOGE(TAG, "Failed to write PCM data: wrote %d/%d bytes", written, bytes_to_write);
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

// ========== Закрытие WAV файла (с учётом 4 каналов) ==========
esp_err_t close_wav(uint32_t total_samples_per_channel) {
    if (!file_opened || f == NULL) {
        ESP_LOGW(TAG, "No open WAV file to close");
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Closing WAV file: %s", current_filename);
    
    if (total_samples_per_channel > 0) {
        // ← ИЗМЕНЕНО: учёт 4 каналов
        uint32_t data_size = total_samples_per_channel * NUM_CHANNELS * BYTES_PER_SAMPLE;
        uint32_t file_size = data_size + 36;
        
        // RIFF header
        wav_header[0] = 'R'; wav_header[1] = 'I'; wav_header[2] = 'F'; wav_header[3] = 'F';
        wav_header[4] = file_size & 0xFF;
        wav_header[5] = (file_size >> 8) & 0xFF;
        wav_header[6] = (file_size >> 16) & 0xFF;
        wav_header[7] = (file_size >> 24) & 0xFF;
        
        wav_header[8] = 'W'; wav_header[9] = 'A'; wav_header[10] = 'V'; wav_header[11] = 'E';
        
        // fmt subchunk
        wav_header[12] = 'f'; wav_header[13] = 'm'; wav_header[14] = 't'; wav_header[15] = ' ';
        wav_header[16] = 16; wav_header[17] = 0; wav_header[18] = 0; wav_header[19] = 0;
        wav_header[20] = 1; wav_header[21] = 0;
        wav_header[22] = NUM_CHANNELS & 0xFF;           // ← ИЗМЕНЕНО: 4 канала
        wav_header[23] = (NUM_CHANNELS >> 8) & 0xFF;
        wav_header[24] = SAMPLE_RATE & 0xFF;
        wav_header[25] = (SAMPLE_RATE >> 8) & 0xFF;
        wav_header[26] = (SAMPLE_RATE >> 16) & 0xFF;
        wav_header[27] = (SAMPLE_RATE >> 24) & 0xFF;
        wav_header[28] = BYTE_RATE & 0xFF;
        wav_header[29] = (BYTE_RATE >> 8) & 0xFF;
        wav_header[30] = (BYTE_RATE >> 16) & 0xFF;
        wav_header[31] = (BYTE_RATE >> 24) & 0xFF;
        wav_header[32] = (NUM_CHANNELS * BYTES_PER_SAMPLE) & 0xFF;
        wav_header[33] = ((NUM_CHANNELS * BYTES_PER_SAMPLE) >> 8) & 0xFF;
        wav_header[34] = 16; wav_header[35] = 0;
        
        // data subchunk
        wav_header[36] = 'd'; wav_header[37] = 'a'; wav_header[38] = 't'; wav_header[39] = 'a';
        wav_header[40] = data_size & 0xFF;
        wav_header[41] = (data_size >> 8) & 0xFF;
        wav_header[42] = (data_size >> 16) & 0xFF;
        wav_header[43] = (data_size >> 24) & 0xFF;
        
        fseek(f, 0, SEEK_SET);
        fwrite(wav_header, 1, 44, f);
        fflush(f);
        
        fclose(f);
        f = NULL;
        file_opened = false;
        
        if (counter_increment() != ESP_OK) {
            ESP_LOGW(TAG, "Failed to increment counter, but file saved");
        }
        
        ESP_LOGI(TAG, "File saved: %s", current_filename);
        ESP_LOGI(TAG, "Total samples per channel: %" PRIu32, total_samples_per_channel);
        ESP_LOGI(TAG, "Duration: %.2f seconds", (float)total_samples_per_channel / SAMPLE_RATE);
    } else {
        fclose(f);
        f = NULL;
        file_opened = false;
        ESP_LOGI(TAG, "No data recorded, deleting empty file");
        remove(current_filename);
        ESP_LOGI(TAG, "Empty file deleted");
    }
    
    return ESP_OK;
}

// ... остальные функции без изменений ...