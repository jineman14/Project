#pragma once

#include <SDKDDKVer.h>

#include <deque>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <functional>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <Util/crc64.h>
#include <Util/WinLock.h>
#include "json/json.h"
#include "jdbc/mysql_driver.h"

#include "Protocol.h"