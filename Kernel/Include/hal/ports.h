#pragma once
#include <lib/types.h>

void port_wait(int iters);

uint8_t port_inb(uint16_t port);
/// @brief Read 16-bit value from IO port @param port Port number @return Data from port
uint16_t port_inw(uint16_t port);
/// @brief Read 32-bit value from IO port @param port Port number @return Data from port
uint32_t port_inl(uint16_t port);
/// @brief Read string from data from IO port @param port Port number @param buff Pointer to destination @param size Size in bytes
void port_insw(uint16_t port, uint8_t* buff, size_t size);

/// @brief Write 8-bit value to IO port @param port Port number @param data Data value
void port_outb(uint16_t port, uint8_t data);
/// @brief Write 16-bit value to IO port @param port Port number @param data Data value
void port_outw(uint16_t port, uint16_t data);
/// @brief Write 32-bit value to IO port @param port Port number @param data Data value
void port_outl(uint16_t port, uint32_t data);
/// @brief Write string to data from IO port @param port Port number @param buff Pointer to source @param size Size in bytes
void port_outsw(uint16_t port, uint8_t* data, size_t size);