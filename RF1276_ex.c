/*
 * RF1276.c
 *
 *  Created on: Sep 8, 2015
 *      Author: cuki
 */

#include "RF1276.h"

#include <stdlib.h>
#include "serial.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define RF1276_DATA_SIZE 			12
#define RF1276_DATA_SIZE_RSSI		2
#define RF1276_COMMAND_SIZE			23
#define RF1276_COMMAND_SIZE_RSSI	13
#define RF1276_HEADER_SIZE			8

uint8_t RF1276_crc(uint8_t *data, uint16_t size) {
    uint16_t acumulo, cont;

    acumulo = 0;
    for (cont = 0; cont < size; ++cont)
        acumulo += data[cont];

    return (uint8_t) acumulo % 256;
}

uint8_t *RF1276_make_radio_request(command_yy_t cmd_type, uint8_t *data,
        uint16_t size) {

    uint8_t *ret;
    uint8_t aux[size + 8];
    uint16_t cont;

    ret = NULL;
    ret = (uint8_t *) malloc((size + 11) * sizeof(uint8_t));

    if (ret == NULL) {
        fprintf(stderr, "Out of memor\n");
        return NULL;
    }

    ret[0] = 0xAF;
    ret[1] = 0xAF;
    ret[2] = 0x00;
    ret[3] = 0x00;
    ret[4] = 0xAF;
    ret[5] = (uint8_t) CMD_XX_SENDING;
    ret[6] = (uint8_t) cmd_type;
    ret[7] = (uint8_t) size;

    for (cont = 0; cont < size; ++cont)
        ret[cont + 8] = data[cont];

    free(data);

    for (cont = 0; cont < sizeof(aux); ++cont)
        aux[cont] = ret[cont];

    ret[size + 8] = RF1276_crc(aux, sizeof(aux));
    ret[size + 9] = 0x0D;
    ret[size + 10] = 0x0A;

    return ret;
}

uint8_t *RF1276_make_radio_read_command(uint16_t size) {
    uint16_t cont;
    uint8_t *data;

    data = NULL;
    data = (uint8_t *) malloc(size * sizeof(uint8_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    for (cont = 0; cont < size; ++cont)
        data[cont] = 0;

    return RF1276_make_radio_request(CMD_READ, data, size);
}

uint8_t *RF1276_make_radio_rssi_command(uint16_t size) {
    uint16_t cont;
    uint8_t *data;

    data = NULL;
    data = (uint8_t *) malloc(size * sizeof(uint8_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    for (cont = 0; cont < size; ++cont)
        data[cont] = 0;

    return RF1276_make_radio_request(CMD_RSSI, data, size);
}

int RF1276_make_radio_read_transaction(int fd, uint8_t *response) {
    uint8_t *request;
    uint16_t cont;

    request = RF1276_make_radio_read_command(RF1276_DATA_SIZE);

    if (serial_transaction(fd, request, response, RF1276_COMMAND_SIZE,
    RF1276_COMMAND_SIZE) == -1) {
        free(request);
        fprintf(stderr, "Serial transaction problem\n");
        return EXIT_FAILURE;
    }

    for (cont = 0; cont < 8; ++cont) {

        if (cont != 5 && response[cont] != request[cont]) {
            free(request);
            fprintf(stderr, "Wrong response\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int RF1276_make_radio_read_rssi_transaction(int fd, uint8_t *response) {
    uint8_t *request;
    uint16_t cont;

    request = RF1276_make_radio_rssi_command(RF1276_DATA_SIZE_RSSI);

    if (serial_transaction(fd, request, response, RF1276_COMMAND_SIZE_RSSI,
    RF1276_COMMAND_SIZE_RSSI) == 0) {
        free(request);
        fprintf(stderr, "Serial transaction problem\n");
        return EXIT_FAILURE;
    }

    for (cont = 0; cont < 8; ++cont) {

        if (cont != 5 && response[cont] != request[cont]) {
            free(request);
            fprintf(stderr, "Wrong response\n");
            return EXIT_FAILURE;
        }
    }

    free(request);
    return EXIT_SUCCESS;
}

float RF1276_uchartofreq(uint8_t freq_hl, uint8_t freq_lh, uint8_t freq_ll) {
    uint32_t aux = 0;
    float ret = 0;

    aux = 0;
    aux = freq_hl << 16;
    aux |= freq_lh << 8;
    aux |= freq_ll;

    ret = (float) aux * 61.035;

    return ret;
}

int RF1276_get_radio_data(int fd, radio_data_t *data) {
    uint8_t *response;

    response = NULL;
    response = (uint8_t *) malloc(RF1276_COMMAND_SIZE * sizeof(uint8_t));

    if (response == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    if (RF1276_make_radio_read_transaction(fd, response) == EXIT_FAILURE) {
        free(response);
        fprintf(stderr, "No radio response\n");
        return EXIT_FAILURE;
    }

    data->baudrate = (baud_rate_t) response[RF1276_HEADER_SIZE + 0];
    data->parity = (parity_t) response[RF1276_HEADER_SIZE + 1];
    data->frequency = RF1276_uchartofreq(response[RF1276_HEADER_SIZE + 2],
            response[RF1276_HEADER_SIZE + 3], response[RF1276_HEADER_SIZE + 4]);
    data->rf_factor = (rf_factor_t) response[RF1276_HEADER_SIZE + 5];
    data->mode = (radio_mode_t) response[RF1276_HEADER_SIZE + 6];
    data->rf_bw = (rf_bw_t) response[RF1276_HEADER_SIZE + 7];
    data->id = (response[RF1276_HEADER_SIZE + 8] << 8)
            & response[RF1276_HEADER_SIZE + 9];
    data->net_id = response[RF1276_HEADER_SIZE + 10];
    data->rf_power = (rf_power_t) response[RF1276_HEADER_SIZE + 11];

    return EXIT_SUCCESS;
}

signed int RF1276_get_radio_rssi(int fd) {
    uint8_t *m_r;
    signed int ret;

    m_r = NULL;
    m_r = (uint8_t *) malloc(RF1276_COMMAND_SIZE_RSSI * sizeof(uint8_t));

    if (m_r == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    if (RF1276_make_radio_read_rssi_transaction(fd, m_r) == EXIT_FAILURE) {
        free(m_r);
        fprintf(stderr, "No radio response\n");
        return EXIT_FAILURE;
    }

    ret = 0;
    ret = -164 + m_r[8];

    free(m_r);
    return ret;
}

uint8_t RF1276_touchar(int in, int index) {
    int mask, aux;

    mask = 0xFF << index * 8;
    aux = in & mask;
    aux >>= index * 8;
    aux &= 0xFF;

    return (uint8_t) aux;
}

uint8_t *RF1276_freqtouchar(float frequencie) {
    uint8_t *ret, cont;
    uint32_t aux;

    ret = NULL;
    ret = (uint8_t *) malloc(3 * sizeof(uint8_t));

    if (ret == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    aux = (uint32_t) ((float) frequencie / 61.035);

    for (cont = 0; cont < 3; ++cont)
        ret[cont] = RF1276_touchar(aux, cont);

    return ret;
}

uint8_t *RF1276_make_radio_write_command(radio_data_t *data) {
    uint8_t *aux, *m_freq;

    aux = NULL;
    aux = (uint8_t *) malloc(RF1276_DATA_SIZE * sizeof(uint8_t));

    if (aux == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    m_freq = NULL;
    m_freq = RF1276_freqtouchar(data->frequency);

    aux[0] = (uint8_t) data->baudrate;
    aux[1] = (uint8_t) data->parity;
    aux[2] = m_freq[2];
    aux[3] = m_freq[1];
    aux[4] = m_freq[0];
    aux[5] = (uint8_t) data->rf_factor;
    aux[6] = (uint8_t) data->mode;
    aux[7] = (uint8_t) data->rf_bw;
    aux[8] = ((data->id & 0xFF00) >> 8) & 0xFF;
    aux[9] = data->id & 0xFF;
    aux[10] = data->net_id;
    aux[11] = (uint8_t) data->rf_power;

    free(m_freq);

    return RF1276_make_radio_request(CMD_WRITE, aux, RF1276_DATA_SIZE);
}

int RF1276_make_radio_write_transaction(int fd, radio_data_t *data) {
    uint8_t *request, *response, cont;

    response = (uint8_t *) malloc(RF1276_COMMAND_SIZE * sizeof(uint8_t));
    request = RF1276_make_radio_write_command(data);

    if (request == NULL || response == NULL) {
        free(request);
        free(response);
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    if (serial_transaction(fd, request, response, RF1276_COMMAND_SIZE,
    RF1276_COMMAND_SIZE) == 0) {
        free(request);
        free(response);
        fprintf(stderr, "No radio response\n");
        return EXIT_FAILURE;
    }

    for (cont = 0; cont < 8; ++cont)
        if (cont != 5 && response[cont] != request[cont]) {
            free(response);
            fprintf(stderr, "Wrong radio response\n");
            return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;
}

int RF1276_write_radio_baudrate(int fd, baud_rate_t baudrate) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->baudrate = baudrate;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_parity(int fd, parity_t parity) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->parity = parity;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_frequencie(int fd, float frequencie) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->frequency = frequencie;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_rf_factor(int fd, rf_factor_t rf_factor) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->rf_factor = rf_factor;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_mode(int fd, radio_mode_t mode) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->mode = mode;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_rf_bw(int fd, rf_bw_t rf_bw) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->rf_bw = rf_bw;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_id(int fd, uint16_t id) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->id = id;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_net_id(int fd, uint8_t net_id) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    usleep(10000);
    data->net_id = net_id;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_write_radio_rf_power(int fd, rf_power_t power) {
    radio_data_t *data;
    int r;

    data = NULL;
    data = (radio_data_t *) malloc(sizeof(radio_data_t));

    if (data == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    r = EXIT_FAILURE;
    r = RF1276_get_radio_data(fd, data);

    if (r == EXIT_FAILURE) {
        free(data);
        fprintf(stderr, "No radio response or wrong response\n");
        return EXIT_FAILURE;
    }

    data->rf_power = power;
    r = EXIT_FAILURE;
    r = RF1276_make_radio_write_transaction(fd, data);
    free(data);

    return r;
}

int RF1276_parse_radio(radio_data_t data) {
    char m_str[20];

    printf("Frequencia: %f\n", data.frequency);
    memset(m_str, 0, sizeof(m_str));

    switch (data.rf_factor) {
    case RF_128:
        strcpy(m_str, "128");
        break;
    case RF_256:
        strcpy(m_str, "256");
        break;
    case RF_512:
        strcpy(m_str, "512");
        break;
    case RF_1024:
        strcpy(m_str, "1024");
        break;
    case RF_2048:
        strcpy(m_str, "2048");
        break;
    case RF_4096:
        strcpy(m_str, "4096");
        break;
    }

    printf("RF_Factor: %s chips\n", m_str);
    memset(m_str, 0, sizeof(m_str));

    switch (data.mode) {
    case MODE_STANDARD:
        strcpy(m_str, "STANDARD");
        break;
    case MODE_SLEEP:
        strcpy(m_str, "SLEEP");
        break;
    case MODE_LOW_POWER:
        strcpy(m_str, "LOW POWER");
        break;
    }

    printf("RF_Mode: %s\n", m_str);
    memset(m_str, 0, sizeof(m_str));

    switch (data.rf_bw) {
    case BW_125K:
        strcpy(m_str, "125");
        break;
    case BW_250K:
        strcpy(m_str, "250");
        break;
    case BW_500K:
        strcpy(m_str, "500");
        break;
    case BW_62_5K:
        strcpy(m_str, "62.5");
        break;
    }

    printf("RF_BW: %s Kbs\n", m_str);
    memset(m_str, 0, sizeof(m_str));
    printf("Node ID: %u\n", data.id);
    printf("Net ID: %u\n", data.net_id);

    switch (data.rf_power) {
    case P_4DBM:
        strcpy(m_str, "1 (4dBm)");
        break;
    case P_7DBM:
        strcpy(m_str, "2 (7dBm)");
        break;
    case P_10DBM:
        strcpy(m_str, "3 (10dBm)");
        break;
    case P_13DBM:
        strcpy(m_str, "4 (13dBm)");
        break;
    case P_14DBM:
        strcpy(m_str, "5 (14dBm)");
        break;
    case P_17DBM:
        strcpy(m_str, "6 (17dBm)");
        break;
    case P_20DBM:
        strcpy(m_str, "7 (20dBm)");
        break;
    }

    printf("Power: %s\n", m_str);
    memset(m_str, 0, sizeof(m_str));

    switch (data.baudrate) {
    case B1200BPS:
        strcpy(m_str, "1200");
        break;
    case B2400BPS:
        strcpy(m_str, "2400");
        break;
    case B4800BPS:
        strcpy(m_str, "4800");
        break;
    case B9600BPS:
        strcpy(m_str, "9600");
        break;
    case B19200BPS:
        strcpy(m_str, "19200");
        break;
    case B38400BPS:
        strcpy(m_str, "38400");
        break;
    case B57600BPS:
        strcpy(m_str, "57600");
        break;
    case B115200PS:
        strcpy(m_str, "115200");
        break;
    }

    printf("Baud rate: %s bps", m_str);
    memset(m_str, 0, sizeof(m_str));

    switch (data.parity) {
    case NO_PARITY:
        strcpy(m_str, "8N1");
        break;
    case ODD_PARITY:
        strcpy(m_str, "8O1");
        break;
    case EVEN_PARITY:
        strcpy(m_str, "8E1");
        break;
    }

    printf(" %s\n", m_str);
    memset(m_str, 0, sizeof(m_str));

    return 0;
}
