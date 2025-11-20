/* include/Tuix/idt_list.h 保存idt x宏 */

#ifndef I_T_IDT_LIST_H
#define I_T_IDT_LIST_H

/*
    使用诸如这样:
    #define X(num) IDT_SET_GATE(num)
    IDT_LIST
    #undef X
*/

// 定义完整的256个IDT条目列表
// 在头文件中定义带名称的中断映射表
#define IDT_LIST_EXP \
    X(0,   divide_error)        \
    X(1,   debug_exception)     \
    X(2,   nmi)                 \
    X(3,   breakpoint)          \
    X(4,   overflow)            \
    X(5,   bounds_check)        \
    X(6,   invalid_opcode)      \
    X(7,   device_not_available) \
    X(8,   double_fault)        \
    X(9,   coprocessor_segment) \
    X(10,  invalid_tss)         \
    X(11,  segment_not_present) \
    X(12,  stack_segment_fault) \
    X(13,  general_protection_fault) \
    X(14,  page_fault)          \
    X(15,  reserved_15)         \
    X(16,  floating_point)      \
    X(17,  alignment_check)     \
    X(18,  machine_check)       \
    X(19,  simd_floating_point) \
    X(20,  virtualization)      \
    X(21,  control_protection)  \
    X(22,  reserved_22)         \
    X(23,  reserved_23)         \
    X(24,  reserved_24)         \
    X(25,  reserved_25)         \
    X(26,  reserved_26)         \
    X(27,  reserved_27)         \
    X(28,  hypervisor_injection) \
    X(29,  vmm_communication)   \
    X(30,  security_exception)  \
    X(31,  reserved_31)

#define IDT_LIST_PIC \
    X(32,  timer)               \
    X(33,  keyboard)            \
    X(34,  cascade)             \
    X(35,  com2)                \
    X(36,  com1)                \
    X(37,  lpt2)                \
    X(38,  floppy)              \
    X(39,  lpt1)                \
    X(40,  rtc)                 \
    X(41,  reserved_41)         \
    X(42,  reserved_42)         \
    X(43,  reserved_43)         \
    X(44,  ps2_mouse)           \
    X(45,  fpu)                 \
    X(46,  ata1)                \
    X(47,  ata2)

#define IDT_LIST_OTHER \
    X(48)  X(49)  X(50)  X(51)  X(52)  X(53)  X(54)  X(55)  \
    X(56)  X(57)  X(58)  X(59)  X(60)  X(61)  X(62)  X(63)  \
    X(64)  X(65)  X(66)  X(67)  X(68)  X(69)  X(70)  X(71)  \
    X(72)  X(73)  X(74)  X(75)  X(76)  X(77)  X(78)  X(79)  \
    X(80)  X(81)  X(82)  X(83)  X(84)  X(85)  X(86)  X(87)  \
    X(88)  X(89)  X(90)  X(91)  X(92)  X(93)  X(94)  X(95)  \
    X(96)  X(97)  X(98)  X(99)  X(100) X(101) X(102) X(103) \
    X(104) X(105) X(106) X(107) X(108) X(109) X(110) X(111) \
    X(112) X(113) X(114) X(115) X(116) X(117) X(118) X(119) \
    X(120) X(121) X(122) X(123) X(124) X(125) X(126) X(127) \
    X(128) X(129) X(130) X(131) X(132) X(133) X(134) X(135) \
    X(136) X(137) X(138) X(139) X(140) X(141) X(142) X(143) \
    X(144) X(145) X(146) X(147) X(148) X(149) X(150) X(151) \
    X(152) X(153) X(154) X(155) X(156) X(157) X(158) X(159) \
    X(160) X(161) X(162) X(163) X(164) X(165) X(166) X(167) \
    X(168) X(169) X(170) X(171) X(172) X(173) X(174) X(175) \
    X(176) X(177) X(178) X(179) X(180) X(181) X(182) X(183) \
    X(184) X(185) X(186) X(187) X(188) X(189) X(190) X(191) \
    X(192) X(193) X(194) X(195) X(196) X(197) X(198) X(199) \
    X(200) X(201) X(202) X(203) X(204) X(205) X(206) X(207) \
    X(208) X(209) X(210) X(211) X(212) X(213) X(214) X(215) \
    X(216) X(217) X(218) X(219) X(220) X(221) X(222) X(223) \
    X(224) X(225) X(226) X(227) X(228) X(229) X(230) X(231) \
    X(232) X(233) X(234) X(235) X(236) X(237) X(238) X(239) \
    X(240) X(241) X(242) X(243) X(244) X(245) X(246) X(247) \
    X(248) X(249) X(250) X(251) X(252) X(253) X(254) X(255)
#endif