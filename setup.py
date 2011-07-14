from distutils.core import setup, Extension
import platform
import sys

if platform.system() == 'Windows':
    if platform.architecture()[0] == '64bit':
        print "ERROR: 64-bit Windows not available"
        sys.exit(-1)
    VMWARE_VIX = r'c:\Program Files (x86)\VMware\VMware VIX'
    VMWARE_INC = VMWARE_VIX
    VMWARE_LIB = VMWARE_VIX
    VMWARE_BIN = 'VixAllProducts'
    OS_LIBS = ['ws2_32',   'user32', 
               'kernel32', 'advapi32', 
               'ole32',    'oleaut32',
               'shell32']
else:
    # linux things
    VMWARE_INC = r''
    VMWARE_LIB = r''
    VMWARE_BIN = r''
    OS_LIBS = []



setup(
    name='vixpy',
    version='0.1.0',
    ext_modules = [
        Extension("_vixpy",
            sources=['vixpy.c'],
            include_dirs = [VMWARE_INC],
            library_dirs = [VMWARE_LIB],
            libraries = [VMWARE_BIN] + OS_LIBS
            )
    ],
    py_modules=['vixpy'],
)

