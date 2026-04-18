// wav_writer.h
#pragma once

#include "app_config.h"
#include "file_counter.h"

// ← ИЗМЕНЕНО: буфер для 4 каналов
extern int16_t pcm_buffer_4ch[DMA_BUF_SIZE * 4];

esp_err_t create_wav(void);
esp_err_t write_wav_4ch(size_t samples_per_channel);  // ← ИЗМЕНЕНО
esp_err_t close_wav(uint32_t total_samples_per_channel);  // ← ИЗМЕЁЕНО
esp_err_t wav_deinit(void);
uint32_t get_current_file_number(void);
const char* get_current_filename(void);