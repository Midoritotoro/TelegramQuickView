#pragma once 


#define CONFIG_HINT_CATEGORY                0x02  /* Start of new category */

#define CONFIG_SUBCATEGORY                  0x07 /* Set subcategory */
#define CONFIG_SECTION                      0x08 /* Start of new section */

/* Configuration item types */
#define CONFIG_ITEM_FLOAT                   (1 << 5)  /* Float option */
#define CONFIG_ITEM_INTEGER                 (2 << 5)  /* Integer option */
#define CONFIG_ITEM_RGB                     (CONFIG_ITEM_INTEGER | 0x01)  /* RGB color option */
#define CONFIG_ITEM_BOOL                    (3 << 5)  /* Bool option */
#define CONFIG_ITEM_STRING                  (4 << 5)  /* String option */
#define CONFIG_ITEM_PASSWORD                (CONFIG_ITEM_STRING | 0x01)  /* Password option (*) */
#define CONFIG_ITEM_KEY                     (CONFIG_ITEM_STRING | 0x02)  /* Hot key option */
#define CONFIG_ITEM_MODULE                  (CONFIG_ITEM_STRING | 0x04)  /* Module option */
#define CONFIG_ITEM_MODULE_CAT              (CONFIG_ITEM_STRING | 0x05)  /* Module option */
#define CONFIG_ITEM_MODULE_LIST             (CONFIG_ITEM_STRING | 0x06)  /* Module option */
#define CONFIG_ITEM_MODULE_LIST_CAT         (CONFIG_ITEM_STRING | 0x07)  /* Module option */
#define CONFIG_ITEM_LOADFILE                (CONFIG_ITEM_STRING | 0x0C)  /* Read file option */
#define CONFIG_ITEM_SAVEFILE                (CONFIG_ITEM_STRING | 0x0D)  /* Written file option */
#define CONFIG_ITEM_DIRECTORY               (CONFIG_ITEM_STRING | 0x0E)  /* Directory option */
#define CONFIG_ITEM_FONT                    (CONFIG_ITEM_STRING | 0x0F)  /* Font option */

/* reduce specific type to type class */
#define CONFIG_CLASS(x) ((x) & ~0x1F)

/* is proper option, not a special hint type? */
#define CONFIG_ITEM(x) (((x) & ~0xF) != 0)

#define IsConfigStringType(type) \
    (((type) & CONFIG_ITEM_STRING) != 0)
#define IsConfigIntegerType(type) \
    (((type) & CONFIG_ITEM_INTEGER) != 0)
#define IsConfigFloatType(type) \
    ((type) == CONFIG_ITEM_FLOAT)

#define VAR_TYPE                0x00ff
#define VAR_CLASS               0x00f0
#define VAR_FLAGS               0xff00
#define VAR_VOID                0x0010
#define VAR_BOOL                0x0020
#define VAR_INTEGER             0x0030
#define VAR_STRING              0x0040
#define VAR_FLOAT               0x0050
#define VAR_ADDRESS             0x0070
#define VAR_COORDS              0x00A0
#define VAR_HASCHOICE           0x0100
#define VAR_ISCOMMAND           0x2000
#define VAR_DOINHERIT           0x8000
#define VAR_SETSTEP             0x0012
#define VAR_SETVALUE            0x0013
#define VAR_SETTEXT             0x0014
#define VAR_GETTEXT             0x0015
#define VAR_GETMIN              0x0016
#define VAR_GETMAX              0x0017
#define VAR_GETSTEP             0x0018
#define VAR_ADDCHOICE           0x0020
#define VAR_DELCHOICE           0x0021
#define VAR_CLEARCHOICES        0x0022
#define VAR_GETCHOICES          0x0024
#define VAR_CHOICESCOUNT        0x0026
#define VAR_SETMINMAX           0x0027
