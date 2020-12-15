#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <set>
#include <map>
#include <iomanip>
#include <string>
#include <cstdio>

using std::vector;
using std::string;
using std::set;
using std::pair;

// Структура, описывающая заголовок WAV файла.
struct WAVHEADER {
    // WAV-формат начинается с RIFF-заголовка:

    // Содержит символы "RIFF" в ASCII кодировке
    // (0x52494646 в big-endian представлении)
    char chunkId[4];

    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    unsigned int chunkSize;

    // Содержит символы "WAVE"
    // (0x57415645 в big-endian представлении)
    char format[4];

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:

    // Содержит символы "fmt "
    // (0x666d7420 в big-endian представлении)
    char subchunk1Id[4];

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    unsigned int subchunk1Size;

    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    unsigned short audioFormat;

    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned short numChannels;

    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned int sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    unsigned int byteRate;

    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    unsigned short blockAlign;

    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    unsigned short bitsPerSample;

    // Подцепочка "data" содержит аудио-данные и их размер.

    // Содержит символы "data"
    // (0x64617461 в big-endian представлении)
    char subchunk2Id[4];

    // numSamples * numChannels * bitsPerSample/8
    // Количество байт в области данных.
    unsigned int subchunk2Size;

    // Далее следуют непосредственно Wav данные.
};


void fft(std::vector<std::pair<double, double>>& vect, bool inv) {
    if (vect.size() == 1) { return; }
    const double PI = 3.14159265359;
    std::vector<std::pair<double, double>> temp1;
    std::vector<std::pair<double, double>> temp2;
    for (int i = 0, j = 0; i < static_cast<int>(vect.size() / 2); ++i, j += 2) {
        temp1.push_back(vect[j]);
        temp2.push_back(vect[j + 1]);
    }
    fft(temp1, inv);
    fft(temp2, inv);
    double temp;
    if (inv) { temp = 2 * PI * (-1) / vect.size(); }
    else { temp = 2 * PI / vect.size(); }
    std::pair<double, double> w;
    w.first = 1;
    w.second = 0;
    std::pair<double, double> wn;
    wn.first = cos(temp);
    wn.second = sin(temp);
    for (int i = 0; i < static_cast<int>(vect.size() / 2); ++i) {
        double complex_real_w = w.first;
        double complex_img_w = w.second;
        double complex_real_temp2 = temp2[i].first;
        double complex_img_temp2 = temp2[i].second;
        temp2[i].first = complex_real_temp2 * complex_real_w - complex_img_temp2 * complex_img_w;
        temp2[i].second = complex_real_temp2 * complex_img_w + complex_img_temp2 * complex_real_w;
        vect[i].first = temp1[i].first + temp2[i].first;
        vect[i].second = temp1[i].second + temp2[i].second;
        vect[i + vect.size() / 2].first = temp1[i].first - temp2[i].first;
        vect[i + vect.size() / 2].second = temp1[i].second - temp2[i].second;
        if (inv) {
            vect[i].first /= 2;
            vect[i].second /= 2;
            vect[i + vect.size() / 2].first /= 2;
            vect[i + vect.size() / 2].second /= 2;
        }
        complex_real_temp2 = wn.first;
        complex_img_temp2 = wn.second;
        w.first = complex_real_w * complex_real_temp2 - complex_img_w * complex_img_temp2;
        w.second = complex_real_w * complex_img_temp2 + complex_img_w * complex_real_temp2;
    }
}

int main() {
    FILE* file = fopen("speech.wav", "r");
    if (!file) {
        std::cout << "Failed open file";
        return 0;
    }

    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, file);

    // Выводим полученные данные
    std::cout << header.chunkId[0] << header.chunkId[1] << header.chunkId[2] << header.chunkId[3] << std::endl;
    printf("Chunk size: %d\n", header.chunkSize);
    std::cout << header.format[0] << header.format[1] << header.format[2] << header.format[3] << std::endl;
    std::cout << header.subchunk1Id[0] << header.subchunk1Id[1] << header.subchunk1Id[2] << header.subchunk1Id[3] << std::endl;
    printf("SubChunkId1: %d\n", header.subchunk1Size);
    printf("Audio format: %d\n", header.audioFormat);
    printf("Channels: %d\n", header.numChannels);
    printf("Sample rate: %d\n", header.sampleRate);
    printf("Bits per sample: %d\n", header.bitsPerSample);
    std::cout << header.subchunk2Id[0] << header.subchunk2Id[1] << header.subchunk2Id[2] << header.subchunk2Id[3] << std::endl;

    // Посчитаем длительность воспроизведения в секундах
    float fDurationSeconds = 1.f * header.subchunk2Size / (header.bitsPerSample / 8) / header.numChannels / header.sampleRate;
    int iDurationMinutes = (int)floor(fDurationSeconds) / 60;
    fDurationSeconds = fDurationSeconds - (iDurationMinutes * 60);
    printf("Duration: %02d:%02.f\n", iDurationMinutes, fDurationSeconds);

    fclose(file);
    file = fopen("speech.wav", "rb");

    char* data = new char[header.subchunk2Size];
    fread(data, header.subchunk2Size, 1, file);

    vector<pair<double, double>> temp;

    for (int i = 0; i < static_cast<int>(header.subchunk2Size); ++i) {
        temp.push_back(std::make_pair(data[i], 0));
    }

    unsigned int size_of_temp = 1;

    while (size_of_temp < header.subchunk2Size) {
        size_of_temp *= 2;
    }

    temp.resize(size_of_temp);

    fft(temp, false);

    for (int i = (temp.size() / 4); i < (3 * temp.size() / 4); ++i) {
        temp[i].first = 0;
        temp[i].second = 0;
    }

    fft(temp, true);

    WAVHEADER new_header = header;

    new_header.subchunk2Size = temp.size();

    char* new_data = new char[new_header.subchunk2Size];

    char p;

    for (int i = 0; i < static_cast<int>(temp.size()); ++i) {
        p = static_cast<char>(temp[i].first);
        new_data[i] = p;
    }

    FILE* new_file = fopen("ans.wav", "w");

    fwrite(&new_header, sizeof(new_header), 1, new_file);

    fwrite(new_data, 1, new_header.subchunk2Size, new_file);

    std::cout << "Data is successfully loaded." << std::endl;

    delete[] data;
    delete[] new_data;
    fclose(file);
    return 0;
}