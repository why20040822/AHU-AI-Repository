#include "reg51.h"
#include <string.h>

#define uchar unsigned char
#define uint unsigned int 
// T6963C 命令定义
#define LC_CUR_POS 0x21 // 光标位置设置	
#define LC_CGR_POS 0x22 // CGRAM 偏置地址设置
#define LC_ADD_POS 0x24 // 地址指针位置
#define LC_TXT_STP 0x40 // 文本区首址
#define LC_TXT_WID 0x41 // 文本区宽度
#define LC_GRH_STP 0x42 // 图形区首址
#define LC_GRH_WID 0x43 // 图形区宽度
#define LC_MOD_OR 0x80 // OR
#define LC_MOD_XOR 0x81 // XOR
#define LC_MOD_AND 0x82 // AND
#define LC_MOD_TCH 0x83 // 文本特征
#define LC_DIS_SW 0x90 // 显示开关
                       // D0=1/0：光标闪烁启用/禁用
                       // D1=1/0：光标显示
                       // D2=1/0：文本显示
                       // D3=1/0：图形显示
#define LC_CUR_SHP 0xA0 // 光标形状选择，0xA0~0xA7表示光标占的行数
#define LC_AUT_WR 0xB0 // 自动写设置
#define LC_AUT_RD 0xB1 // 自动读设置
#define LC_AUT_OVR 0xB2 // 自动读/写结束
#define LC_INC_WR 0xC0 // 数据一次写地址加1
#define LC_INC_RD 0xC1 // 数据一次读地址加1
#define LC_DEC_WR 0xC2 // 数据一次写地址减1
#define LC_DEC_RD 0xC3 // 数据一次读地址减1
#define LC_NOC_WR 0xC4 // 数据一次写地址不变
#define LC_NOC_RD 0xC5 // 数据一次读地址不变
#define LC_SCN_RD 0xE0 // 屏读
#define LC_SCN_CP 0xE8 // 屏拷贝
#define LC_BIT_OP 0xF0 // 位操作
// 液晶屏定义
#define Gus_LCM_XMAX 0x10 // 屏幕最大高度 128/8
#define Gus_LCM_YMAX 0x14 // 屏幕最大宽度 160/8

/****接口定义****/
sbit LCD_CD = P2^0;
sbit LCD_CE = P2^1;
sbit LCD_WR = P2^2;
sbit LCD_RD = P2^3;
#define DataPort P1
/****
字符字模表
字符库：横向取模，数据排列：从上到下
****/
char code ASC_MSK[96][16] = {
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*-- --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18, /*-- ! --*/
0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x66,0x66,0x66,0x00,0x00, /*-- " --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x36,0x36,0x7F,0x36,0x36, /*-- # --*/
0x36,0x7F,0x36,0x36,0x00,0x00,0x00,0x00},
{0x00,0x18,0x18,0x3C,0x66,0x60,0x30,0x18, /*-- $ --*/
0x0C,0x06,0x66,0x3C,0x18,0x18,0x00,0x00},
{0x00,0x00,0x70,0xD8,0xDA,0x76,0x0C,0x18, /*-- % --*/
0x30,0x6E,0x5B,0x1B,0x0E,0x00,0x00,0x00},
{0x00,0x00,0x00,0x38,0x6C,0x6C,0x38,0x60, /*-- & --*/
0x6F,0x66,0x66,0x3B,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x18,0x18,0x18,0x00,0x00, /*-- ' --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x0C,0x18,0x18,0x30,0x30, /*-- ( --*/
0x30,0x30,0x30,0x18,0x18,0x0C,0x00,0x00},
{0x00,0x00,0x00,0x30,0x18,0x18,0x0C,0x0C, /*-- ) --*/
0x0C,0x0C,0x0C,0x18,0x18,0x30,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x36,0x1C,0x7F, /*-- * --*/
0x1C,0x36,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7E, /*-- + --*/
0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*-- , --*/
0x00,0x00,0x1C,0x1C,0x0C,0x18,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E, /*-- - --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*-- . --*/
0x00,0x00,0x1C,0x1C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x06,0x06,0x0C,0x0C,0x18, /*-- / --*/
0x18,0x30,0x30,0x60,0x60,0x00,0x00,0x00},
{0x00,0x00,0x00,0x1E,0x33,0x37,0x37,0x33, /*-- 0 --*/
0x3B,0x3B,0x33,0x1E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x0C,0x1C,0x7C,0x0C,0x0C, /*-- 1 --*/
0x0C,0x0C,0x0C,0x0C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x0C, /*-- 2 --*/
0x18,0x30,0x60,0x7E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x1C, /*-- 3 --*/
0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x30,0x30,0x36,0x36,0x36, /*-- 4 --*/
0x66,0x7F,0x06,0x06,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C, /*-- 5 --*/
0x06,0x06,0x0C,0x78,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x1C,0x18,0x30,0x7C,0x66, /*-- 6 --*/
0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7E,0x06,0x0C,0x0C,0x18, /*-- 7 --*/
0x18,0x30,0x30,0x30,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x76,0x3C, /*-- 8 --*/
0x6E,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66, /*-- 9 --*/
0x3E,0x0C,0x18,0x38,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x00, /*-- : --*/
0x00,0x00,0x1C,0x1C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x00, /*-- ; --*/
0x00,0x00,0x1C,0x1C,0x0C,0x18,0x00,0x00},
{0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60, /*-- < --*/
0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00, /*-- = --*/
0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06, /*-- > --*/
0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x0C,0x18, /*-- ? --*/
0x18,0x00,0x18,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7E,0xC3,0xC3,0xCF,0xDB, /*-- @ --*/
0xDB,0xCF,0xC0,0x7F,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x18,0x3C,0x66,0x66,0x66, /*-- A --*/
0x7E,0x66,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x7C, /*-- B --*/
0x66,0x66,0x66,0x7C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x60,0x60, /*-- C --*/
0x60,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x78,0x6C,0x66,0x66,0x66, /*-- D --*/
0x66,0x66,0x6C,0x78,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C, /*-- E --*/
0x60,0x60,0x60,0x7E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C, /*-- F --*/
0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x60,0x60, /*-- G --*/
0x6E,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x7E, /*-- H --*/
0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x18,0x18,0x18,0x18, /*-- I --*/
0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x06,0x06,0x06,0x06,0x06, /*-- J --*/
0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x66,0x66,0x6C,0x6C,0x78, /*-- K --*/
0x6C,0x6C,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60, /*-- L --*/
0x60,0x60,0x60,0x7E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x63,0x63,0x77,0x6B,0x6B, /*-- M --*/
0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x63,0x63,0x73,0x7B,0x6F, /*-- N --*/
0x67,0x63,0x63,0x63,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3c,0x66,0x66,0x66,0x66, /*-- O --*/
0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x7C, /*-- P --*/
0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66, /*-- Q --*/
0x66,0x66,0x66,0x3C,0x0c,0x06,0x00,0x00},
{0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x7C, /*-- R --*/
0x6c,0x66,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3c,0x66,0x60,0x30,0x18, /*-- S --*/
0x0c,0x06,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x7E,0x18,0x18,0x18,0x18, /*-- T --*/
0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66, /*-- U --*/
0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66, /*-- V --*/
0x66,0x66,0x3c,0x18,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B, /*-- W --*/
0x6B,0x36,0x36,0x36,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x66,0x66,0x34,0x18,0x18, /*-- X --*/
0x2c,0x66,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x3c, /*-- Y --*/
0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00}, 
{0x00,0x00,0x00,0x7E,0x06,0x06,0x0c,0x18, /*-- Z --*/
0x30,0x60,0x60,0x7E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x30,0x30,0x30,0x30, /*-- [ --*/
0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x3C},
{0x00,0x00,0x00,0x60,0x60,0x30,0x30,0x18, /*-- \ --*/
0x18,0x0c,0x0c,0x06,0x06,0x00,0x00,0x00},
{0x00,0x00,0x00,0x3C,0x0c,0x0c,0x0c,0x0c, /*-- ] --*/
0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3c,0x3c},
{0x00,0x18,0x3c,0x66,0x00,0x00,0x00,0x00, /*-- ^ --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*-- _ --*/
0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF},
{0x00,0x38,0x18,0x0c,0x00,0x00,0x00,0x00, /*-- ` --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x3c,0x06,0x06, /*-- a --*/
0x3E,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x60,0x60,0x7c,0x66,0x66, /*-- b --*/
0x66,0x66,0x66,0x7c,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x60, /*-- c --*/
0x60,0x60,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x06,0x06,0x3E,0x66,0x66, /*-- d --*/
0x66,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x66, /*-- e --*/
0x7E,0x60,0x60,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x1E,0x30,0x30,0x30,0x7E, /*-- f --*/
0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x3E,0x66,0x66, /*-- g --*/
0x66,0x66,0x66,0x3E,0x06,0x06,0x7C,0x7C},
{0x00,0x00,0x00,0x60,0x60,0x7c,0x66,0x66, /*-- h --*/
0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x18,0x18,0x00,0x78,0x18,0x18, /*-- i --*/
0x18,0x18,0x18,0x7E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x0c,0x0c,0x00,0x3C,0x0c,0x0c, /*-- j --*/
0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x78,0x78},
{0x00,0x00,0x00,0x60,0x60,0x66,0x66,0x6C, /*-- k --*/
0x78,0x6C,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x78,0x18,0x18,0x18,0x18, /*-- 1 --*/
0x18,0x18,0x18,0x7E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x7E,0x6B,0x6B, /*-- m --*/
0x6B,0x6B,0x6B,0x63,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x7c,0x66,0x66, /*-- n --*/
0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00}, 
{0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x66, /*-- o --*/
0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x7c,0x66,0x66, /*-- p --*/
0x66,0x66,0x66,0x7c,0x60,0x60,0x60,0x60},
{0x00,0x00,0x00,0x00,0x00,0x3E,0x66,0x66, /*-- q --*/
0x66,0x66,0x66,0x3E,0x06,0x06,0x06,0x06},
{0x00,0x00,0x00,0x00,0x00,0x66,0x6E,0x70, /*-- r --*/
0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x3E,0x60,0x60, /*-- s --*/
0x3c,0x06,0x06,0x7c,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x30,0x30,0x7E,0x30,0x30, /*-- t --*/
0x30,0x30,0x30,0x1E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66, /*-- u --*/
0x66,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66, /*-- v --*/
0x66,0x66,0x3c,0x18,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x63,0x6B,0x6B, /*-- w --*/
0x6B,0x6B,0x36,0x36,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x3c, /*-- x --*/
0x18,0x3c,0x66,0x66,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66, /*-- y --*/
0x66,0x66,0x66,0x3C,0x0c,0x18,0xF0,0xF0},
{0x00,0x00,0x00,0x00,0x00,0x7E,0x06,0x0c, /*-- z --*/
0x18,0x30,0x60,0x7E,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x0c,0x18,0x18,0x18,0x30, /*-- { --*/
0x60,0x30,0x18,0x18,0x18,0x0c,0x00,0x00},
{0x00,0x00,0x00,0x18,0x18,0x18,0x18,0x18, /*-- | --*/
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18},
{0x00,0x00,0x00,0x30,0x18,0x18,0x18,0x0c, /*-- } --*/
0x06,0x0c,0x18,0x18,0x18,0x30,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x71,0xDB, /*-- ~ --*/
0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*-- . --*/
0x00,0x00,0x1c,0x1c,0x00,0x00,0x00,0x00}
}; 
/****汉字字模数据结构****/
typedef struct typefnt {
    unsigned char Index[2]; // 汉字内码索引
    unsigned char Msk[32];  // 点阵码数据
} FNT_GBHZ;
/****
汉字字模表
汉字库：横向取模，数据排列：从上到下
****/
FNT_GBHZ code GBHZ_Table[] = {
{	"■",
	0x00,0x00,0x7F,0xFC,0x7F,0xFC,0x7F,0xFC,
     0x7F,0xFC,0x7F,0xFC,0x7F,0xFC,0x7F,0xFC,
     0x7F,0xFC,0x7F,0xFC,0x7F,0xFC,0x7F,0xFC,
     0x7F,0xFC,0x7F,0xFC,0x00,0x00,0x00,0x00},/*"■",*/
{"液",
	0x40,0x40,0x20,0x20,0x27,0xFE,0x09,0x20,
     0x89,0x20,0x52,0x7c,0x52,0x44,0x16,0xA8,
     0x2B,0x98,0x22,0x50,0xE2,0x20,0x22,0x30,
     0x22,0x50,0x22,0x88,0x23,0x0E,0x22,0x04},/*"液",*/
{"晶",0x00,0x00,0x0F,0xF0,0x08,0x10,0x0F,0xF0,
     0x08,0x10,0x0F,0xF0,0x08,0x10,0x00,0x00,
     0x7E,0x7E,0x42,0x42,0x7E,0x7E,0x42,0x42,
     0x42,0x42,0x7E,0x7E,0x42,0x42,0x00,0x00},/*"晶",*/
{"显",0x00,0x00,0x1F,0xF0,0x10,0x10,0x1F,0xF0,
     0x10,0x10,0x1F,0xF0,0x04,0x40,0x04,0x40,
     0x44,0x48,0x24,0x48,0x14,0x50,0x14,0x60,
     0x04,0x40,0xFF,0xFE,0x00,0x00,0x00,0x00},/*"显",*/
{"示",0x00,0x00,0x1F,0xF8,0x00,0x00,0x00,0x00,
     0x00,0x00,0x7F,0xFE,0x01,0x00,0x01,0x00,
     0x11,0x20,0x11,0x10,0x21,0x08,0x41,0x0c,
     0x81,0x04,0x01,0x00,0x05,0x00,0x02,0x00},/*"示",*/
{"测",0x40,0x02,0x27,0xC2,0x24,0x42,0x84,0x52,
     0x45,0x52,0x55,0x52,0x15,0x52,0x25,0x52,
     0x25,0x52,0x25,0x52,0xC5,0x52,0x41,0x02,
     0x42,0x82,0x42,0x42,0x44,0x4A,0x48,0x04},/*"测",*/
{"试",0x00,0x20,0x40,0x28,0x20,0x24,0x30,0x24,
     0x27,0xFE,0x00,0x20,0xE0,0x20,0x27,0xE0,
     0x21,0x20,0x21,0x10,0x21,0x10,0x21,0x0A,
     0x29,0xCA,0x36,0x06,0x20,0x02,0x00,0x00},/*"试",*/
{"屏",0x3F,0xFC,0x20,0x04,0x20,0x04,0x3F,0xFC,
     0x24,0x10,0x22,0x20,0x2F,0xFC,0x22,0x20,
     0x22,0x20,0x3F,0xFE,0x22,0x20,0x22,0x20,
     0x44,0x20,0x44,0x20,0x88,0x20,0x10,0x20},/*"屏",*/
{"幕",0x04,0x40,0xFF,0xFE,0x04,0x40,0x1F,0xF0,
     0x10,0x10,0x1F,0xF0,0x10,0x10,0x1F,0xF0,
     0x02,0x00,0xFF,0xFE,0x09,0x10,0x1F,0xE8,
     0x29,0x26,0xC9,0x24,0x09,0x60,0x01,0x00},/*"幕",*/
{"分",0x08,0x80,0x0c,0x80,0x08,0x40,0x10,0x20,
     0x10,0x30,0x20,0x18,0x40,0x0E,0x9F,0xE4,
     0x04,0x20,0x04,0x20,0x04,0x20,0x04,0x20,
     0x08,0x20,0x10,0xA0,0x20,0x40,0x40,0x00},/*"分",*/
{"辨",0x20,0x90,0x10,0x88,0xFE,0xFE,0x00,0x84,
     0x46,0xC6,0x2A,0xA8,0xFE,0x80,0x12,0xFE,
     0x10,0x90,0x7c,0x90,0x11,0x7E,0x21,0x10,
     0x22,0x10,0x42,0x10,0x84,0x10,0x08,0x10},/*"辨",*/
{"率",0x02,0x00,0x01,0x00,0x7F,0xFE,0x41,0x00,
     0x22,0x28,0x17,0xD0,0x04,0x80,0x11,0x10,
     0x22,0x48,0x47,0xC4,0x01,0x20,0xFF,0xFE,
     0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00},/*"率",*/
{"年",0x08,0x00,0x0F,0xFC,0x10,0x80,0x10,0x80,
     0x20,0x80,0x4F,0xF8,0x88,0x80,0x08,0x80,
     0x08,0x80,0x08,0x80,0xFF,0xFE,0x00,0x80,
     0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80},/*"年",*/
{"月",0x07,0xF0,0x04,0x10,0x04,0x10,0x04,0x10,
     0x07,0xF0,0x04,0x10,0x04,0x10,0x04,0x10,
     0x07,0xF0,0x04,0x10,0x08,0x10,0x08,0x10,
     0x10,0x10,0x20,0x50,0x40,0x20,0x00,0x00},/*"月",*/
{"日",0x00,0x00,0x1F,0xF0,0x10,0x10,0x10,0x10,
     0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,
     0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
     0x1F,0xF0,0x10,0x10,0x00,0x00,0x00,0x00},
{"时",
	0x00, 0x08, 0x00, 0x08, 0x7c, 0x08, 0x44, 0x08,
0x45, 0xfe, 0x44, 0x08, 0x44, 0x08, 0x7c, 0x08,
	0x44, 0x88, 0x44, 0x48, 0x44, 0x48, 0x44, 0x08, 
0x7c, 0x08, 0x44, 0x08, 0x00, 0x28, 0x00, 0x10},
{"秒",
	0x08, 0x20, 0x1c, 0x20, 0xf0, 0x20, 0x10, 0xa8, 
0x10, 0xa4, 0xfc, 0xa2, 0x11, 0x22, 0x31, 0x20, 
	0x3a, 0x24, 0x54, 0x24, 0x54, 0x28, 0x90, 0x08, 
0x10, 0x10, 0x10, 0x20, 0x10, 0xc0, 0x13, 0x00},
{"六",
	0x02, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x80, 
0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 
	0x04, 0x40, 0x04, 0x20, 0x08, 0x10, 0x08, 0x08, 
0x10, 0x08, 0x20, 0x04, 0x40, 0x04, 0x00, 0x00},
{"五",
0x00, 0x00,0x7f, 0xfc,0x02, 0x00,0x02, 0x00, 
    0x02, 0x00,0x02, 0x00,0x3f, 0xf0,0x04, 0x10,
0x04, 0x10,0x04, 0x10,0x04, 0x10,0x08, 0x10,
	0x08, 0x10,0x08, 0x10,0xff, 0xfe,0x00, 0x00},
{"四",
  0x00, 0x00,0x00, 0x00,0x7f, 0xfc,0x44, 0x44, 
    0x44, 0x44,0x44, 0x44,0x44, 0x44,0x44, 0x44, 
 0x48, 0x44,0x48, 0x3c,0x50, 0x04,0x60, 0x04, 
    0x40, 0x04,0x7f, 0xfc,0x40, 0x04,0x00, 0x00},
{"三",
  0x00, 0x00,0x00, 0x00,0x7f, 0xfc,0x00, 0x00, 
    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x3f, 0xf8, 
  0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00, 
    0x00, 0x00,0xff, 0xfe,0x00, 0x00,0x00, 0x00},
{"二",
0x00, 0x00,0x00, 0x00,0x00, 0x00,0x3f, 0xf8, 
    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00, 
0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00, 
    0xff, 0xfe,0x00, 0x00,0x00, 0x00,0x00, 0x00},
{"一",
  0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00, 
    0x00, 0x00,0x00, 0x00,0x00, 0x00,0xff, 0xfe, 
 0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00, 
    0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00},
{"星",
 0x00, 0x00, 0x1f, 0xf0,0x10, 0x10,0x1f, 0xf0, 
    0x10, 0x10,0x1f, 0xf0,0x01, 0x00,0x11, 0x00, 
0x1f, 0xf8, 0x21, 0x00,0x41, 0x00,0x1f, 0xf0, 
    0x01, 0x00,0x01, 0x00, 0x7f, 0xfc,0x00, 0x00},
{"期",
    0x22, 0x00,0x22, 0x7c,0x7f, 0x44, 0x22, 0x44, 
    0x22, 0x44,0x3e, 0x7c,0x22, 0x44,0x22, 0x44, 
    0x3e, 0x44,0x22, 0x7c,0x22, 0x44, 0xff, 0x44, 
    0x04, 0x84,0x22, 0x84, 0x41, 0x14,0x82, 0x08},
{"万",
    0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x04, 0x00, 
    0x04, 0x00, 0x04, 0x00, 0x07, 0xf0, 0x04, 0x10, 
    0x04, 0x10, 0x08, 0x10, 0x08, 0x10, 0x10, 0x10, 
    0x10, 0x10, 0x20, 0x10, 0x40, 0xa0, 0x80, 0x40},
{"历",
    0x00, 0x00, 0x3f, 0xfe, 0x20, 0x00, 0x20, 0x80, 
    0x20, 0x80, 0x20, 0x80, 0x2f, 0xfc, 0x20, 0x84, 
    0x20, 0x84, 0x21, 0x04, 0x21, 0x04, 0x22, 0x04, 
    0x22, 0x04, 0x44, 0x04, 0x48, 0x28, 0x90, 0x10},

{"当",
    0x01, 0x00,0x21, 0x08,0x11, 0x08, 0x09, 0x10, 
    0x09, 0x20,0x01, 0x00,0x7f, 0xf8, 0x00, 0x08, 
    0x00, 0x08,0x00, 0x08,0x3f, 0xf8,0x00, 0x08, 
    0x00, 0x08,0x00, 0x08,0x7f, 0xf8,0x00, 0x08},
{"前",
0x10, 0x10,0x08, 0x10,0x08, 0x20,0xff, 0xfe, 
    0x00, 0x00,0x3e, 0x08,0x22, 0x48,0x22, 0x48, 
0x3e, 0x48, 0x22, 0x48,0x22, 0x48,0x3e, 0x48, 
    0x22, 0x08,0x22, 0x08,0x2a, 0x28,0x24, 0x10},
{"温",
    0x00, 0x00, 0x23, 0xf8, 0x12, 0x08, 0x12, 0x08, 
    0x83, 0xf8, 0x42, 0x08, 0x42, 0x08, 0x13, 0xf8, 
0x10, 0x00, 0x27, 0xfc, 0xe4, 0xa4, 0x24, 0xa4, 
    0x24, 0xa4, 0x24, 0xa4, 0x2f, 0xfe, 0x00, 0x00},
{"度",
    0x01, 0x00, 0x00, 0x80, 0x3f, 0xfe, 0x22, 0x20, 
    0x22, 0x20, 0x3f, 0xfc, 0x22, 0x20, 0x22, 0x20, 
    0x23, 0xe0, 0x20, 0x00, 0x2f, 0xf0, 0x24, 0x10, 
    0x42, 0x20, 0x41, 0xc0, 0x86, 0x30, 0x38, 0x0e},
{"℃",
    0x60, 0x00, 0x91, 0xf4, 0x96, 0x0c, 0x6c, 0x04, 
    0x08, 0x04, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
    0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x08, 0x00, 
    0x0c, 0x04, 0x06, 0x08, 0x01, 0xf0, 0x00, 0x00}/*"日",*/
};
/****函数声明****/
void LCD_Busy();       // 检测LCD控制器状态
void LCD_Data(unsigned char dat); // 写数据
void LCD_Com1(unsigned char command); // 写1个参数
void LCD_Com2(unsigned char dat, unsigned char command); // 写2个参数
void LCD_Com3(unsigned char data1, unsigned char data2, unsigned char command); // 写3个参数
void LCD_Clear();       // 清屏
void LCD_Init();        // PG160128初始化
void LCD_HZ(unsigned int x, unsigned int y, unsigned int n); // 显示一个汉字
void LCD_CHAR(unsigned int x, unsigned int y, unsigned int n); // 显示一个字符
uint findHzIndex(uchar *hz); // 查找汉字在汉字库中的位置
void writeString(unsigned char *pcStr, unsigned short x0, unsigned int y0); // 显示中英混合字符串
/****
判断LCD是否准备好
****/
void LCD_Busy() {
    unsigned int dat;
	do{
		DataPort=0xff;  // clear P1
		LCD_CD=1;       // CD=1
		LCD_RD=0;       //RD=0
		dat=DataPort;   // read
		LCD_RD=1;       // RD=0
		dat=0x03&dat;
	}
	while(dat!=0x03);
}
/****
写数据
****/
void LCD_Data(unsigned char dat) {
    LCD_Busy(); // 判断状态
    LCD_CD = 0;  // CD = 0
    DataPort = dat; // write
    LCD_WR = 0;  // WR = 0
    LCD_WR = 1;  // WR = 1
}
/****
写有1个参数命令
****/
void LCD_Com1(unsigned char command) {
    LCD_Busy(); // 判断状态
    LCD_CD = 1; // CD = 1
    DataPort = command; // 写命令
    LCD_WR = 0;  // WR = 0
    LCD_WR = 1;  // WR = 1
}
/****
写有2个参数命令
****/
void LCD_Com2(unsigned char dat, unsigned char command) {
    LCD_Data(dat); // 写数据
    LCD_Com1(command); // 写命令
}
/****
写有3个参数命令
****/
void LCD_Com3(unsigned char data1, unsigned char data2, unsigned char command) {
    LCD_Data(data1); // 写参数1
    LCD_Data(data2); // 写参数2
    LCD_Com1(command); // 写命令
}
/****
清屏
****/
void LCD_Clear() {
    long i;
    LCD_Com3(0x00, 0x00, LC_ADD_POS); // 设置光标位置
    LCD_Com1(LC_AUT_WR); // 设置自动写
    for (i = 0; i < 20480; i++) LCD_Data(0x00); // 160*128
    LCD_Com1(LC_AUT_OVR); // 取消自动写
}
/****
初始化LCD
****/
void LCD_Init() {
    LCD_CE = 0;
    LCD_CD = 1; // CD = 1
    LCD_WR = 1; // WR = 1
    LCD_RD = 1; // RD = 1

    LCD_Com1(LC_DIS_SW); // 显示禁用
    LCD_Com3(0x00, 0x00, LC_GRH_STP); // 设置图形显示首地址
    LCD_Com3(Gus_LCM_YMAX, 0x00, LC_GRH_WID); // 设置图形显示宽度
    LCD_Com1(LC_MOD_OR); // 设置显示模式
    LCD_Com1(LC_DIS_SW | 0x08); // 启动图形显示
    LCD_Clear(); // 清屏
}
/****
显示一个汉字
参数：x为显示第几行，y为显示第几列，n为显示的第几个字符
****/
void LCD_HZ(unsigned int x, unsigned int y, unsigned int n) {
    unsigned int i, StartAddr;
    FNT_GBHZ *ptGb16 = (FNT_GBHZ *)GBHZ_Table;
    StartAddr = x * 160 + y; // 显示开始地址
    for (i = 0; i < 16; i++) {
        LCD_Com3(StartAddr, (StartAddr / 256), LC_ADD_POS);
        // 设置地址指针位置不能动
        LCD_Com2(ptGb16[n].Msk[i * 2], LC_INC_WR);
        // 数据一次写地址加1
        LCD_Com2(ptGb16[n].Msk[i * 2 + 1], LC_NOC_WR);
        // 数据一次写地址不变
        StartAddr += 20; // 地址移动到下一行，20使宽度：160/8
    }
}
/****
显示一个字符
参数：x为显示第几行，y为显示第几列，n为显示的第几个字符
****/
void LCD_CHAR(unsigned int x, unsigned int y, unsigned int n) {
    unsigned int i, StartAddr;
    StartAddr = x * 160 + y; // 显示开始地址
    for (i = 0; i < 16; i++) {
        LCD_Com3(StartAddr, (StartAddr / 256), LC_ADD_POS);
        // 设置地址指针位置不能动
        LCD_Com2(ASC_MSK[n][i], LC_INC_WR);
        // 数据一次写地址加1
        StartAddr += 20; // 地址移动到下一行，20使宽度：160/8
    }
}
/****
查找汉字在汉字库中的位置
参数：*hz为当前那要显示的汉字编码
****/
uint findHzIndex(uchar *hz) {
    uint i = 0;
    FNT_GBHZ *ptGb16 = (FNT_GBHZ *)GBHZ_Table;
    while (ptGb16[i].Index[0] > 0x80) {
        if ((*hz == ptGb16[i].Index[0]) && (*(hz + 1) == ptGb16[i].Index[1])) {
            return i;
        }
        i++;
        if (i > (sizeof(GBHZ_Table) / sizeof(FNT_GBHZ) - 1)) // 汉字库中汉字总数
		{
            break;
        }
    }
    return 0;
}
/****
显示中英混合字符串
参数：*pcStr为当前显示的字符串，x为显示第几行，y为显示第几列
****/
void writeString(unsigned char *pcStr, unsigned short x0, unsigned int y0) {
    uchar i, uLen;
    unsigned int usIndex;
    unsigned int usWidth = 0;
    FNT_GBHZ *ptGb16 = 0;
    ptGb16 = (FNT_GBHZ *)GBHZ_Table;
    i = 0;
    uLen = strlen(pcStr);
    while (1) 
	{
        if (i >= uLen)  // 字符串显示结束 
		{
            break;
        }
        if (*pcStr > 0x80)  //中文字符显示
		{
			if (x0+2>Gus_LCM_XMAX)
			{
				x0=0;       // 横坐标超出显示范围
				y0=0;
			}
			if (y0+2>Gus_LCM_YMAX)
			{
				x0=x0+2;       // 纵坐标超出显示范围
				y0=0;
			}
			usIndex=findHzIndex(pcStr); // 查找字符位置
			LCD_HZ(x0,y0,usIndex);      // 显示一个汉字字符
			y0=y0+2;                    // 指向下一个显示位置
			pcStr+=2;                   // 指向下一个显示字符
			i+=2;
		}
		else     // 西文字符显示
		{
			if (*pcStr == '\r')  // 特殊字符：换行处理
			{
				x0=x0+2;
				if (x0+2>=Gus_LCM_XMAX)
				{
					x0=0;
				}
				pcStr++;i++;
				continue;
			}
			else if(*pcStr == '\n')  // 特殊字符：对齐处理
			{
				y0=0;
				pcStr++;i++;
				continue;
			}
			else
			{
				if (x0+1>Gus_LCM_XMAX)
			    {
				    x0=0;       // 横坐标超出显示范围
				    y0=0;
			    }
			    if (y0+1>Gus_LCM_YMAX)
			    {
				    x0=x0+2;       // 纵坐标超出显示范围
			   	    y0=0;
			    }
				usIndex=*pcStr-0x20; // 字符显示位置计算
				LCD_CHAR(x0,y0,usIndex); // 显示一个西文字符
				y0=y0+1;              // 指向下一个显示位置
				pcStr++;              // 指向下一个显示字符
				i++;
			}
		}
	}
}

	