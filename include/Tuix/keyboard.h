/* include/Tuix/keyboard.h 处理键盘中断 */

#ifndef I_T_KEYBOARD_H
#define I_T_KEYBOARD_H

#include <stddef.h>
#include <Tuix/trap_frame.h>

/**
 * 扫描码到字符的映射
 * @param scancode 扫描码
 */
char scancode_to_char(uint8_t scancode);

/**
 * 中断处理函数
 * @param frame 中断帧数据
 */
void keyboard_handler(struct trap_frame* frame);

#endif
