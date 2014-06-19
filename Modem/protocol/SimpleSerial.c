#include <string.h>
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "protocol/SimpleSerial.h"

bool PRINT_SRC = true;
bool PRINT_DST = true;
bool PRINT_PATH = true;
bool PRINT_DATA = true;
bool PRINT_INFO = true;
bool VERBOSE = true;
bool SILENT = false;
bool SS_INIT = false;
bool SS_DEFAULT_CONF = false;

AX25Call src;
AX25Call dst;
AX25Call path1;
AX25Call path2;

char CALL[6] = DEFAULT_CALLSIGN;
int CALL_SSID = 0;
char DST[6] = DEFAULT_DESTINATION_CALL;
int DST_SSID = 0;
char PATH1[6] = "WIDE1";
int PATH1_SSID = 1;
char PATH2[6] = "WIDE2";
int PATH2_SSID = 2;

AX25Call path[4];

#define NV_MAGIC_BYTE 0x69
uint8_t EEMEM nvMagicByte;
uint8_t EEMEM nvCALL[6];
uint8_t EEMEM nvDST[6];
uint8_t EEMEM nvPATH1[6];
uint8_t EEMEM nvPATH2[6];
uint8_t EEMEM nvCALL_SSID;
uint8_t EEMEM nvDST_SSID;
uint8_t EEMEM nvPATH1_SSID;
uint8_t EEMEM nvPATH2_SSID;
bool EEMEM nvPRINT_SRC;
bool EEMEM nvPRINT_DST;
bool EEMEM nvPRINT_PATH;
bool EEMEM nvPRINT_DATA;
bool EEMEM nvPRINT_INFO;
bool EEMEM nvVERBOSE;
bool EEMEM nvSILENT;

// Packet assembly fields
char message_recip[6];
int message_recip_ssid;

char latitude[9];
char longtitude[9];

int power;
int height;
int gain;

/////////////////////////

void ss_init(void) {
    ss_loadSettings();
    SS_INIT = true;
    if (VERBOSE) {
        _delay_ms(300);
        kprintf("---------------\n");
        kprintf("MicroAPRS v0.2a\n");
        kprintf("unsigned.io/microaprs\n");
        if (SS_DEFAULT_CONF) kprintf("Default configuration loaded!\n");
        kprintf("Modem ready\n");
        kprintf("---------------\n");
    }
}

void ss_clearSettings(void) {
    eeprom_update_byte((void*)&nvMagicByte, 0xFF);
    if (VERBOSE) kprintf("Configuration cleared\n");
    if (!VERBOSE && !SILENT) kprintf("1\n");
}

void ss_loadSettings(void) {
    uint8_t verification = eeprom_read_byte((void*)&nvMagicByte);
    if (verification == NV_MAGIC_BYTE) {
        eeprom_read_block((void*)CALL, (void*)nvCALL, 6);
        eeprom_read_block((void*)DST, (void*)nvDST, 6);
        eeprom_read_block((void*)PATH1, (void*)nvPATH1, 6);
        eeprom_read_block((void*)PATH2, (void*)nvPATH2, 6);

        CALL_SSID = eeprom_read_byte((void*)&nvCALL_SSID);
        DST_SSID = eeprom_read_byte((void*)&nvDST_SSID);
        PATH1_SSID = eeprom_read_byte((void*)&nvPATH1_SSID);
        PATH2_SSID = eeprom_read_byte((void*)&nvPATH2_SSID);

        PRINT_SRC = eeprom_read_byte((void*)&nvPRINT_SRC);
        PRINT_DST = eeprom_read_byte((void*)&nvPRINT_DST);
        PRINT_PATH = eeprom_read_byte((void*)&nvPRINT_PATH);
        PRINT_DATA = eeprom_read_byte((void*)&nvPRINT_DATA);
        PRINT_INFO = eeprom_read_byte((void*)&nvPRINT_INFO);
        VERBOSE = eeprom_read_byte((void*)&nvVERBOSE);
        SILENT = eeprom_read_byte((void*)&nvSILENT);

        if (VERBOSE && SS_INIT) kprintf("Configuration loaded\n");
    } else {
        if (SS_INIT && !SILENT && VERBOSE) kprintf("Error: No stored configuration to load!\n");
        if (SS_INIT && !SILENT && !VERBOSE) kprintf("0\n");
        SS_DEFAULT_CONF = true;
    }
}

void ss_saveSettings(void) {
    eeprom_update_block((void*)CALL, (void*)nvCALL, 6);
    eeprom_update_block((void*)DST, (void*)nvDST, 6);
    eeprom_update_block((void*)PATH1, (void*)nvPATH1, 6);
    eeprom_update_block((void*)PATH2, (void*)nvPATH2, 6);

    eeprom_update_byte((void*)&nvCALL_SSID, CALL_SSID);
    eeprom_update_byte((void*)&nvDST_SSID, DST_SSID);
    eeprom_update_byte((void*)&nvPATH1_SSID, PATH1_SSID);
    eeprom_update_byte((void*)&nvPATH2_SSID, PATH2_SSID);

    eeprom_update_byte((void*)&nvPRINT_SRC, PRINT_SRC);
    eeprom_update_byte((void*)&nvPRINT_DST, PRINT_DST);
    eeprom_update_byte((void*)&nvPRINT_PATH, PRINT_PATH);
    eeprom_update_byte((void*)&nvPRINT_DATA, PRINT_DATA);
    eeprom_update_byte((void*)&nvPRINT_INFO, PRINT_INFO);
    eeprom_update_byte((void*)&nvVERBOSE, VERBOSE);
    eeprom_update_byte((void*)&nvSILENT, SILENT);

    eeprom_update_byte((void*)&nvMagicByte, NV_MAGIC_BYTE);

    if (VERBOSE) kprintf("Configuration saved\n");
    if (!VERBOSE && !SILENT) kprintf("1\n");
}

void ss_printSettings(void) {
    kprintf("Configuration:\n");
    kprintf("Callsign: %.6s-%d\n", CALL, CALL_SSID);
    kprintf("Destination: %.6s-%d\n", DST, DST_SSID);
    kprintf("Path1: %.6s-%d\n", PATH1, PATH1_SSID);
    kprintf("Path2: %.6s-%d\n", PATH2, PATH2_SSID);
}

void ss_messageCallback(struct AX25Msg *msg, Serial *ser) {
    if (PRINT_SRC) {
        if (PRINT_INFO) kfile_print(&ser->fd, "SRC: ");
        kfile_printf(&ser->fd, "[%.6s-%d] ", msg->src.call, msg->src.ssid);
    }
    if (PRINT_DST) {
        if (PRINT_INFO) kfile_printf(&ser->fd, "DST: ");
        kfile_printf(&ser->fd, "[%.6s-%d] ", msg->dst.call, msg->dst.ssid);
    }

    if (PRINT_PATH) {
        if (PRINT_INFO) kfile_print(&ser->fd, "PATH: ");
        for (int i = 0; i < msg->rpt_cnt; i++)
            kfile_printf(&ser->fd, "[%.6s-%d] ", msg->rpt_lst[i].call, msg->rpt_lst[i].ssid);
    }
    
    if (PRINT_DATA) {
        if (PRINT_INFO) kfile_print(&ser->fd, "DATA: ");
        kfile_printf(&ser->fd, "%.*s", msg->len, msg->info);
    }
    kfile_print(&ser->fd, "\r\n");
}

void ss_serialCallback(void *_buffer, size_t length, Serial *ser, AX25Ctx *ctx) {
    uint8_t *buffer = (uint8_t *)_buffer;
    if (length > 0) {
        // ! as first char to send packet
        if (buffer[0] == '!' && length > 1) {
            buffer++; length--;
            ss_sendMsg(buffer, length, ctx);
        } else if (buffer[0] == 'h') {
            ss_printHelp();
        } else if (buffer[0] == 'H') {
            ss_printSettings();
        } else if (buffer[0] == 'S') {
            ss_saveSettings();
        } else if (buffer[0] == 'C') {
            ss_clearSettings();
        } else if (buffer[0] == 'L') {
            ss_loadSettings();
        } else if (buffer[0] == 'c' && length > 3) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    CALL[count] = c;
                } else {
                    CALL[count] = 0x00;
                }
                count++;
            }
            while (count < 6) {
                CALL[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Callsign: %.6s-%d\n", CALL, CALL_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");

        } else if (buffer[0] == 'd' && length > 3) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    DST[count] = c;
                } else {
                    DST[count] = 0;
                }
                count++;
            }
            while (count < 6) {
                DST[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Destination: %.6s-%d\n", DST, DST_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");


        } else if (buffer[0] == '1' && length > 1) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    PATH1[count] = c;
                } else {
                    PATH1[count] = 0;
                }
                count++;
            }
            while (count < 6) {
                PATH1[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Path1: %.6s-%d\n", PATH1, PATH1_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");


        } else if (buffer[0] == '2' && length > 1) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    PATH2[count] = c;
                } else {
                    PATH2[count] = 0;
                }
                count++;
            }
            while (count < 6) {
                PATH2[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Path2: %.6s-%d\n", PATH2, PATH2_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");


        } else if (buffer[0] == 's' && length > 2) {
            buffer++; length--;
            if (buffer[0] == 'c') {
                if (length > 2) {
                    CALL_SSID = 10+buffer[2]-48;
                } else {
                    CALL_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Callsign: %.6s-%d\n", CALL, CALL_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            if (buffer[0] == 'd') {
                if (length > 2) {
                    DST_SSID = 10+buffer[2]-48;
                } else {
                    DST_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Destination: %.6s-%d\n", DST, DST_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            if (buffer[0] == '1') {
                if (length > 2) {
                    PATH1_SSID = 10+buffer[2]-48;
                } else {
                    PATH1_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Path1: %.6s-%d\n", PATH1, PATH1_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            if (buffer[0] == '2') {
                if (length > 2) {
                    PATH2_SSID = 10+buffer[2]-48;
                } else {
                    PATH2_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Path2: %.6s-%d\n", PATH2, PATH2_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            
        } else if (buffer[0] == 'p' && length > 2) {
            buffer++; length--;
            if (buffer[0] == 's') {
                if (buffer[1] == 49) {
                    PRINT_SRC = true;
                    if (VERBOSE) kprintf("Print SRC enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_SRC = false;
                    if (VERBOSE) kprintf("Print SRC disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'd') {
                if (buffer[1] == 49) {
                    PRINT_DST = true;
                    if (VERBOSE) kprintf("Print DST enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_DST = false;
                    if (VERBOSE) kprintf("Print DST disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'p') {
                if (buffer[1] == 49) {
                    PRINT_PATH = true;
                    if (VERBOSE) kprintf("Print PATH enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_PATH = false;
                    if (VERBOSE) kprintf("Print PATH disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'm') {
                if (buffer[1] == 49) {
                    PRINT_DATA = true;
                    if (VERBOSE) kprintf("Print DATA enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_DATA = false;
                    if (VERBOSE) kprintf("Print DATA disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'i') {
                if (buffer[1] == 49) {
                    PRINT_INFO = true;
                    if (VERBOSE) kprintf("Print INFO enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_INFO = false;
                    if (VERBOSE) kprintf("Print INFO disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
        } else if (buffer[0] == 'v') {
            if (buffer[1] == 49) {
                VERBOSE = true;
                kfile_printf(&ser->fd, "Verbose mode enabled\n");
            } else {
                VERBOSE = false;
                kfile_printf(&ser->fd, "Verbose mode disabled\n");
            }
        } else if (buffer[0] == 'V') {
            if (buffer[1] == 49) {
                SILENT = true;
                VERBOSE = false;
                kfile_printf(&ser->fd, "Silent mode enabled\n");
            } else {
                SILENT = false;
                kfile_printf(&ser->fd, "Silent mode disabled\n");
            }
        } else {
            if (VERBOSE) kprintf("Error: Invalid command\n");
            if (!VERBOSE && !SILENT) kprintf("0\n");
        }

    }

}

void ss_sendMsg(void *_buffer, size_t length, AX25Ctx *ax25) {

    uint8_t *buffer = (uint8_t *)_buffer;

    memcpy(dst.call, DST, 6);
    dst.ssid = DST_SSID;

    memcpy(src.call, CALL, 6);
    src.ssid = CALL_SSID;

    memcpy(path1.call, PATH1, 6);
    path1.ssid = PATH1_SSID;

    memcpy(path2.call, PATH2, 6);
    path2.ssid = PATH2_SSID;

    path[0] = dst;
    path[1] = src;
    path[2] = path1;
    path[3] = path2;

    ax25_sendVia(ax25, path, countof(path), buffer, length);
}

void ss_printSrc(bool val) {
    PRINT_SRC = val;
}

void ss_printDst(bool val) {
    PRINT_DST = val;
}

void ss_printPath(bool val) {
    PRINT_PATH = val;
}

void ss_printData(bool val) {
    PRINT_DATA = val;
}

void ss_printInfo(bool val) {
    PRINT_INFO = val;
}

void ss_printHelp(void) {
    kprintf("----------------------------------\n");
    kprintf("Serial commands:\n");
    kprintf("!<msg>    Send packet\n");
    kprintf("c<call>   Set your callsign\n");
    kprintf("d<call>   Set destination callsign\n");
    kprintf("1<call>   Set PATH1 callsign\n");
    kprintf("2<call>   Set PATH2 callsign\n");
    kprintf("sc<ssid>  Set your SSID\n");
    kprintf("sd<ssid>  Set destination SSID\n");
    kprintf("s1<ssid>  Set PATH1 SSID\n");
    kprintf("s2<ssid>  Set PATH2 SSID\n");
    kprintf("ps<1/0>   Print SRC on/off\n");
    kprintf("pd<1/0>   Print DST on/off\n");
    kprintf("pp<1/0>   Print PATH on/off\n");
    kprintf("pm<1/0>   Print DATA on/off\n");
    kprintf("pi<1/0>   Print INFO on/off\n");
    kprintf("v<1/0>    Verbose mode on/off\n");
    kprintf("V<1/0>    Silent mode on/off\n");
    kprintf("S         Save configuration\n");
    kprintf("L         Load configuration\n");
    kprintf("C         Clear configuration\n");
    kprintf("H         Print configuration\n");
    kprintf("----------------------------------\n");
}