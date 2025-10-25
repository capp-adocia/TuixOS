/* include/Hydrangea/pic.h - 8259A PIC操作 */

#ifndef I_PIC_H
#define I_PIC_H

void init_pic(void);
void disable_pic(void);
void enable_irq(uint8_t irq);
void disable_irq(uint8_t irq);
void send_eoi(uint8_t irq);

#endif