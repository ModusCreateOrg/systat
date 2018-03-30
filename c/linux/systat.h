#ifndef SYSTAT_H
#define SYSTAT_H

#define ENABLE_LOGGING
#undef ENABLE_LOGGING

#include <fcntl.h>
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "../common/Console.h"
#include "../common/Log.h"
#include "../common/Parser.h"

#include "Disk.h"
#include "Memory.h"
#include "Network.h"
#include "Platform.h"
#include "Processor.h"
#include "VirtualMemory.h"

#endif
