// Adapted from http://www.linuxhowtos.org/C_C++/socket.htm
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>

#include "networkInp.h"