from distutils.core import setup, Extension
import platform
import sys
import os

#
# Set install path to VMWare VIX libraries
#
if platform.system() == 'Windows':
    VMWARE_VIX = r'c:\Program Files (x86)\VMware\VMware VIX'
    VMWARE_INC = VMWARE_VIX
    VMWARE_LIB = VMWARE_VIX
elif platform.system() == 'Darwin':
    VMWARE_VIX = '/Applications/VMware Fusion.app/Contents/Public'
    VMWARE_LIB = VMWARE_VIX
    VMWARE_INC = VMWARE_VIX + '/include'
else:
    VMWARE_LIB = '/usr/lib/vmware-vix'
    VMWARE_INC = '/usr/include/vmware-vix'


if platform.system() == 'Windows':
    if platform.architecture()[0] == '64bit':
        VMWARE_BIN = 'Vix64AllProducts'
    else:
        VMWARE_BIN = 'VixAllProducts'
    OS_LIBS = ['ws2_32',   'user32', 
               'kernel32', 'advapi32', 
               'ole32',    'oleaut32',
               'shell32']
else:
    # Linux
    VMWARE_BIN = 'vixAllProducts'
    OS_LIBS = []


setup(
    name='vixpy',
    version='0.1.2',
    ext_modules = [
        Extension("_vixpy",
            sources=['vixpy.c'],
            include_dirs = [VMWARE_INC],
            library_dirs = [VMWARE_LIB],
            libraries = [VMWARE_BIN] + OS_LIBS,
            )
    ],
    py_modules=['vixpy'],
)

