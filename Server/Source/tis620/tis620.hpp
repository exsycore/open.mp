#pragma once

#include <cstring>
#include <cstdlib>
#include "../fixcolour/fixce.hpp"

/**
 * @brief ตารางสำหรับแปลง Unicode ภาษาไทย (0x0E00-0x0E7F) เป็นรหัส TIS-620
 *
 * ใช้ภายใน `unicode_to_tis620()` เพื่อแมป Unicode เป็น TIS-620 โดยตรง
 */
extern const unsigned char thai_unicode_to_tis620[128];

class tis620
{
    public:
        /**
         * @brief แปลง UTF-8 เป็น Unicode codepoint (U+xxxx)
         *
         * รองรับ UTF-8 ยาว 1-4 ไบต์ และตรวจสอบความถูกต้องของลำดับ byte
         *
         * @param utf8 พอยน์เตอร์ไปยังข้อมูล UTF-8
         * @param len ความยาวข้อมูลที่เหลือใน buffer
         * @param codepoint ตัวแปรสำหรับรับค่ารหัส Unicode
         * @return จำนวน byte ที่ถูกใช้ในลำดับ UTF-8 นี้, หรือ -1 หากข้อมูลไม่ถูกต้อง
         */
        static int decode_utf8(const unsigned char* utf8, int len, unsigned int* codepoint);

        /**
         * @brief แปลง Unicode codepoint ให้เป็น TIS-620
         *
         * รองรับตัวอักษร ASCII (0x00-0x7F) และช่วง Unicode ภาษาไทย (0x0E00-0x0E7F)
         *
         * @param codepoint รหัส Unicode ที่ต้องการแปลง
         * @return ค่า TIS-620 หากสามารถแปลงได้, มิฉะนั้นคืน '?'
         */
        static unsigned char unicode_to_tis620(unsigned int codepoint);

        /**
         * @brief แปลงข้อความจาก UTF-8 เป็น TIS-620 และปรับสีข้อความ
         *
         * ฟังก์ชันนี้แปลงข้อความ UTF-8 เป็นรหัส TIS-620 พร้อมกับเรียกใช้ `Fixcolour::fixcolour_msg`
         * เพื่อจัดการสีข้อความ (เช่น เปลี่ยนตำแหน่ง tag สีให้แสดงผลภาษาไทยได้ถูกต้อง)
         *
         * @param utf8_input ข้อความต้นฉบับในรูปแบบ UTF-8
         * @param tis620_output บัฟเฟอร์สำหรับเก็บผลลัพธ์ที่เป็น TIS-620
         * @param output_size ขนาดของบัฟเฟอร์ปลายทาง
         * @return จำนวน byte ที่ถูกแปลงแล้ว, หรือ -1 หากเกิดข้อผิดพลาด
         */
        static int utf8_to_tis620(const char* utf8_input, char* tis620_output, int output_size);

        /**
         * @brief ตรวจสอบว่า string ที่กำหนดเป็น UTF-8 ที่ถูกต้องหรือไม่
         *
         * ใช้สำหรับตรวจสอบว่า input นั้นเป็นลำดับ byte UTF-8 ที่ไม่มีข้อผิดพลาด
         *
         * @param str ข้อความที่ต้องการตรวจสอบ
         * @return true หากข้อความเป็น UTF-8 ที่ถูกต้อง, false หากไม่ใช่
         */
        static bool is_valid_utf8(const char* str);
};
