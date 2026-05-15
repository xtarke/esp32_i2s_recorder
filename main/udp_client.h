/**
 * @file wifi_sta.h
 * @author Renan Augusto Starke
 * @date 15 Mar 2026
 * @brief  File containing wifi station public functions.
 *
 */

#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

void setup_udp_client(void);
void udp_data_send(const char * bufs, size_t count);

#endif //__UDP_CLIENT_H__