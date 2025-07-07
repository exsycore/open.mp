#pragma once // ป้องกันการ include ซ้ำ

#include <cstdio>
#include <cstring>
#include <cstdlib>

// ==============================
// Fixcolour - Utility class
// ใช้จัดการตำแหน่ง Tag สี เช่น {FFFFFF} ให้ไม่เพี้ยนเมื่อแสดงผลภาษาไทย
// ==============================

class Fixcolour
{
    private:
        // กลุ่มตัวอักษรพิเศษที่ใช้ในระบบจัดการข้อความ (กำหนดไว้ใน fixce.cpp)

        unsigned char igor_spaces[4] = { ' ', '\r', '\n', '\t' };
        unsigned char spaces[3] = { ' ', '\r', '\t' };
        unsigned char vowel[17] = { 0xd1, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xd3 };
        unsigned char bugSaraI[3] = { '\n', '\r', '\t' };
        unsigned char SaraAir[4] = { 0xD4, 0xD5, 0xD6, 0xD7 };

        /* 
            unsigned char igor_spaces[4];   // ช่องว่างทุกชนิด รวมถึง newline/tab
            unsigned char spaces[3];        // ช่องว่างทั่วไป ยกเว้น newline
            unsigned char vowel[17];        // ตัวอักษรสระ/วรรณยุกต์ที่ต้องพิจารณาตำแหน่ง
            unsigned char bugSaraI[3];      // กลุ่มที่อาจเกิดบั๊กกับสระอิ
            unsigned char SaraAir[4];       // สระลอยพิเศษ (ex. สระอา ฯลฯ)
        */

        char* subc(const char* input, int offset, int len, char* dest);
        int shiftLeft(char text[], char color[], int Pos, int shiftBy);
        int shiftRight(char text[], char color[], int Pos, int shiftBy);

    public:
        /**
         * @brief จัดตำแหน่งของ color tag สำหรับข้อความทั่วไป เช่น sendClientMessage
         *
         * @param input   ข้อความต้นฉบับ (TIS-620)
         * @param output  ข้อความหลังปรับตำแหน่งสีแล้ว (จะถูกแก้ไขในฟังก์ชัน)
         */
        static void fixcolour_msg(const char* input, char* output);

        /**
         * @brief จัดตำแหน่งของ color tag สำหรับข้อความใน Dialog
         *
         * @param input        ข้อความต้นฉบับ (TIS-620)
         * @param output       ข้อความหลังปรับตำแหน่งสีแล้ว
         * @param dialogStyle  ประเภทของ Dialog (เช่น Style_MSGBOX, Style_TABLIST, ...)
         */
        static void fixcolour_dialog(const char* input, char* output, int dialogStyle);
};
