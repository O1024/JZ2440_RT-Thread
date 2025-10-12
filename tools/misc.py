#
# File      : misc.py
# This file is part of RT-Thread RTOS
# COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Change Logs:
# Date           Author       Notes
# 2024-07-07     Chen Wen     The first version

import os
import subprocess

import rtconfig

def run_openocd():
    openocd_cfg = os.getenv('BSP_ROOT') + '/openocd_jz2440v3.cfg'
    openocd_cmd = ['openocd', '-f', openocd_cfg]

    openocd_process = subprocess.Popen(openocd_cmd, stderr=subprocess.PIPE)

    # read OpenOCD stderr
    while True:
        stderr_line = openocd_process.stderr.readline().decode('utf-8')
        if stderr_line == '' and openocd_process.poll() is not None:
            break
        if stderr_line:
            print(stderr_line.strip())

    openocd_process.wait()
