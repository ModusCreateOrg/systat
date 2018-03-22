#ifndef SYSTAT_H
#define SYSTAT_H

// global header file for systat program

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_mib.h>
#include <net/if_types.h>
#include <net/if_var.h>
#include <net/route.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/types.h>

#include <errno.h>
#include <mach/mach_host.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <unistd.h>

#include <locale.h>
#include <assert.h>

#include "Console.h"
#include "Platform.h"
#include "Processor.h"
#include "Memory.h"
#include "Network.h"

extern void printNetwork();

#endif
