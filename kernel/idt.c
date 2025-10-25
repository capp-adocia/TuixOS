/* kernel/idt.c */

#include <Hydrangea/idt.h>
#define IDT_SET_GATE(num) idt_set_gate(num, (uint32_t)isr_##num, 0x08, 0x8E);

struct idt_entry idt[IDT_ITEM_NUM];
interrupt_handler_t interrupt_handlers[IDT_ITEM_NUM];

struct idt_ptr idtp;

static inline void idt_load(uint32_t idt_ptr) {
    __asm__ volatile("lidt (%0)" : : "r"(idt_ptr));
}
static void idt_set(void);
static void register_interrupt_handlers(void);

void init_idt(void)
{
    idtp.limit = sizeof(struct idt_entry) * IDT_ITEM_NUM - 1;
    idtp.base = (uint32_t)&idt;
    // 设置桩函数
    idt_set();
    // 设置C中断处理函数
    register_interrupt_handlers();
    // 加载IDT
    idt_load((uint32_t)&idtp);
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// 传结构体指针时，逆序push结构体的字段!!!
void isr_common(void) {
    __asm__ volatile(
        "pusha\n\t" // 压入 edi,esi,ebp,esp,ebx,edx,ecx,eax
        "pushl %%ds\n\t" // 压入 ds
        "pushl %%es\n\t" // 压入 es
        
        "movw $0x10, %%ax\n\t"
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        
        "pushl %%esp\n\t" // 压入 当前的ESP值（指向第一个字段es的位置）
        "call isr_handler\n\t"
        "addl $4, %%esp\n\t"
        
        "popl %%es\n\t"
        "popl %%ds\n\t"
        "popa\n\t"
        "addl $8, %%esp\n\t"
        "iret\n\t"
        : : : "memory"
    );
}

void isr_handler(struct interrupt_frame* frame)
{    
    if(interrupt_handlers[frame->int_no])
        interrupt_handlers[frame->int_no](frame);
    else isr_default(frame); // 默认处理
}

static void register_interrupt_handlers(void)
{
    // 注册前32个异常处理函数
    register_interrupt_handler(0, isr_0_c);
    register_interrupt_handler(1, isr_1_c);
    register_interrupt_handler(2, isr_2_c);
    register_interrupt_handler(3, isr_3_c);
    register_interrupt_handler(4, isr_4_c);
    register_interrupt_handler(5, isr_5_c);
    register_interrupt_handler(6, isr_6_c);
    register_interrupt_handler(7, isr_7_c);
    register_interrupt_handler(8, isr_8_c);
    register_interrupt_handler(9, isr_9_c);
    register_interrupt_handler(10, isr_10_c);
    register_interrupt_handler(11, isr_11_c);
    register_interrupt_handler(12, isr_12_c);
    register_interrupt_handler(13, isr_13_c);
    register_interrupt_handler(14, isr_14_c);
    register_interrupt_handler(15, isr_15_c);
    register_interrupt_handler(16, isr_16_c);
    register_interrupt_handler(17, isr_17_c);
    register_interrupt_handler(18, isr_18_c);
    register_interrupt_handler(19, isr_19_c);
    register_interrupt_handler(20, isr_20_c);
    register_interrupt_handler(21, isr_21_c);
    register_interrupt_handler(22, isr_22_c);
    register_interrupt_handler(23, isr_23_c);
    register_interrupt_handler(24, isr_24_c);
    register_interrupt_handler(25, isr_25_c);
    register_interrupt_handler(26, isr_26_c);
    register_interrupt_handler(27, isr_27_c);
    register_interrupt_handler(28, isr_28_c);
    register_interrupt_handler(29, isr_29_c);
    register_interrupt_handler(30, isr_30_c);
    register_interrupt_handler(31, isr_31_c);
}

static void idt_set(void)
{
    // 0-31: CPU 异常
    IDT_SET_GATE(0)  IDT_SET_GATE(1)  IDT_SET_GATE(2)  IDT_SET_GATE(3)
    IDT_SET_GATE(4)  IDT_SET_GATE(5)  IDT_SET_GATE(6)  IDT_SET_GATE(7)
    IDT_SET_GATE(8)  IDT_SET_GATE(9)  IDT_SET_GATE(10) IDT_SET_GATE(11)
    IDT_SET_GATE(12) IDT_SET_GATE(13) IDT_SET_GATE(14) IDT_SET_GATE(15)
    IDT_SET_GATE(16) IDT_SET_GATE(17) IDT_SET_GATE(18) IDT_SET_GATE(19)

    
    IDT_SET_GATE(20) IDT_SET_GATE(21) IDT_SET_GATE(22) IDT_SET_GATE(23)
    IDT_SET_GATE(24) IDT_SET_GATE(25) IDT_SET_GATE(26) IDT_SET_GATE(27)
    IDT_SET_GATE(28) IDT_SET_GATE(29) IDT_SET_GATE(30) IDT_SET_GATE(31)

    IDT_SET_GATE(60) IDT_SET_GATE(61) IDT_SET_GATE(62) IDT_SET_GATE(63)
    IDT_SET_GATE(64) IDT_SET_GATE(65) IDT_SET_GATE(66) IDT_SET_GATE(67)
    IDT_SET_GATE(68) IDT_SET_GATE(69) IDT_SET_GATE(70) IDT_SET_GATE(71)
    IDT_SET_GATE(72) IDT_SET_GATE(73) IDT_SET_GATE(74) IDT_SET_GATE(75)
    IDT_SET_GATE(76) IDT_SET_GATE(77) IDT_SET_GATE(78) IDT_SET_GATE(79)
    IDT_SET_GATE(80) IDT_SET_GATE(81) IDT_SET_GATE(82) IDT_SET_GATE(83)
    IDT_SET_GATE(84) IDT_SET_GATE(85) IDT_SET_GATE(86) IDT_SET_GATE(87)
    IDT_SET_GATE(88) IDT_SET_GATE(89) IDT_SET_GATE(90) IDT_SET_GATE(91)
    IDT_SET_GATE(92) IDT_SET_GATE(93) IDT_SET_GATE(94) IDT_SET_GATE(95)
    IDT_SET_GATE(96) IDT_SET_GATE(97) IDT_SET_GATE(98) IDT_SET_GATE(99)
    IDT_SET_GATE(100) IDT_SET_GATE(101) IDT_SET_GATE(102) IDT_SET_GATE(103)
    IDT_SET_GATE(104) IDT_SET_GATE(105) IDT_SET_GATE(106) IDT_SET_GATE(107)
    IDT_SET_GATE(108) IDT_SET_GATE(109) IDT_SET_GATE(110) IDT_SET_GATE(111)
    IDT_SET_GATE(112) IDT_SET_GATE(113) IDT_SET_GATE(114) IDT_SET_GATE(115)
    IDT_SET_GATE(116) IDT_SET_GATE(117) IDT_SET_GATE(118) IDT_SET_GATE(119)
    IDT_SET_GATE(120) IDT_SET_GATE(121) IDT_SET_GATE(122) IDT_SET_GATE(123)
    IDT_SET_GATE(124) IDT_SET_GATE(125) IDT_SET_GATE(126) IDT_SET_GATE(127)
    IDT_SET_GATE(128) IDT_SET_GATE(129) IDT_SET_GATE(130) IDT_SET_GATE(131)
    IDT_SET_GATE(132) IDT_SET_GATE(133) IDT_SET_GATE(134) IDT_SET_GATE(135)
    IDT_SET_GATE(136) IDT_SET_GATE(137) IDT_SET_GATE(138) IDT_SET_GATE(139)
    IDT_SET_GATE(140) IDT_SET_GATE(141) IDT_SET_GATE(142) IDT_SET_GATE(143)
    IDT_SET_GATE(144) IDT_SET_GATE(145) IDT_SET_GATE(146) IDT_SET_GATE(147)
    IDT_SET_GATE(148) IDT_SET_GATE(149) IDT_SET_GATE(150) IDT_SET_GATE(151)
    IDT_SET_GATE(152) IDT_SET_GATE(153) IDT_SET_GATE(154) IDT_SET_GATE(155)
    IDT_SET_GATE(156) IDT_SET_GATE(157) IDT_SET_GATE(158) IDT_SET_GATE(159)
    IDT_SET_GATE(160) IDT_SET_GATE(161) IDT_SET_GATE(162) IDT_SET_GATE(163)
    IDT_SET_GATE(164) IDT_SET_GATE(165) IDT_SET_GATE(166) IDT_SET_GATE(167)
    IDT_SET_GATE(168) IDT_SET_GATE(169) IDT_SET_GATE(170) IDT_SET_GATE(171)
    IDT_SET_GATE(172) IDT_SET_GATE(173) IDT_SET_GATE(174) IDT_SET_GATE(175)
    IDT_SET_GATE(176) IDT_SET_GATE(177) IDT_SET_GATE(178) IDT_SET_GATE(179)
    IDT_SET_GATE(180) IDT_SET_GATE(181) IDT_SET_GATE(182) IDT_SET_GATE(183)
    IDT_SET_GATE(184) IDT_SET_GATE(185) IDT_SET_GATE(186) IDT_SET_GATE(187)
    IDT_SET_GATE(188) IDT_SET_GATE(189) IDT_SET_GATE(190) IDT_SET_GATE(191)
    IDT_SET_GATE(192) IDT_SET_GATE(193) IDT_SET_GATE(194) IDT_SET_GATE(195)
    IDT_SET_GATE(196) IDT_SET_GATE(197) IDT_SET_GATE(198) IDT_SET_GATE(199)
    IDT_SET_GATE(200) IDT_SET_GATE(201) IDT_SET_GATE(202) IDT_SET_GATE(203)
    IDT_SET_GATE(204) IDT_SET_GATE(205) IDT_SET_GATE(206) IDT_SET_GATE(207)
    IDT_SET_GATE(208) IDT_SET_GATE(209) IDT_SET_GATE(210) IDT_SET_GATE(211)
    IDT_SET_GATE(212) IDT_SET_GATE(213) IDT_SET_GATE(214) IDT_SET_GATE(215)
    IDT_SET_GATE(216) IDT_SET_GATE(217) IDT_SET_GATE(218) IDT_SET_GATE(219)
    IDT_SET_GATE(220) IDT_SET_GATE(221) IDT_SET_GATE(222) IDT_SET_GATE(223)
    IDT_SET_GATE(224) IDT_SET_GATE(225) IDT_SET_GATE(226) IDT_SET_GATE(227)
    IDT_SET_GATE(228) IDT_SET_GATE(229) IDT_SET_GATE(230) IDT_SET_GATE(231)
    IDT_SET_GATE(232) IDT_SET_GATE(233) IDT_SET_GATE(234) IDT_SET_GATE(235)
    IDT_SET_GATE(236) IDT_SET_GATE(237) IDT_SET_GATE(238) IDT_SET_GATE(239)
    IDT_SET_GATE(240) IDT_SET_GATE(241) IDT_SET_GATE(242) IDT_SET_GATE(243)
    IDT_SET_GATE(244) IDT_SET_GATE(245) IDT_SET_GATE(246) IDT_SET_GATE(247)
    IDT_SET_GATE(248) IDT_SET_GATE(249) IDT_SET_GATE(250) IDT_SET_GATE(251)
    IDT_SET_GATE(252) IDT_SET_GATE(253) IDT_SET_GATE(254) IDT_SET_GATE(255)

}