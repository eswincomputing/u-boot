// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 * SPDX-License-Identifier: GPL-2.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: JackYaun <yuanxiaofeng@eswin.com>
 */

#include "mglib_driver.h"
#include "linux/string.h"
#include "stdio.h"
#include "font.h"
#include "font_zk.h"
#include "error_code.h"
#include "../display_logo/display_logo.h"
#include "video.h"
#include "video_eswin.h"

extern const unsigned char* font24x24zk_table[];
extern const unsigned char* font12x24ascii_table[];
extern const unsigned char* font24x24zk_code;

RESOLUTION_S gResolution_Layer1 =
{
    .w = DRM_ESWIN_FB_WIDTH,
    .h = DRM_ESWIN_FB_HEIGHT
};

RESOLUTION_S gResolution_Layer2 =
{
    .w = DRM_ESWIN_FB_WIDTH,
    .h = DRM_ESWIN_FB_HEIGHT
};

/* layer 1 only support ARGB8888 */
COLOR_FMT_E gColorFmt_Layer1 = ARGB8888;
COLOR_FMT_E gColorFmt_Layer2 = ARGB8888;

FONT_S defaultFont =
{
    .fontName = "default",
    .fontData = NULL,
    .fontSize =
    {
        .w = 16,
        .h = 24
    }
};

FONT_S* gFont = &defaultFont;

/*
 * if support other size , add width height in
 * this function
 */
err_no eswin_mgl_set_fontSize(FONT_SIZE_E size)
{
    switch (size)
    {
    case ASCII_12x24:
        strcpy(gFont->fontName,"default");
        gFont->fontSize.w = 16;
        gFont->fontSize.h = 24;
        gFont->fontData = NULL;
        break;
    default :
        printf("mini graphic library: %s: unsupport fontSize\n", __func__);
        return ERR_INVAL;
    }
    return RET_OK;
}

unsigned int _strlen(const void *vstr)
{
    u32 ret_val;
    const unsigned char * str = (const unsigned char *)vstr;
    for(ret_val = 0; *str != '\0'; str++)
        ret_val++;
    return ret_val;
}

err_no eswin_mgl_display_update(OSDLAYER_E layer)
{
    err_no ret = RET_FAILED;

    // printf("mini graphic library: %s: please add call osd update api\n", __func__);

    ret = eswin_show_fbbase(layer);
    return ret;
}

err_no eswin_mgl_set_resolution(OSDLAYER_E layer, RESOLUTION_S resolution)
{
    err_no ret = RET_OK;
    switch (layer)
    {
    case OSDLAYER1:
        gResolution_Layer1.w  = resolution.w;
        gResolution_Layer1.h  = resolution.h;
        break;
    case OSDLAYER2:
        gResolution_Layer2.w  = resolution.w;
        gResolution_Layer2.h  = resolution.h;
        break;
    default :
        ret = ERR_INVAL;
    }
    return ret;
}

err_no eswin_mgl_set_colorFmt(OSDLAYER_E layer, COLOR_FMT_E colorFmt)
{
    err_no ret = RET_OK;
    switch (layer)
    {
    case OSDLAYER1:
        printf("mini graphic library : %s : only layer 2 can set color fmt!\n", __func__);
        ret = ERR_INVAL;
        break;
    case OSDLAYER2:
        gColorFmt_Layer2 = colorFmt;
        break;
    default :
        ret = ERR_INVAL;
    }
    return ret;
}

err_no eswin_mgl_set_osdBuf (OSDLAYER_E layer, RESOLUTION_S resolution, COLOR_FMT_E colorFmt)
{
    err_no ret = RET_OK;
    switch (layer)
    {
    case OSDLAYER1:
    case OSDLAYER2:
        /* call osd api to set resolution and colorFmt; */
        printf("mini graphic library : %s : please finsih this func", __func__);
        break;
    default :
        ret = ERR_INVAL;
    }
    return ret;
}


OSD_INFO_S eswin_mgl_get_osdBuf(OSDLAYER_E layer)
{
    OSD_INFO_S osdInfo;

    memset(&osdInfo, 0, sizeof(OSD_INFO_S));

    if((layer != OSDLAYER1) && (layer != OSDLAYER2)) {
        printf("mini graphic library : %s : invalid layer, please check!\n", __func__);
    } else {
    /* get osdBuf from osd */
        osdInfo.osdBuf = (u32 *)(u64)eswin_get_fbbuf(layer);
        osdInfo.resolution.w = eswin_get_fbwidth(layer);
        osdInfo.resolution.h = eswin_get_fbheight(layer);
        osdInfo.colorFmt = eswin_get_fbbpp(layer);
    }
    return osdInfo;
}

err_no eswin_mgl_get_resolution(OSDLAYER_E layer, RESOLUTION_S* resolution)
{
    err_no ret = RET_OK;
    switch (layer)
    {
    case OSDLAYER1:
        resolution->w = gResolution_Layer1.w;
        resolution->h = gResolution_Layer1.h;
        break;
    case OSDLAYER2:
        resolution->w = gResolution_Layer2.w;
        resolution->h = gResolution_Layer2.h;
        break;
    default :
        ret = ERR_INVAL;
    }
    return ret;

}

static void get_char_font_data(const u32 ch, u8** fontData, int* width, int* height)
{
    int pos, i;
    /* decode chinese gb2312*/
    u8 CH = ch >> 8;      /* high byte */
    u8 CL = ch & 0xFF;   /* low byte */

    if (CH >= 0xA1 && CH < 0xF7 && CL >= 0xA1 && CL < 0xFF)
    {
        for(i=0; i<_strlen(font24x24zk_code); i+=2)
        {
            if(CH == font24x24zk_code[i]  && CL == font24x24zk_code[i+1])
                break;
        }

        pos = i/2;

        /*find the font data of this char*/
        *fontData = (u8*)font24x24zk_table[pos];
        *width = 24;
        *height = 24;

        return;
    }

    /*ascii support*/
    if (ch >= 0 && ch <0x80)
    {
        pos = (int)ch;

        *fontData = (u8*)font12x24ascii_table[pos]; /* font->fontData[pos]; */
        *width = 16;
        *height = 24;

        return;
    }
}

static void eswin_mgl_draw_char(DRAW_CHAR_S* pchar, u32* osdBuf)
{
    u16 screen_line_length = gResolution_Layer1.w;
    u32* dest = osdBuf + screen_line_length * pchar->position.y + pchar->position.x;
    u8 bits;

    u32 x, y, i;
    for (y = 0; y <= pchar->charSize.h; y++, dest+= screen_line_length)
    {
        u32 *d = dest;
        for (x = 0; x < pchar-> charSize.w / 8; x++)
        {
            bits = pchar->pData[(y * pchar->charSize.w) / 8 + x];
            for (i = 0; i <8; i++)
            {
                *d++ = bits & 0x80 ? pchar-> charColor : pchar->charbgColor;
                bits <<= 1;
            }
        }
    }
}

err_no eswin_mgl_draw_string(u16 x, u16 y, char* string, u32 charColor, u32 charbgColor, u32* osdBuf)
{
    int width = 0;
    int height = 0;
    int start_x = x;
    int start_y = y;
    u8* fontData = 0;

    const char *str = string;
    int cc = _strlen(string);

    if((x > gResolution_Layer1.w) ||
            (y > gResolution_Layer1.h))
    {
        printf("mini graphic library : %s : invalid x,y !\n", __func__);
        return ERR_INVAL;
    }


    if (cc >= (gResolution_Layer1.w - x) / gFont->fontSize.w)
    {
        printf("mini graphic library: %s: text is to long\n", __func__);
        return ERR_INVAL;
    }

    while (--cc >= 0)
    {
        u32 ch = *str++;

        /* chinese gb2312 decoding*/
        if (ch >= 0xA1 && ch < 0xF8 && cc >= 1 &&
                *str >= 0xA1 && *str < 0xFF)
        {
            ch = (ch << 8) | *str++;
            --cc;
        }

        get_char_font_data(ch, &fontData, &width, &height);

        if (!fontData) /*  no found data */
        {
            printf("mini graphic library : %s : font %c not found in font library\n", __func__, ch);
            continue;
        }
        DRAW_CHAR_S sChar;

        sChar.pData = fontData;

        sChar.position.x = start_x;
        sChar.position.y = start_y;
        sChar.position.x &= ~1;
        sChar.position.y &= ~1;

        sChar.charSize.w = width;
        sChar.charSize.h = height;

        sChar.charColor = charColor;
        sChar.charbgColor = charbgColor;

        eswin_mgl_draw_char(&sChar, osdBuf);
        start_x += width;
    }
    return RET_OK;
}

err_no eswin_mgl_draw_point(u16 x, u16 y, u32 color, u32* osdBuf)
{
    if((x > gResolution_Layer1.w) ||
            (y > gResolution_Layer1.h))
    {
        printf("mini graphic library : %s : invalid x,y !\n", __func__);
        return ERR_INVAL;
    }

    u16 screen_line_length = gResolution_Layer1.w;
    u32* dest = osdBuf + screen_line_length * y + x;

    *dest = color;

    return RET_OK;
}

/*
 * only support draw horiontal line or vertical line
 */

#define IS_SUPPORT_FLOAT false
err_no eswin_mgl_draw_line(POINT_S point1, POINT_S point2, u32 color, u32* osdBuf)
{
    u16 i = 0;
    u16 screen_line_length = gResolution_Layer1.w;

    if((point1.x > gResolution_Layer1.w) ||
            (point1.y > gResolution_Layer1.h))
    {
        printf("mini graphic library : %s : invalid ponit1 !\n", __func__);
        return ERR_INVAL;
    }

    if((point2.x > gResolution_Layer1.w) ||
            (point2.y > gResolution_Layer1.h))
    {
        printf("mini graphic library : %s : invalid ponit2 !\n", __func__);
        return ERR_INVAL;
    }

#if IS_SUPPORT_FLOAT
    /* can draw slash */
    double k = 0.0;

    if(point1.x < point2.x)
    {
        k =(long double) (point2.y - point1.y) / (point2.x - point1.x);
        u32* dest = osdBuf + screen_line_length * point1.y + point1.x;
        u32* d = dest;
        for(i=0; i <= point2.x - point1.x; i++)
        {
            *d = color;
            d = dest + (u32)(screen_line_length * k * i + 1);
        }
        return RET_OK;
    }

    if(point1.x > point2.x)
    {
        k = (long double) (point1.y - point2.y) / (point1.x - point2.x);
        u32* dest = osdBuf + screen_line_length * point2.y + point2.x;
        u32* d = dest;
        for(i=0; i <= point1.x - point2.x; i++)
        {
            *d = color;
            d = dest + (u32)(screen_line_length * k * i + 1);
        }
        return RET_OK;
    }

#else
    /* can't draw slash */
    if(point1.y == point2.y)/* a vertical line */
    {
        if(point2.x >= point1.x)
        {
            u32* dest = osdBuf + screen_line_length * point1.y + point1.x;
            for(i=0; i<=point2.x - point1.x; i++)
            {
                *dest++ = color;
            }
        }
        else
        {
            u32* dest = osdBuf + screen_line_length * point2.y + point2.x;
            for(i=0; i<=point1.x - point2.x; i++)
            {
                *dest++ = color;
            }
        }
        return RET_OK;
    }

#endif
    if(point1.x == point2.x) /* a vectical line */
    {
        if(point2.y >= point1.y) /* ponit2 is behind ponit1 */
        {
            u32* dest = osdBuf + screen_line_length * point1.y + point1.x;
            for(i=0; i <= point2.y - point1.y; i++)
            {
                *dest = color;
                dest += screen_line_length;
            }
        }
        else /* ponit1 behind point2 */
        {
            u32* dest = osdBuf + screen_line_length * point2.y + point2.x;
            for(i=0; i <= point1.y - point2.y; i++)
            {
                *dest = color;
                dest += screen_line_length;
            }
        }
        return RET_OK;
    }

    printf("mini graphic librfary : %s : draw slash is unsupported !\n",__func__);
    return ERR_PERM;
}

err_no eswin_mgl_draw_rect(u16 x, u16 y, u16 length, u16 height, u32 fillColor, u32* osdBuf)
{
    if((x + length > gResolution_Layer1.w) ||
            (y + height> gResolution_Layer1.h))
    {
        printf("mini graphic library : %s : invalid x,y !\n", __func__);
        return ERR_INVAL;
    }

    u16 screen_line_length = gResolution_Layer1.w;
    u32* dest = osdBuf + screen_line_length * y + x;

    u32 i, j;
    for (i = 0; i <= height; i++, dest+= screen_line_length)
    {
        u32 *d = dest;
        for (j = 0; j < length; j++)
        {
            *d++ = fillColor;
        }
    }
    return RET_OK;
}

/* set all pixel default color in osdBuf */
void eswin_mgl_set_default_color(RESOLUTION_S resolution, u32 color, u32* osdBuf)
{
    u16 screen_line_length = resolution.w;
    u32* dest = osdBuf;

    u32 x, y;
    for (y = 0; y < resolution.h; y++, dest+= screen_line_length)
    {
        u32 *d = dest;
        for (x = 0; x < resolution.w; x++)
        {
            *d++ = color;
        }
    }
}

err_no eswin_mgl_get_background_data(u8 *osdBuf)
{
    err_no ret = RET_OK;
    if(NULL == osdBuf)
    {   
        printf("mini graphic library: %s: osdBuf is null!\n", __func__);
        return ERR_INVAL;
    }   

    ret = eswin_display_logo_get_logo_data(osdBuf, gResolution_Layer2, gColorFmt_Layer2);

    //get relevant data form emmc logo partition according to type, resolution
    printf("mini graphic library: %s: please finish the func\n", __func__);
    return ret;                                                                                                                         
}

err_no eswin_mgl_draw_background(BACKGROUNDTYPE_E type, COLOR_FMT_E colorFmt, u32 bgColor, void* osdBuf)
{
    err_no ret = RET_OK;

    switch (type)
    {
    case PICTURE_BG :
        ret = eswin_mgl_get_background_data(osdBuf);
        break;
    case PURECOLOR_BG:
        if(colorFmt != ARGB8888)
            return ERR_INVAL;
        eswin_mgl_set_default_color(gResolution_Layer2, bgColor, osdBuf);
        break;
    default :
        printf("mini graphic library: %s: wrong backgound type\n", __func__);
        ret = ERR_INVAL;
    }
    return ret;
}

void eswin_mgl_exit(void)
{
    //some exit job if need
}

