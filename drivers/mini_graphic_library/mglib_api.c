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
#include "mglib_api.h"
#include "malloc.h"
#include "stdio.h"
#include "linux/string.h"

extern RESOLUTION_S gResolution_Layer1;
extern RESOLUTION_S gResolution_Layer2;

extern FONT_S* gFont;

#define PROGRESSBAR_TEXT_BOUNDARY 10
#define TEXTBAR_TEXT_BOUNDARY 10

err_no eswin_mgl_create_progressbar(PROGRESSBAR_S** progressBar, u32 x, u32 y, u32 w, u32 h, u32 highlightColor, u32 bgColor, u32* osdBuf)
{
    if((x + w > gResolution_Layer1.w) ||
            (y + h > gResolution_Layer1.h))
    {
        printf("mini graphic library : %s : invalid x,y !\n", __func__);
        return ERR_INVAL;
    }

    *progressBar = (PROGRESSBAR_S*)malloc(sizeof(PROGRESSBAR_S));

    if(NULL == progressBar)
    {
        printf("mini graphic library : %s : malloc fail!\n",__func__);
        return ERR_OOMEM;
    }

    (*progressBar)->x = x;
    (*progressBar)->y = y;
    (*progressBar)->length = w;
    (*progressBar)->height = h;
    (*progressBar)->highlightColor = highlightColor;
    (*progressBar)->bgColor = bgColor;
    (*progressBar)->text_x = 0;
    (*progressBar)->text_y = 0;
    (*progressBar)->percentage_x = 0;
    (*progressBar)->percentage_y = 0;
    /* background part draw */
    eswin_mgl_draw_rect((*progressBar)->x, (*progressBar)->y,
                        (*progressBar)->length, (*progressBar)->height,
                        bgColor, osdBuf);
    return RET_OK;
}

void eswin_mgl_distory_progressBar(PROGRESSBAR_S** progressBar)
{
    if(*progressBar != NULL)
    {
        free(*progressBar);
        *progressBar = NULL;
    }
    else 
        printf("mini graphic library: %s: progressBar is NULL, can't distory!\n", __func__);
}

err_no  eswin_mgl_update_progressBar_percentage(PROGRESSBAR_S* progressBar, u16 percentage, u32 charColor, u32 charbgColor, u32* osdBuf)
{
    if(percentage > 100)
    {
        printf("mini graphic library: %s: invalid percentage \n", __func__);
        return ERR_INVAL;
    }
    char str_percentage[5]="000%";

    u16 highlight_length = percentage * progressBar->length / 100;
    /* highlight part draw*/
    eswin_mgl_draw_rect(progressBar->x, progressBar->y, highlight_length,
                        progressBar->height, progressBar->highlightColor, osdBuf);
    /* background part draw */
    eswin_mgl_draw_rect(progressBar->x + highlight_length, progressBar->y,
                        progressBar->length - highlight_length, progressBar->height, progressBar->bgColor, osdBuf);

    progressBar->percentage_x = progressBar->x + progressBar->length/2;
    progressBar->percentage_y = progressBar->y - gFont->fontSize.h - PROGRESSBAR_TEXT_BOUNDARY;

    if(percentage < 10 )
    {
        str_percentage[0] = ' ';
        str_percentage[1] = ' ';
        str_percentage[2] = (char)('0' + percentage);
    }
    else if(percentage < 100)
    {
        str_percentage[0] = ' ';
        str_percentage[1] = (char)('0' + percentage / 10);
        str_percentage[2] = (char)('0' + percentage % 10);
    }
    else
    {
        str_percentage[0] = '1';
        str_percentage[1] = '0';
        str_percentage[2] = '0';
    }

    /* because percentage length fixed = 4 bytes,
     * so no need to clear percentage area
     */
    eswin_mgl_draw_string(progressBar->percentage_x,
                          progressBar->percentage_y, str_percentage,
                          charColor, charbgColor, osdBuf);
    return RET_OK;
}

err_no eswin_mgl_update_progressBar_text(PROGRESSBAR_S* progressBar, char* string, u32 charColor, u32 charbgColor, u32* osdBuf)
{

    u16 str_x, str_y, str_nums;

    progressBar->percentage_x = progressBar->x + progressBar->length / 2;
    progressBar->percentage_y = progressBar->y - gFont->fontSize.h - PROGRESSBAR_TEXT_BOUNDARY;

    str_nums = _strlen(string);
    str_x = progressBar->percentage_x - str_nums * gFont->fontSize.w - PROGRESSBAR_TEXT_BOUNDARY;
    str_y = progressBar->percentage_y;

    /* clear text area */
    if(progressBar->text_x != 0)
    {
        eswin_mgl_draw_rect(progressBar->x, progressBar->text_y,
                            progressBar->length, progressBar->height,
                            SYSTEM_BLANK_COLOR, osdBuf);
    }
    progressBar->text_x = str_x;
    progressBar->text_y = str_y;

    /* draw new string */
    return eswin_mgl_draw_string(progressBar->text_x,
                                 progressBar->text_y, string,
                                 charColor, charbgColor, osdBuf);
}

err_no eswin_mgl_create_textbar(TEXTBAR_S** textBar, u32 x, u32 y, u32 w, u32 h, u32 bgColor, u32* osdBuf)
{
    if((x + w > gResolution_Layer1.w) ||
            (y + h > gResolution_Layer1.h))
    {
        printf("mini graphic library : %s : invalid x,y !\n", __func__);
        return ERR_INVAL;
    }

    *textBar = (TEXTBAR_S*)malloc(sizeof(TEXTBAR_S));
    if(NULL == textBar)
    {
        printf("mini graphic library : %s : malloc fail!\n",__func__);
        return ERR_OOMEM;
    }

    (*textBar)->x = x;
    (*textBar)->y = y;
    (*textBar)->length = w;
    (*textBar)->height = h;
    (*textBar)->bgColor = bgColor;

    (*textBar)->text_x = (*textBar)->x + TEXTBAR_TEXT_BOUNDARY;
    (*textBar)->text_y = (*textBar)->y + ((*textBar)->height - gFont->fontSize.h) / 2;
    (*textBar)->maxCharNums = ((*textBar)->length - TEXTBAR_TEXT_BOUNDARY * 2) /
                              (gFont->fontSize.w);
    (*textBar)->lastCharNums = 0;

    /* background part draw */
    eswin_mgl_draw_rect((*textBar)->x, (*textBar)->y,
                        (*textBar)->length, (*textBar)->height,
                        (*textBar)->bgColor, osdBuf);
    return RET_OK;
}

void eswin_mgl_distory_textbar(TEXTBAR_S** textBar)
{
    if(*textBar != NULL)
    {
        free(*textBar);
        *textBar = NULL;
    }
}


err_no eswin_mgl_update_text(TEXTBAR_S* textBar, u32 charColor, char* string, u32* osdBuf)
{
    u16 strlen = _strlen(string);
    /* last byte used to put '\0' */
    char strbuf[textBar->maxCharNums + 1];

    /* clear textBar last time draw area  */
    if(1 == textBar->lastCharNums)
    {
        eswin_mgl_draw_rect(textBar->x, textBar->y,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);

    }
    else if(2 == textBar->lastCharNums)
    {

        eswin_mgl_draw_rect(textBar->x, textBar->y,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);

        eswin_mgl_draw_rect(textBar->x, textBar->y + textBar->height,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);
    }
    else if(3 == textBar->lastCharNums)
    {
        eswin_mgl_draw_rect(textBar->x, textBar->y,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);

        eswin_mgl_draw_rect(textBar->x, textBar->y + textBar->height,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);

        eswin_mgl_draw_rect(textBar->x, textBar->y + textBar->height * 2,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);
    }

    /* one line  */
    if(strlen < textBar->maxCharNums)
    {
        textBar->lastCharNums = 1;
        /* draw new string */
        eswin_mgl_draw_string(textBar->text_x,
                              textBar->text_y, string,
                              charColor, textBar->bgColor, osdBuf);
    }  /* second line */
    else if(strlen < textBar->maxCharNums * 2)
    {
        textBar->lastCharNums = 2;
        strncpy(strbuf, string, textBar->maxCharNums);
        strbuf[textBar->maxCharNums] = '\0';
        /* draw first line */
        eswin_mgl_draw_string(textBar->text_x,
                              textBar->text_y, strbuf,
                              charColor, textBar->bgColor, osdBuf);
        /*
         * draw second line
         * background part draw
         */
        eswin_mgl_draw_rect(textBar->x, textBar->y + textBar->height,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);

        strncpy(strbuf, string + textBar->maxCharNums, strlen - textBar->maxCharNums);
        strbuf[strlen - textBar->maxCharNums] = '\0';
        eswin_mgl_draw_string(textBar->text_x,
                              textBar->text_y + textBar->height, strbuf,
                              charColor, textBar->bgColor, osdBuf);
    }  /* third line */
    else if(strlen < textBar->maxCharNums * 3)
    {
        textBar->lastCharNums = 3;
        strncpy(strbuf, string, textBar->maxCharNums);
        strbuf[textBar->maxCharNums] = '\0';
        /* draw first line */
        eswin_mgl_draw_string(textBar->text_x,
                              textBar->text_y, strbuf,
                              charColor, textBar->bgColor, osdBuf);
        /*
         * draw second line
         * background part draw
         */
        eswin_mgl_draw_rect(textBar->x, textBar->y + textBar->height,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);

        strncpy(strbuf, string + textBar->maxCharNums, textBar->maxCharNums);
        strbuf[textBar->maxCharNums * 2] = '\0';
        eswin_mgl_draw_string(textBar->text_x,
                              textBar->text_y + textBar->height, strbuf,
                              charColor, textBar->bgColor, osdBuf);
        /*
         * draw third line
         * background part draw
         */
        eswin_mgl_draw_rect(textBar->x, textBar->y + textBar->height * 2,
                            textBar->length, textBar->height,
                            textBar->bgColor, osdBuf);

        strncpy(strbuf, string + textBar->maxCharNums * 2, strlen - textBar->maxCharNums * 2);
        strbuf[strlen - textBar->maxCharNums * 2] = '\0';
        eswin_mgl_draw_string(textBar->text_x,
                              textBar->text_y + textBar->height * 2, strbuf,
                              charColor, textBar->bgColor, osdBuf);
    }
    else
    {
        printf("mini graphic library: %s: text is to long\n", __func__);
        return ERR_INVAL;
    }
    return RET_OK;
}

/* set all pixel transparent in osdBuf of frongPanle */
void eswin_mgl_frontPanel_init(u32* osdBuf)
{
    u32 color;
    color = SYSTEM_BLANK_COLOR;
    eswin_mgl_set_default_color(gResolution_Layer1, color, osdBuf);
}
