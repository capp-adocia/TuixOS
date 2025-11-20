/* include/Tuix/keyboard.h 处理键盘中断 */

#ifndef I_T_KEYBOARD_H
#define I_T_KEYBOARD_H

#include <stddef.h>
#include <Tuix/interrupts_types.h>

/**
 * 扫描码到字符的映射
 * @param scancode 扫描码
 */
char scancode_to_char(uint8_t scancode);

/**
 * 中断处理函数
 * @param frame interrupt_frame结构体指针
 */
void keyboard_handler(struct interrupt_frame* frame);

#endif