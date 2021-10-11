import os
import platform
from ctypes import *
from enum import IntEnum, IntFlag

__all__ = ['VixHost',
           'VixVm',
           'VixException']

if platform.system() == 'Windows':
    VMWARE_VIX = r'C:\Program Files (x86)\VMware\VMware VIX'
    VMWARE_LIB = os.path.join(VMWARE_VIX, 'Vix64AllProductsDyn.dll')
elif platform.system() == 'Darwin':
    VMWARE_VIX = '/Applications/VMware Fusion.app/Contents/Public'
    VMWARE_LIB = os.path.join(VMWARE_VIX, 'libvixAllProducts.dylib')
else:
    VMWARE_LIB = '/usr/lib/vmware-vix/libvixAllProducts.so'


vix = CDLL(VMWARE_LIB)

def to_bytes(s):
    if isinstance(s, str):
        s = s.encode()
    return s

VixHandle = c_int
VixErrorType = c_ulonglong
VixEventProc = CFUNCTYPE(None, VixHandle, c_int, VixHandle, c_void_p)

VIX_INVALID_HANDLE              = 0
VIX_FILE_ATTRIBUTES_DIRECTORY   = 0x0001
VIX_FILE_ATTRIBUTES_SYMLINK     = 0x0002
VIX_API_VERSION                 = -1
VIX_FIND_RUNNING_VMS            = 1
VIX_EVENTTYPE_FIND_ITEM         = 8
VIX_VMOPEN_NORMAL               = 0x0
VIX_LOGIN_IN_GUEST_REQUIRE_INTERACTIVE_ENVIRONMENT      = 0x08
VIX_RUNPROGRAM_RETURN_IMMEDIATELY   = 0x0001
VIX_RUNPROGRAM_ACTIVATE_WINDOW      = 0x0002
VIX_VM_GUEST_VARIABLE            = 1
VIX_VM_CONFIG_RUNTIME_ONLY       = 2
VIX_GUEST_ENVIRONMENT_VARIABLE   = 3
VIX_SNAPSHOT_REMOVE_CHILDREN     = 0x0001
VIX_SNAPSHOT_INCLUDE_MEMORY      = 0x0002
VIX_SHAREDFOLDER_WRITE_ACCESS    = 0x04
VIX_CAPTURESCREENFORMAT_PNG            = 0x01
VIX_CAPTURESCREENFORMAT_PNG_NOCOMPRESS = 0x02
VIX_CLONETYPE_FULL       = 0
VIX_CLONETYPE_LINKED     = 1
VIX_INSTALLTOOLS_MOUNT_TOOLS_INSTALLER = 0x00
VIX_INSTALLTOOLS_AUTO_UPGRADE          = 0x01
VIX_INSTALLTOOLS_RETURN_IMMEDIATELY    = 0x02
VIX_VMDELETE_DISK_FILES     = 0x0002

class VixException(Exception):
    def __init__(self, error):
        self.error = VIX_ERROR_CODE(error)
        self.message = Vix_GetErrorText(self.error)

    def __str__(self):
        return f'{self.error} -> {self.message}'

class VixError(IntEnum):
    VIX_OK                                       = 0

    VIX_E_FAIL                                   = 1
    VIX_E_OUT_OF_MEMORY                          = 2
    VIX_E_INVALID_ARG                            = 3
    VIX_E_FILE_NOT_FOUND                         = 4
    VIX_E_OBJECT_IS_BUSY                         = 5
    VIX_E_NOT_SUPPORTED                          = 6
    VIX_E_FILE_ERROR                             = 7
    VIX_E_DISK_FULL                              = 8
    VIX_E_INCORRECT_FILE_TYPE                    = 9
    VIX_E_CANCELLED                              = 10
    VIX_E_FILE_READ_ONLY                         = 11
    VIX_E_FILE_ALREADY_EXISTS                    = 12
    VIX_E_FILE_ACCESS_ERROR                      = 13
    VIX_E_REQUIRES_LARGE_FILES                   = 14
    VIX_E_FILE_ALREADY_LOCKED                    = 15
    VIX_E_VMDB                                   = 16
    VIX_E_NOT_SUPPORTED_ON_REMOTE_OBJECT         = 20
    VIX_E_FILE_TOO_BIG                           = 21
    VIX_E_FILE_NAME_INVALID                      = 22
    VIX_E_ALREADY_EXISTS                         = 23
    VIX_E_BUFFER_TOOSMALL                        = 24
    VIX_E_OBJECT_NOT_FOUND                       = 25
    VIX_E_HOST_NOT_CONNECTED                     = 26
    VIX_E_INVALID_UTF8_STRING                    = 27
    VIX_E_OPERATION_ALREADY_IN_PROGRESS          = 31
    VIX_E_UNFINISHED_JOB                         = 29
    VIX_E_NEED_KEY                               = 30
    VIX_E_LICENSE                                = 32
    VIX_E_VM_HOST_DISCONNECTED                   = 34
    VIX_E_AUTHENTICATION_FAIL                    = 35
    VIX_E_HOST_CONNECTION_LOST                   = 36
    VIX_E_DUPLICATE_NAME                         = 41
    VIX_E_ARGUMENT_TOO_BIG                       = 44

    VIX_E_INVALID_HANDLE                         = 1000
    VIX_E_NOT_SUPPORTED_ON_HANDLE_TYPE           = 1001
    VIX_E_TOO_MANY_HANDLES                       = 1002

    VIX_E_NOT_FOUND                              = 2000
    VIX_E_TYPE_MISMATCH                          = 2001
    VIX_E_INVALID_XML                            = 2002

    VIX_E_TIMEOUT_WAITING_FOR_TOOLS              = 3000
    VIX_E_UNRECOGNIZED_COMMAND                   = 3001
    VIX_E_OP_NOT_SUPPORTED_ON_GUEST              = 3003
    VIX_E_PROGRAM_NOT_STARTED                    = 3004
    VIX_E_CANNOT_START_READ_ONLY_VM              = 3005
    VIX_E_VM_NOT_RUNNING                         = 3006
    VIX_E_VM_IS_RUNNING                          = 3007
    VIX_E_CANNOT_CONNECT_TO_VM                   = 3008
    VIX_E_POWEROP_SCRIPTS_NOT_AVAILABLE          = 3009
    VIX_E_NO_GUEST_OS_INSTALLED                  = 3010
    VIX_E_VM_INSUFFICIENT_HOST_MEMORY            = 3011
    VIX_E_SUSPEND_ERROR                          = 3012
    VIX_E_VM_NOT_ENOUGH_CPUS                     = 3013
    VIX_E_HOST_USER_PERMISSIONS                  = 3014
    VIX_E_GUEST_USER_PERMISSIONS                 = 3015
    VIX_E_TOOLS_NOT_RUNNING                      = 3016
    VIX_E_GUEST_OPERATIONS_PROHIBITED            = 3017
    VIX_E_ANON_GUEST_OPERATIONS_PROHIBITED       = 3018
    VIX_E_ROOT_GUEST_OPERATIONS_PROHIBITED       = 3019
    VIX_E_MISSING_ANON_GUEST_ACCOUNT             = 3023
    VIX_E_CANNOT_AUTHENTICATE_WITH_GUEST         = 3024
    VIX_E_UNRECOGNIZED_COMMAND_IN_GUEST          = 3025
    VIX_E_CONSOLE_GUEST_OPERATIONS_PROHIBITED    = 3026
    VIX_E_MUST_BE_CONSOLE_USER                   = 3027
    VIX_E_VMX_MSG_DIALOG_AND_NO_UI               = 3028
    VIX_E_OPERATION_NOT_ALLOWED_FOR_LOGIN_TYPE   = 3031
    VIX_E_LOGIN_TYPE_NOT_SUPPORTED               = 3032
    VIX_E_EMPTY_PASSWORD_NOT_ALLOWED_IN_GUEST    = 3033
    VIX_E_INTERACTIVE_SESSION_NOT_PRESENT        = 3034
    VIX_E_INTERACTIVE_SESSION_USER_MISMATCH      = 3035
    VIX_E_CANNOT_POWER_ON_VM                     = 3041
    VIX_E_NO_DISPLAY_SERVER                      = 3043
    VIX_E_TOO_MANY_LOGONS                        = 3046
    VIX_E_INVALID_AUTHENTICATION_SESSION         = 3047

    VIX_E_VM_NOT_FOUND                           = 4000
    VIX_E_NOT_SUPPORTED_FOR_VM_VERSION           = 4001
    VIX_E_CANNOT_READ_VM_CONFIG                  = 4002
    VIX_E_TEMPLATE_VM                            = 4003
    VIX_E_VM_ALREADY_LOADED                      = 4004
    VIX_E_VM_ALREADY_UP_TO_DATE                  = 4006
    VIX_E_VM_UNSUPPORTED_GUEST                   = 4011

    VIX_E_UNRECOGNIZED_PROPERTY                  = 6000
    VIX_E_INVALID_PROPERTY_VALUE                 = 6001
    VIX_E_READ_ONLY_PROPERTY                     = 6002
    VIX_E_MISSING_REQUIRED_PROPERTY              = 6003
    VIX_E_INVALID_SERIALIZED_DATA                = 6004
    VIX_E_PROPERTY_TYPE_MISMATCH                 = 6005

    VIX_E_BAD_VM_INDEX                           = 8000

    VIX_E_INVALID_MESSAGE_HEADER                 = 10000
    VIX_E_INVALID_MESSAGE_BODY                   = 10001

    VIX_E_SNAPSHOT_INVAL                         = 13000
    VIX_E_SNAPSHOT_DUMPER                        = 13001
    VIX_E_SNAPSHOT_DISKLIB                       = 13002
    VIX_E_SNAPSHOT_NOTFOUND                      = 13003
    VIX_E_SNAPSHOT_EXISTS                        = 13004
    VIX_E_SNAPSHOT_VERSION                       = 13005
    VIX_E_SNAPSHOT_NOPERM                        = 13006
    VIX_E_SNAPSHOT_CONFIG                        = 13007
    VIX_E_SNAPSHOT_NOCHANGE                      = 13008
    VIX_E_SNAPSHOT_CHECKPOINT                    = 13009
    VIX_E_SNAPSHOT_LOCKED                        = 13010
    VIX_E_SNAPSHOT_INCONSISTENT                  = 13011
    VIX_E_SNAPSHOT_NAMETOOLONG                   = 13012
    VIX_E_SNAPSHOT_VIXFILE                       = 13013
    VIX_E_SNAPSHOT_DISKLOCKED                    = 13014
    VIX_E_SNAPSHOT_DUPLICATEDDISK                = 13015
    VIX_E_SNAPSHOT_INDEPENDENTDISK               = 13016
    VIX_E_SNAPSHOT_NONUNIQUE_NAME                = 13017
    VIX_E_SNAPSHOT_MEMORY_ON_INDEPENDENT_DISK    = 13018
    VIX_E_SNAPSHOT_MAXSNAPSHOTS                  = 13019
    VIX_E_SNAPSHOT_MIN_FREE_SPACE                = 13020
    VIX_E_SNAPSHOT_HIERARCHY_TOODEEP             = 13021
    VIX_E_SNAPSHOT_NOT_REVERTABLE                = 13024

    VIX_E_HOST_DISK_INVALID_VALUE                = 14003
    VIX_E_HOST_DISK_SECTORSIZE                   = 14004
    VIX_E_HOST_FILE_ERROR_EOF                    = 14005
    VIX_E_HOST_NETBLKDEV_HANDSHAKE               = 14006
    VIX_E_HOST_SOCKET_CREATION_ERROR             = 14007
    VIX_E_HOST_SERVER_NOT_FOUND                  = 14008
    VIX_E_HOST_NETWORK_CONN_REFUSED              = 14009
    VIX_E_HOST_TCP_SOCKET_ERROR                  = 14010
    VIX_E_HOST_TCP_CONN_LOST                     = 14011
    VIX_E_HOST_NBD_HASHFILE_VOLUME               = 14012
    VIX_E_HOST_NBD_HASHFILE_INIT                 = 14013
    VIX_E_HOST_SERVER_SHUTDOWN                   = 14014
    VIX_E_HOST_SERVER_NOT_AVAILABLE              = 14015

    VIX_E_DISK_INVAL                             = 16000
    VIX_E_DISK_NOINIT                            = 16001
    VIX_E_DISK_NOIO                              = 16002
    VIX_E_DISK_PARTIALCHAIN                      = 16003
    VIX_E_DISK_NEEDSREPAIR                       = 16006
    VIX_E_DISK_OUTOFRANGE                        = 16007
    VIX_E_DISK_CID_MISMATCH                      = 16008
    VIX_E_DISK_CANTSHRINK                        = 16009
    VIX_E_DISK_PARTMISMATCH                      = 16010
    VIX_E_DISK_UNSUPPORTEDDISKVERSION            = 16011
    VIX_E_DISK_OPENPARENT                        = 16012
    VIX_E_DISK_NOTSUPPORTED                      = 16013
    VIX_E_DISK_NEEDKEY                           = 16014
    VIX_E_DISK_NOKEYOVERRIDE                     = 16015
    VIX_E_DISK_NOTENCRYPTED                      = 16016
    VIX_E_DISK_NOKEY                             = 16017
    VIX_E_DISK_INVALIDPARTITIONTABLE             = 16018
    VIX_E_DISK_NOTNORMAL                         = 16019
    VIX_E_DISK_NOTENCDESC                        = 16020
    VIX_E_DISK_NEEDVMFS                          = 16022
    VIX_E_DISK_RAWTOOBIG                         = 16024
    VIX_E_DISK_TOOMANYOPENFILES                  = 16027
    VIX_E_DISK_TOOMANYREDO                       = 16028
    VIX_E_DISK_RAWTOOSMALL                       = 16029
    VIX_E_DISK_INVALIDCHAIN                      = 16030
    VIX_E_DISK_KEY_NOTFOUND                      = 16052
    VIX_E_DISK_SUBSYSTEM_INIT_FAIL               = 16053
    VIX_E_DISK_INVALID_CONNECTION                = 16054
    VIX_E_DISK_ENCODING                          = 16061
    VIX_E_DISK_CANTREPAIR                        = 16062
    VIX_E_DISK_INVALIDDISK                       = 16063
    VIX_E_DISK_NOLICENSE                         = 16064
    VIX_E_DISK_NODEVICE                          = 16065
    VIX_E_DISK_UNSUPPORTEDDEVICE                 = 16066
    VIX_E_DISK_CAPACITY_MISMATCH                 = 16067
    VIX_E_DISK_PARENT_NOTALLOWED                 = 16068
    VIX_E_DISK_ATTACH_ROOTLINK                   = 16069

    VIX_E_CRYPTO_UNKNOWN_ALGORITHM               = 17000
    VIX_E_CRYPTO_BAD_BUFFER_SIZE                 = 17001
    VIX_E_CRYPTO_INVALID_OPERATION               = 17002
    VIX_E_CRYPTO_RANDOM_DEVICE                   = 17003
    VIX_E_CRYPTO_NEED_PASSWORD                   = 17004
    VIX_E_CRYPTO_BAD_PASSWORD                    = 17005
    VIX_E_CRYPTO_NOT_IN_DICTIONARY               = 17006
    VIX_E_CRYPTO_NO_CRYPTO                       = 17007
    VIX_E_CRYPTO_ERROR                           = 17008
    VIX_E_CRYPTO_BAD_FORMAT                      = 17009
    VIX_E_CRYPTO_LOCKED                          = 17010
    VIX_E_CRYPTO_EMPTY                           = 17011
    VIX_E_CRYPTO_KEYSAFE_LOCATOR                 = 17012

    VIX_E_CANNOT_CONNECT_TO_HOST                 = 18000
    VIX_E_NOT_FOR_REMOTE_HOST                    = 18001
    VIX_E_INVALID_HOSTNAME_SPECIFICATION         = 18002

    VIX_E_SCREEN_CAPTURE_ERROR                   = 19000
    VIX_E_SCREEN_CAPTURE_BAD_FORMAT              = 19001
    VIX_E_SCREEN_CAPTURE_COMPRESSION_FAIL        = 19002
    VIX_E_SCREEN_CAPTURE_LARGE_DATA              = 19003

    VIX_E_GUEST_VOLUMES_NOT_FROZEN               = 20000
    VIX_E_NOT_A_FILE                             = 20001
    VIX_E_NOT_A_DIRECTORY                        = 20002
    VIX_E_NO_SUCH_PROCESS                        = 20003
    VIX_E_FILE_NAME_TOO_LONG                     = 20004
    VIX_E_OPERATION_DISABLED                     = 20005

    VIX_E_TOOLS_INSTALL_NO_IMAGE                 = 21000
    VIX_E_TOOLS_INSTALL_IMAGE_INACCESIBLE        = 21001
    VIX_E_TOOLS_INSTALL_NO_DEVICE                = 21002
    VIX_E_TOOLS_INSTALL_DEVICE_NOT_CONNECTED     = 21003
    VIX_E_TOOLS_INSTALL_CANCELLED                = 21004
    VIX_E_TOOLS_INSTALL_INIT_FAILED              = 21005
    VIX_E_TOOLS_INSTALL_AUTO_NOT_SUPPORTED       = 21006
    VIX_E_TOOLS_INSTALL_GUEST_NOT_READY          = 21007
    VIX_E_TOOLS_INSTALL_SIG_CHECK_FAILED         = 21008
    VIX_E_TOOLS_INSTALL_ERROR                    = 21009
    VIX_E_TOOLS_INSTALL_ALREADY_UP_TO_DATE       = 21010
    VIX_E_TOOLS_INSTALL_IN_PROGRESS              = 21011
    VIX_E_TOOLS_INSTALL_IMAGE_COPY_FAILED        = 21012

    VIX_E_WRAPPER_WORKSTATION_NOT_INSTALLED      = 22001
    VIX_E_WRAPPER_VERSION_NOT_FOUND              = 22002
    VIX_E_WRAPPER_SERVICEPROVIDER_NOT_FOUND      = 22003
    VIX_E_WRAPPER_PLAYER_NOT_INSTALLED           = 22004
    VIX_E_WRAPPER_RUNTIME_NOT_INSTALLED          = 22005
    VIX_E_WRAPPER_MULTIPLE_SERVICEPROVIDERS      = 22006

    VIX_E_MNTAPI_MOUNTPT_NOT_FOUND               = 24000
    VIX_E_MNTAPI_MOUNTPT_IN_USE                  = 24001
    VIX_E_MNTAPI_DISK_NOT_FOUND                  = 24002
    VIX_E_MNTAPI_DISK_NOT_MOUNTED                = 24003
    VIX_E_MNTAPI_DISK_IS_MOUNTED                 = 24004
    VIX_E_MNTAPI_DISK_NOT_SAFE                   = 24005
    VIX_E_MNTAPI_DISK_CANT_OPEN                  = 24006
    VIX_E_MNTAPI_CANT_READ_PARTS                 = 24007
    VIX_E_MNTAPI_UMOUNT_APP_NOT_FOUND            = 24008
    VIX_E_MNTAPI_UMOUNT                          = 24009
    VIX_E_MNTAPI_NO_MOUNTABLE_PARTITONS          = 24010
    VIX_E_MNTAPI_PARTITION_RANGE                 = 24011
    VIX_E_MNTAPI_PERM                            = 24012
    VIX_E_MNTAPI_DICT                            = 24013
    VIX_E_MNTAPI_DICT_LOCKED                     = 24014
    VIX_E_MNTAPI_OPEN_HANDLES                    = 24015
    VIX_E_MNTAPI_CANT_MAKE_VAR_DIR               = 24016
    VIX_E_MNTAPI_NO_ROOT                         = 24017
    VIX_E_MNTAPI_LOOP_FAILED                     = 24018
    VIX_E_MNTAPI_DAEMON                          = 24019
    VIX_E_MNTAPI_INTERNAL                        = 24020
    VIX_E_MNTAPI_SYSTEM                          = 24021
    VIX_E_MNTAPI_NO_CONNECTION_DETAILS           = 24022

    VIX_E_MNTAPI_INCOMPATIBLE_VERSION            = 24300
    VIX_E_MNTAPI_OS_ERROR                        = 24301
    VIX_E_MNTAPI_DRIVE_LETTER_IN_USE             = 24302
    VIX_E_MNTAPI_DRIVE_LETTER_ALREADY_ASSIGNED   = 24303
    VIX_E_MNTAPI_VOLUME_NOT_MOUNTED              = 24304
    VIX_E_MNTAPI_VOLUME_ALREADY_MOUNTED          = 24305
    VIX_E_MNTAPI_FORMAT_FAILURE                  = 24306
    VIX_E_MNTAPI_NO_DRIVER                       = 24307
    VIX_E_MNTAPI_ALREADY_OPENED                  = 24308
    VIX_E_MNTAPI_ITEM_NOT_FOUND                  = 24309
    VIX_E_MNTAPI_UNSUPPROTED_BOOT_LOADER         = 24310
    VIX_E_MNTAPI_UNSUPPROTED_OS                  = 24311
    VIX_E_MNTAPI_CODECONVERSION                  = 24312
    VIX_E_MNTAPI_REGWRITE_ERROR                  = 24313
    VIX_E_MNTAPI_UNSUPPORTED_FT_VOLUME           = 24314
    VIX_E_MNTAPI_PARTITION_NOT_FOUND             = 24315
    VIX_E_MNTAPI_PUTFILE_ERROR                   = 24316
    VIX_E_MNTAPI_GETFILE_ERROR                   = 24317
    VIX_E_MNTAPI_REG_NOT_OPENED                  = 24318
    VIX_E_MNTAPI_REGDELKEY_ERROR                 = 24319
    VIX_E_MNTAPI_CREATE_PARTITIONTABLE_ERROR     = 24320
    VIX_E_MNTAPI_OPEN_FAILURE                    = 24321
    VIX_E_MNTAPI_VOLUME_NOT_WRITABLE             = 24322

    VIX_ASYNC                                    = 25000

    VIX_E_ASYNC_MIXEDMODE_UNSUPPORTED            = 26000

    VIX_E_NET_HTTP_UNSUPPORTED_PROTOCOL     = 30001
    VIX_E_NET_HTTP_URL_MALFORMAT            = 30003
    VIX_E_NET_HTTP_COULDNT_RESOLVE_PROXY    = 30005
    VIX_E_NET_HTTP_COULDNT_RESOLVE_HOST     = 30006
    VIX_E_NET_HTTP_COULDNT_CONNECT          = 30007
    VIX_E_NET_HTTP_HTTP_RETURNED_ERROR      = 30022
    VIX_E_NET_HTTP_OPERATION_TIMEDOUT       = 30028
    VIX_E_NET_HTTP_SSL_CONNECT_ERROR        = 30035
    VIX_E_NET_HTTP_TOO_MANY_REDIRECTS       = 30047
    VIX_E_NET_HTTP_TRANSFER                 = 30200
    VIX_E_NET_HTTP_SSL_SECURITY             = 30201
    VIX_E_NET_HTTP_GENERIC                  = 30202

def VIX_ERROR_CODE(err):
    return err & 0xffff

def VIX_SUCCEEDED(err):
    return VixError.VIX_OK == err

def VIX_FAILED(err):
    return VixError.VIX_OK != err

class VixPropertyType(IntEnum):
    VIX_PROPERTYTYPE_ANY             = 0
    VIX_PROPERTYTYPE_INTEGER         = 1
    VIX_PROPERTYTYPE_STRING          = 2
    VIX_PROPERTYTYPE_BOOL            = 3
    VIX_PROPERTYTYPE_HANDLE          = 4
    VIX_PROPERTYTYPE_INT64           = 5
    VIX_PROPERTYTYPE_BLOB            = 6

class VixPropertyID(IntEnum):
    VIX_PROPERTY_NONE                                  = 0

    VIX_PROPERTY_META_DATA_CONTAINER                   = 2

    # VIX_HANDLETYPE_HOST properties
    VIX_PROPERTY_HOST_HOSTTYPE                         = 50
    VIX_PROPERTY_HOST_API_VERSION                      = 51
    VIX_PROPERTY_HOST_SOFTWARE_VERSION                 = 52

    # VIX_HANDLETYPE_VM properties
    VIX_PROPERTY_VM_NUM_VCPUS                          = 101
    VIX_PROPERTY_VM_VMX_PATHNAME                       = 103 
    VIX_PROPERTY_VM_VMTEAM_PATHNAME                    = 105 
    VIX_PROPERTY_VM_MEMORY_SIZE                        = 106
    VIX_PROPERTY_VM_READ_ONLY                          = 107
    VIX_PROPERTY_VM_NAME                               = 108
    VIX_PROPERTY_VM_GUESTOS                            = 109
    VIX_PROPERTY_VM_IN_VMTEAM                          = 128
    VIX_PROPERTY_VM_POWER_STATE                        = 129
    VIX_PROPERTY_VM_TOOLS_STATE                        = 152
    VIX_PROPERTY_VM_IS_RUNNING                         = 196
    VIX_PROPERTY_VM_SUPPORTED_FEATURES                 = 197
    VIX_PROPERTY_VM_SSL_ERROR                          = 293

    # Result properties; these are returned by various procedures 
    VIX_PROPERTY_JOB_RESULT_ERROR_CODE                 = 3000
    VIX_PROPERTY_JOB_RESULT_VM_IN_GROUP                = 3001
    VIX_PROPERTY_JOB_RESULT_USER_MESSAGE               = 3002
    VIX_PROPERTY_JOB_RESULT_EXIT_CODE                  = 3004
    VIX_PROPERTY_JOB_RESULT_COMMAND_OUTPUT             = 3005
    VIX_PROPERTY_JOB_RESULT_HANDLE                     = 3010
    VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS        = 3011
    VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME = 3017
    VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE    = 3018
    VIX_PROPERTY_JOB_RESULT_ITEM_NAME                  = 3035
    VIX_PROPERTY_JOB_RESULT_FOUND_ITEM_DESCRIPTION     = 3036
    VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_COUNT        = 3046
    VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_HOST         = 3048
    VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_FLAGS        = 3049
    VIX_PROPERTY_JOB_RESULT_PROCESS_ID                 = 3051
    VIX_PROPERTY_JOB_RESULT_PROCESS_OWNER              = 3052
    VIX_PROPERTY_JOB_RESULT_PROCESS_COMMAND            = 3053
    VIX_PROPERTY_JOB_RESULT_FILE_FLAGS                 = 3054
    VIX_PROPERTY_JOB_RESULT_PROCESS_START_TIME         = 3055
    VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING         = 3056
    VIX_PROPERTY_JOB_RESULT_PROCESS_BEING_DEBUGGED     = 3057
    VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_SIZE          = 3058
    VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_DATA          = 3059
    VIX_PROPERTY_JOB_RESULT_FILE_SIZE                  = 3061
    VIX_PROPERTY_JOB_RESULT_FILE_MOD_TIME              = 3062
    VIX_PROPERTY_JOB_RESULT_EXTRA_ERROR_INFO           = 3084

    # Event properties; these are sent in the moreEventInfo for some events.
    VIX_PROPERTY_FOUND_ITEM_LOCATION                   = 4010

    # VIX_HANDLETYPE_SNAPSHOT properties
    VIX_PROPERTY_SNAPSHOT_DISPLAYNAME                  = 4200   
    VIX_PROPERTY_SNAPSHOT_DESCRIPTION                  = 4201
    VIX_PROPERTY_SNAPSHOT_POWERSTATE                   = 4205

    VIX_PROPERTY_GUEST_SHAREDFOLDERS_SHARES_PATH       = 4525

    # Virtual machine encryption properties
    VIX_PROPERTY_VM_ENCRYPTION_PASSWORD                = 7001



class VixPropertyType(IntEnum):
    VIX_PROPERTYTYPE_ANY             = 0
    VIX_PROPERTYTYPE_INTEGER         = 1
    VIX_PROPERTYTYPE_STRING          = 2
    VIX_PROPERTYTYPE_BOOL            = 3
    VIX_PROPERTYTYPE_HANDLE          = 4
    VIX_PROPERTYTYPE_INT64           = 5
    VIX_PROPERTYTYPE_BLOB            = 6

class VixPowerState(IntFlag):
    VIX_POWERSTATE_POWERING_OFF    = 0x0001
    VIX_POWERSTATE_POWERED_OFF     = 0x0002
    VIX_POWERSTATE_POWERING_ON     = 0x0004
    VIX_POWERSTATE_POWERED_ON      = 0x0008
    VIX_POWERSTATE_SUSPENDING      = 0x0010
    VIX_POWERSTATE_SUSPENDED       = 0x0020
    VIX_POWERSTATE_TOOLS_RUNNING   = 0x0040
    VIX_POWERSTATE_RESETTING       = 0x0080
    VIX_POWERSTATE_BLOCKED_ON_MSG  = 0x0100
    VIX_POWERSTATE_PAUSED          = 0x0200
    VIX_POWERSTATE_RESUMING        = 0x0800

class VixToolsState(IntEnum):
    VIX_TOOLSSTATE_UNKNOWN           = 0x0001
    VIX_TOOLSSTATE_RUNNING           = 0x0002
    VIX_TOOLSSTATE_NOT_INSTALLED     = 0x0004

class VixVMPowerOpOptions(IntFlag):
    VIX_VMPOWEROP_NORMAL                      = 0
    VIX_VMPOWEROP_FROM_GUEST                  = 0x0004
    VIX_VMPOWEROP_SUPPRESS_SNAPSHOT_POWERON   = 0x0080
    VIX_VMPOWEROP_LAUNCH_GUI                  = 0x0200
    VIX_VMPOWEROP_START_VM_PAUSED             = 0x1000

class VixServiceProvider(IntEnum):
    VIX_SERVICEPROVIDER_DEFAULT                   = 1
    VIX_SERVICEPROVIDER_VMWARE_SERVER             = 2
    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION        = 3
    VIX_SERVICEPROVIDER_VMWARE_PLAYER             = 4
    VIX_SERVICEPROVIDER_VMWARE_VI_SERVER          = 10
    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION_SHARED = 11


_Vix_GetErrorText = vix.Vix_GetErrorText
_Vix_GetErrorText.argtypes = [VixErrorType, c_char_p]
_Vix_GetErrorText.restype = c_char_p

Vix_ReleaseHandle = vix.Vix_ReleaseHandle
Vix_ReleaseHandle.argtypes = [VixHandle]
Vix_ReleaseHandle.restype = None

_Vix_GetProperties = vix.Vix_GetProperties
#_Vix_GetProperties.argtypes
_Vix_GetProperties.restype = VixError

_Vix_GetPropertyType = vix.Vix_GetPropertyType
_Vix_GetPropertyType.argtypes = [VixHandle, c_int, POINTER(c_int)]
_Vix_GetPropertyType.restype = VixError

Vix_FreeBuffer = vix.Vix_FreeBuffer
Vix_FreeBuffer.argtypes = [c_char_p]
Vix_FreeBuffer.restype = None

_VixJob_Wait = vix.VixJob_Wait
#_VixJob_Wait.argtypes
_VixJob_Wait.restype = VixError

VixJob_GetNumProperties = vix.VixJob_GetNumProperties
VixJob_GetNumProperties.argtypes = [VixHandle, c_int]
VixJob_GetNumProperties.restype = c_int

_VixJob_GetNthProperties = vix.VixJob_GetNthProperties
#_VixJob_GetNthProperties.argtypes
_VixJob_GetNthProperties.restype = VixError

_VixHost_Connect = vix.VixHost_Connect
_VixHost_Connect.argtypes = [c_int, c_int, c_char_p, c_int, c_char_p, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixHost_Connect.restype = VixHandle

VixHost_Disconnect = vix.VixHost_Disconnect
VixHost_Disconnect.argtypes = [VixHandle]
VixHost_Disconnect.restype = None

_VixHost_FindItems = vix.VixHost_FindItems
_VixHost_FindItems.argtypes = [VixHandle, c_int, c_int, c_int, VixEventProc, c_void_p]
_VixHost_FindItems.restype = VixHandle

_VixHost_OpenVM = vix.VixHost_OpenVM
_VixHost_OpenVM.argtypes = [VixHandle, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixHost_OpenVM.restype = VixHandle

_VixVM_Open = vix.VixVM_Open
_VixVM_Open.argtypes = [VixHandle, c_char_p, c_void_p, c_void_p]
_VixVM_Open.restype = VixHandle

_VixVM_PowerOn = vix.VixVM_PowerOn
_VixVM_PowerOn.argtypes = [VixHandle, c_int, c_int, c_void_p, c_void_p]
_VixVM_PowerOn.restype = VixHandle

_VixVM_PowerOff = vix.VixVM_PowerOff
_VixVM_PowerOff.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_PowerOff.restype = VixHandle

_VixVM_Reset = vix.VixVM_Reset
_VixVM_Reset.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_Reset.restype = VixHandle

_VixVM_Suspend = vix.VixVM_Suspend
_VixVM_Suspend.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_Suspend.restype = VixHandle

_VixVM_Pause = vix.VixVM_Pause
_VixVM_Pause.argtypes = [VixHandle, c_int, c_int, c_void_p, c_void_p]
_VixVM_Pause.restype = VixHandle

_VixVM_Unpause = vix.VixVM_Unpause
_VixVM_Unpause.argtypes = [VixHandle, c_int, c_int, c_void_p, c_void_p]
_VixVM_Unpause.restype = VixHandle

_VixVM_WaitForToolsInGuest = vix.VixVM_WaitForToolsInGuest
_VixVM_WaitForToolsInGuest.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_WaitForToolsInGuest.restype = VixHandle

_VixVM_LoginInGuest = vix.VixVM_LoginInGuest
_VixVM_LoginInGuest.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_LoginInGuest.restype = VixHandle

_VixVM_LogoutFromGuest = vix.VixVM_LogoutFromGuest
_VixVM_LogoutFromGuest.argtypes = [VixHandle, c_void_p, c_void_p]
_VixVM_LogoutFromGuest.restype = VixHandle

_VixVM_RunProgramInGuest = vix.VixVM_RunProgramInGuest
_VixVM_RunProgramInGuest.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixVM_RunProgramInGuest.restype = VixHandle

_VixVM_ListProcessesInGuest = vix.VixVM_ListProcessesInGuest
_VixVM_ListProcessesInGuest.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_ListProcessesInGuest.restype = VixHandle

_VixVM_KillProcessInGuest = vix.VixVM_KillProcessInGuest
_VixVM_KillProcessInGuest.argtypes = [VixHandle, c_ulonglong, c_int, c_void_p, c_void_p]
_VixVM_KillProcessInGuest.restype = VixHandle

_VixVM_RunScriptInGuest = vix.VixVM_RunScriptInGuest
_VixVM_RunScriptInGuest.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixVM_RunScriptInGuest.restype = VixHandle

_VixVM_CopyFileFromHostToGuest = vix.VixVM_CopyFileFromHostToGuest
_VixVM_CopyFileFromHostToGuest.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixVM_CopyFileFromHostToGuest.restype = VixHandle

_VixVM_CopyFileFromGuestToHost = vix.VixVM_CopyFileFromGuestToHost
_VixVM_CopyFileFromGuestToHost.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixVM_CopyFileFromGuestToHost.restype = VixHandle

_VixVM_DeleteFileInGuest = vix.VixVM_DeleteFileInGuest
_VixVM_DeleteFileInGuest.argtypes = [VixHandle, c_char_p, c_void_p, c_void_p]
_VixVM_DeleteFileInGuest.restype = VixHandle

_VixVM_FileExistsInGuest = vix.VixVM_FileExistsInGuest
_VixVM_FileExistsInGuest.argtypes = [VixHandle, c_char_p, c_void_p, c_void_p]
_VixVM_FileExistsInGuest.restype = VixHandle

_VixVM_RenameFileInGuest = vix.VixVM_RenameFileInGuest
_VixVM_RenameFileInGuest.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixVM_RenameFileInGuest.restype = VixHandle

_VixVM_CreateTempFileInGuest = vix.VixVM_CreateTempFileInGuest
_VixVM_CreateTempFileInGuest.argtypes = [VixHandle, c_int, c_int, c_void_p, c_void_p]
_VixVM_CreateTempFileInGuest.restype = VixHandle

_VixVM_GetFileInfoInGuest = vix.VixVM_GetFileInfoInGuest
_VixVM_GetFileInfoInGuest.argtypes = [VixHandle, c_char_p, c_void_p, c_void_p]
_VixVM_GetFileInfoInGuest.restype = VixHandle

_VixVM_ListDirectoryInGuest = vix.VixVM_ListDirectoryInGuest
_VixVM_ListDirectoryInGuest.argtypes = [VixHandle, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_ListDirectoryInGuest.restype = VixHandle

_VixVM_CreateDirectoryInGuest = vix.VixVM_CreateDirectoryInGuest
_VixVM_CreateDirectoryInGuest.argtypes = [VixHandle, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_CreateDirectoryInGuest.restype = VixHandle

_VixVM_DeleteDirectoryInGuest = vix.VixVM_DeleteDirectoryInGuest
_VixVM_DeleteDirectoryInGuest.argtypes = [VixHandle, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_DeleteDirectoryInGuest.restype = VixHandle

_VixVM_DirectoryExistsInGuest = vix.VixVM_DirectoryExistsInGuest
_VixVM_DirectoryExistsInGuest.argtypes = [VixHandle, c_char_p, c_void_p, c_void_p]
_VixVM_DirectoryExistsInGuest.restype = VixHandle

_VixVM_ReadVariable = vix.VixVM_ReadVariable
_VixVM_ReadVariable.argtypes = [VixHandle, c_int, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_ReadVariable.restype = VixHandle

_VixVM_WriteVariable = vix.VixVM_WriteVariable
_VixVM_WriteVariable.argtypes = [VixHandle, c_int, c_char_p, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_WriteVariable.restype = VixHandle

_VixVM_CaptureScreenImage = vix.VixVM_CaptureScreenImage
_VixVM_CaptureScreenImage.argtypes = [VixHandle, c_int, c_int, c_char_p, c_char_p]
_VixVM_CaptureScreenImage.restype = VixHandle

_VixVM_UpgradeVirtualHardware = vix.VixVM_UpgradeVirtualHardware
_VixVM_UpgradeVirtualHardware.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_UpgradeVirtualHardware.restype = VixHandle

_VixVM_InstallTools = vix.VixVM_InstallTools
_VixVM_InstallTools.argtypes = [VixHandle, c_int, c_char_p, c_void_p, c_void_p]
_VixVM_InstallTools.restype = VixHandle

_VixVM_Delete = vix.VixVM_Delete
_VixVM_Delete.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_Delete.restype = VixHandle

_VixVM_Clone = vix.VixVM_Clone
_VixVM_Clone.argtypes = [VixHandle, VixHandle, c_int, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixVM_Clone.restype = VixHandle

_VixVM_CreateSnapshot = vix.VixVM_CreateSnapshot
_VixVM_CreateSnapshot.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_int, c_void_p, c_void_p]
_VixVM_CreateSnapshot.restype = VixHandle

_VixVM_RevertToSnapshot = vix.VixVM_RevertToSnapshot
_VixVM_RevertToSnapshot.argtypes = [VixHandle, VixHandle, c_int, c_int, c_void_p, c_void_p]
_VixVM_RevertToSnapshot.restype = VixHandle

_VixVM_RemoveSnapshot = vix.VixVM_RemoveSnapshot
_VixVM_RemoveSnapshot.argtypes = [VixHandle, VixHandle, c_int, c_void_p, c_void_p]
_VixVM_RemoveSnapshot.restype = VixHandle

_VixVM_GetNamedSnapshot = vix.VixVM_GetNamedSnapshot
_VixVM_GetNamedSnapshot.argtypes = [VixHandle, c_char_p, POINTER(VixHandle)]
_VixVM_GetNamedSnapshot.restype = VixErrorType

_VixVM_GetCurrentSnapshot = vix.VixVM_GetCurrentSnapshot
_VixVM_GetCurrentSnapshot.argtypes = [VixHandle, POINTER(VixHandle)]
_VixVM_GetCurrentSnapshot.restype = VixErrorType

_VixVM_GetRootSnapshot = vix.VixVM_GetRootSnapshot
_VixVM_GetRootSnapshot.argtypes = [VixHandle, c_int, POINTER(VixHandle)]
_VixVM_GetRootSnapshot.restype = VixErrorType

_VixVM_GetNumRootSnapshots = vix.VixVM_GetNumRootSnapshots
_VixVM_GetNumRootSnapshots.argtypes = [VixHandle, POINTER(c_int)]
_VixVM_GetNumRootSnapshots.restype = VixErrorType

_VixVM_EnableSharedFolders = vix.VixVM_EnableSharedFolders
_VixVM_EnableSharedFolders.argtypes = [VixHandle, c_bool, c_int, c_void_p, c_void_p]
_VixVM_EnableSharedFolders.restype = VixHandle

_VixVM_GetNumSharedFolders = vix.VixVM_GetNumSharedFolders
_VixVM_GetNumSharedFolders.argtypes = [VixHandle, c_void_p, c_void_p]
_VixVM_GetNumSharedFolders.restype = VixHandle

_VixVM_AddSharedFolder = vix.VixVM_AddSharedFolder
_VixVM_AddSharedFolder.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_AddSharedFolder.restype = VixHandle

_VixVM_RemoveSharedFolder = vix.VixVM_RemoveSharedFolder
_VixVM_RemoveSharedFolder.argtypes = [VixHandle, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_RemoveSharedFolder.restype = VixHandle

_VixVM_GetSharedFolderState = vix.VixVM_GetSharedFolderState
_VixVM_GetSharedFolderState.argtypes = [VixHandle, c_int, c_void_p, c_void_p]
_VixVM_GetSharedFolderState.restype = VixHandle

_VixVM_SetSharedFolderState = vix.VixVM_SetSharedFolderState
_VixVM_SetSharedFolderState.argtypes = [VixHandle, c_char_p, c_char_p, c_int, c_void_p, c_void_p]
_VixVM_SetSharedFolderState.restype = VixHandle

_VixSnapshot_GetNumChildren = vix.VixSnapshot_GetNumChildren
_VixSnapshot_GetNumChildren.argtypes = [VixHandle, POINTER(c_int)]
_VixSnapshot_GetNumChildren.restype = VixErrorType

_VixSnapshot_GetChild = vix.VixSnapshot_GetChild
_VixSnapshot_GetChild.argtypes = [VixHandle, c_int, POINTER(VixHandle)]
_VixSnapshot_GetChild.restype = VixErrorType

_VixSnapshot_GetParent = vix.VixSnapshot_GetParent
_VixSnapshot_GetParent.argtypes = [VixHandle, POINTER(VixHandle)]
_VixSnapshot_GetParent.restype = VixErrorType

def Vix_GetErrorText(err):
    return _Vix_GetErrorText(err, None).decode()

def Vix_GetPropertyType(handle, propid):
    proptype = c_int()
    err = _Vix_GetPropertyType(handle, propid, pointer(proptype))
    if VIX_FAILED(err):
        raise VixException(err)
    return VixPropertyType(proptype.value)

def Vix_GetProperties(handle, propid):
    proptype = Vix_GetPropertyType(handle, propid)
    args = [handle, propid]
    if proptype == VixPropertyType.VIX_PROPERTYTYPE_INTEGER:
        val = c_int()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_STRING:
        val = c_char_p()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_BOOL:
        val = c_int()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_HANDLE:
        val = VixHandle()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_INT64:
        val = c_longlong()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_BLOB:
        val = c_int()
        val2 = c_char_p()
    else:
        raise VixException(VixError.VIX_E_UNRECOGNIZED_PROPERTY)

    args.append(pointer(val))
    if proptype == VixPropertyType.VIX_PROPERTYTYPE_BLOB:
        args.append(pointer(val2))
    args.append(VixPropertyID.VIX_PROPERTY_NONE)

    err = _Vix_GetProperties(*args)
    if VIX_FAILED(err):
        raise VixException(err)
    if proptype == VixPropertyType.VIX_PROPERTYTYPE_BOOL:
        ret = bool(val.value)
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_STRING:
        ret = val.value.decode()
        Vix_FreeBuffer(val)
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_BLOB:
        ret = val2.value
        Vix_FreeBuffer(val2)
    else:
        ret = val.value
    return ret

def VixJob_GetNthProperties(handle, index, propid):
    proptype = Vix_GetPropertyType(handle, propid)
    args = [handle, index, propid]
    if proptype == VixPropertyType.VIX_PROPERTYTYPE_INTEGER:
        val = c_int()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_STRING:
        val = c_char_p()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_BOOL:
        val = c_int()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_HANDLE:
        val = VixHandle()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_INT64:
        val = c_longlong()
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_BLOB:
        val = c_int()
        val2 = c_char_p()
    else:
        raise VixException(VixError.VIX_E_UNRECOGNIZED_PROPERTY)

    args.append(pointer(val))
    if proptype == VixPropertyType.VIX_PROPERTYTYPE_BLOB:
        args.append(pointer(val2))
    args.append(VixPropertyID.VIX_PROPERTY_NONE)

    err = _VixJob_GetNthProperties(*args)
    if VIX_FAILED(err):
        raise VixException(err)
    if proptype == VixPropertyType.VIX_PROPERTYTYPE_BOOL:
        ret = bool(val.value)
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_STRING:
        ret = val.value.decode()
        Vix_FreeBuffer(val)
    elif proptype == VixPropertyType.VIX_PROPERTYTYPE_BLOB:
        ret = val2.value
        Vix_FreeBuffer(val2)
    else:
        ret = val.value
    return ret

def VixJob_Wait(job, return_handle=False, props=[], N=False):
    if return_handle:
        handle = VixHandle() 
        args = [job,
                VixPropertyID.VIX_PROPERTY_JOB_RESULT_HANDLE,
                pointer(handle), 
                VixPropertyID.VIX_PROPERTY_NONE]
    else:
        handle = None
        args = [job, VixPropertyID.VIX_PROPERTY_NONE]
    err = _VixJob_Wait(*args)

    proplist = {}
    if VIX_SUCCEEDED(err):
        if N:
            count = VixJob_GetNumProperties(job, props[0])
            proplist = []
            for i in range(count):
                propset = {}
                for prop in props:
                    propset[prop] = VixJob_GetNthProperties(job, i, prop)
                proplist.append(propset)
        else:
            for prop in props:
                proplist[prop] = Vix_GetProperties(job, prop)

    Vix_ReleaseHandle(job)
    
    if VIX_FAILED(err):
        raise VixException(err)
    
    ret = None
    if proplist:
        ret  = proplist
    else:
        ret = handle
    return ret


#
# VIX Host
#

def VixHost_Connect(provider=VixServiceProvider.VIX_SERVICEPROVIDER_VMWARE_WORKSTATION):
    args = [VIX_API_VERSION,
            provider,
            None,
            0,
            None,
            None,
            0,
            0,
            None,
            None]
    job = _VixHost_Connect(*args)
    return VixJob_Wait(job, True)


def py_FindItemsCB(job, event, info, data):
    if event != VIX_EVENTTYPE_FIND_ITEM:
        return

    loc = c_char_p()
    err = _Vix_GetProperties(info, VixPropertyID.VIX_PROPERTY_FOUND_ITEM_LOCATION,
            pointer(loc), VixPropertyID.VIX_PROPERTY_NONE)

    if VIX_SUCCEEDED(err):
        pydata = cast(data,POINTER(py_object)).contents.value
        pydata.append(loc.value.decode())
        Vix_FreeBuffer(loc)


def VixHost_FindItems(host):
    items = []
    data = cast(pointer(py_object(items)), c_void_p)
    args = [host, VIX_FIND_RUNNING_VMS, VIX_INVALID_HANDLE, -1,
            VixEventProc(py_FindItemsCB), data]
    job = _VixHost_FindItems(*args)
    VixJob_Wait(job)
    return items

def VixHost_OpenVM(host, vmx):
    vmx = to_bytes(vmx)
    job = _VixHost_OpenVM(host, vmx, VIX_VMOPEN_NORMAL, 0, None, None)
    return VixJob_Wait(job, True)

#
# VIX VM
#

def VixVM_Open(host, path):
    path = to_bytes(path)
    job = _VixVM_Open(host, path, None, None)
    return VixJob_Wait(job, True)

def VixVM_PowerOn(handle, options):
    job = _VixVM_PowerOn(handle, options, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_PowerOff(handle, options):
    job = _VixVM_PowerOff(handle, options, None, None)
    VixJob_Wait(job)

def VixVM_Reset(handle, options):
    job = _VixVM_Reset(handle, options, None, None)
    VixJob_Wait(job)

def VixVM_Suspend(handle):
    job = _VixVM_Suspend(handle, 0, None, None)
    VixJob_Wait(job)

def VixVM_Pause(handle):
    job = _VixVM_Pause(handle, 0, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_Unpause(handle):
    job = _VixVM_Unpause(handle, 0, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_WaitForToolsInGuest(handle, timeout=0):
    job = _VixVM_WaitForToolsInGuest(handle, timeout)
    VixJob_Wait(job)

def VixVM_LoginInGuest(handle, user, pword, options):
    user = to_bytes(user)
    pword = to_bytes(pword)
    job = _VixVM_LoginInGuest(handle, user, pword, options, None, None)
    VixJob_Wait(job)

def VixVM_LogoutFromGuest(handle):
    job = _VixVM_LogoutFromGuest(handle, None, None)
    VixJob_Wait(job)

def VixVM_RunProgramInGuest(handle, program, args, options):
    program = to_bytes(program)
    args = to_bytes(args)
    job = _VixVM_RunProgramInGuest(handle, program, args, options,
            VIX_INVALID_HANDLE, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_PROCESS_ID]
    if not (options & VIX_RUNPROGRAM_RETURN_IMMEDIATELY):
        props.append(VixPropertyID.VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME)
        props.append(VixPropertyID.VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE)
    return VixJob_Wait(job, props=props)

def VixVM_ListProcessesInGuest(handle):
    job = _VixVM_ListProcessesInGuest(handle, 0, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_PROCESS_ID,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_PROCESS_OWNER,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_PROCESS_COMMAND,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_PROCESS_BEING_DEBUGGED,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_PROCESS_START_TIME]
    return VixJob_Wait(job, props=props, N=True)

def VixVM_KillProcessInGuest(handle, pid):
    job = _VixVM_KillProcessInGuest(handle, pid, 0, None, None)
    VixJob_Wait(job)

def VixVM_RunScriptInGuest(handle, interpreter, text, options):
    interpreter = to_bytes(interpreter)
    text = to_bytes(text)
    job = _VixVM_RunScriptInGuest(handle, interpreter, text, options,
            VIX_INVALID_HANDLE, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_PROCESS_ID]
    if not (options & VIX_RUNPROGRAM_RETURN_IMMEDIATELY):
        props.append(VixPropertyID.VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME)
        props.append(VixPropertyID.VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE)
    return VixJob_Wait(job, props=props)

def VixVM_CopyFileFromHostToGuest(handle, src, dst):
    src = to_bytes(src)
    dst = to_bytes(dst)
    job = _VixVM_CopyFileFromHostToGuest(handle, src, dst, 0,
            VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_CopyFileFromGuestToHost(handle, src, dst):
    src = to_bytes(src)
    dst = to_bytes(dst)
    job = _VixVM_CopyFileFromGuestToHost(handle, src, dst, 0,
            VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_DeleteFileInGuest(handle, path):
    path = to_bytes(path)
    job = _VixVM_DeleteFileInGuest(handle, path, None, None)
    VixJob_Wait(job)

def VixVM_FileExistsInGuest(handle, path):
    path = to_bytes(path)
    job = _VixVM_FileExistsInGuest(handle, path, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS]
    results = VixJob_Wait(job, props=props)
    return results[props[0]]

def VixVM_RenameFileInGuest(handle, old, new):
    old = to_bytes(old)
    new = to_bytes(new)
    job = _VixVM_RenameFileInGuest(handle, old, new, 0, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_CreateTempFileInGuest(handle):
    job = _VixVM_CreateTempFileInGuest(handle, 0, VIX_INVALID_HANDLE, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_ITEM_NAME]
    results = VixJob_Wait(job, props=props)
    return results[props[0]]

def VixVM_GetFileInfoInGuest(handle, path):
    path = to_bytes(path)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_FILE_SIZE,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_FILE_FLAGS,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_FILE_MOD_TIME]
    job = _VixVM_GetFileInfoInGuest(handle, path, None, None)
    return VixJob_Wait(job, props=props)

def VixVM_ListDirectoryInGuest(handle, path):
    path = to_bytes(path)
    job = _VixVM_ListDirectoryInGuest(handle, path, 0, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_FILE_SIZE,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_FILE_FLAGS,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_FILE_MOD_TIME]
    return VixJob_Wait(job, props=props, N=True)

def VixVM_CreateDirectoryInGuest(handle, path):
    path = to_bytes(path)
    job = _VixVM_CreateDirectoryInGuest(handle, path, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_DeleteDirectoryInGuest(handle, path):
    path = to_bytes(path)
    job = _VixVM_DeleteDirectoryInGuest(handle, path, 0, None, None)
    VixJob_Wait(job)

def VixVM_DirectoryExistsInGuest(handle, path):
    path = to_bytes(path)
    job = _VixVM_DirectoryExistsInGuest(handle, path, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS]
    results = VixJob_Wait(job, props=props)
    return results[props[0]]

def VixVM_ReadVariable(handle, vtype, name):
    name = to_bytes(name)
    job = _VixVM_ReadVariable(handle, vtype, name, 0, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING]
    results = VixJob_Wait(job, props=props)
    return results[props[0]]

def VixVM_WriteVariable(handle, vtype, name, value):
    name = to_bytes(name)
    value = to_bytes(value)
    job = _VixVM_WriteVariable(handle, vtype, name, value, 0, None, None)
    VixJob_Wait(job)

def VixVM_CaptureScreenImage(handle):
    job = _VixVM_CaptureScreenImage(handle, VIX_CAPTURESCREENFORMAT_PNG, VIX_INVALID_HANDLE, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_DATA]
    results = VixJob_Wait(job, props=props)
    return results[props[0]]

def VixVM_UpgradeVirtualHardware(handle):
    job = _VixVM_UpgradeVirtualHardware(handle, 0, None, None)
    VixJob_Wait(job)

def VixVM_InstallTools(handle, options):
    job = _VixVM_InstallTools(handle, options, None, None, None)
    VixJob_Wait(job)

def VixVM_Delete(handle, options):
    job = _VixVM_Delete(handle, options, None, None)
    VixJob_Wait(job)

def VixVM_Clone(handle, snap, ctype, path):
    path = to_bytes(path)
    job = _VixVM_Clone(handle, snap, ctype, path, 0, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_CreateSnapshot(handle, name, desc, options):
    name = to_bytes(name)
    desc = to_bytes(desc)
    job = _VixVM_CreateSnapshot(handle, name, desc, options, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_RevertToSnapshot(handle, snap, options):
    job = _VixVM_RevertToSnapshot(handle, snap, options, VIX_INVALID_HANDLE, None, None)
    VixJob_Wait(job)

def VixVM_RemoveSnapshot(handle, snap, options):
    job = _VixVM_RemoveSnapshot(handle, snap, options, None, None)
    VixJob_Wait(job)

def VixVM_GetNamedSnapshot(handle, name):
    name = to_bytes(name)
    snap = VixHandle()
    err = _VixVM_GetNamedSnapshot(handle, name, pointer(snap))
    if VIX_FAILED(err):
        raise VixException(err)
    return snap.value

def VixVM_GetCurrentSnapshot(handle):
    snap = VixHandle()
    err = _VixVM_GetCurrentSnapshot(handle, pointer(snap))
    if VIX_FAILED(err):
        raise VixException(err)
    return snap.value

def VixVM_GetRootSnapshot(handle, index):
    snap = VixHandle()
    err = _VixVM_GetRootSnapshot(handle, index, pointer(snap))
    if VIX_FAILED(err):
        raise VixException(err)
    return snap.value

def VixVM_GetNumRootSnapshots(handle):
    count = c_int()
    err = _VixVM_GetNumRootSnapshots(handle, pointer(count))
    if VIX_FAILED(err):
        raise VixException(err)
    return count.value

def VixVM_EnableSharedFolders(handle, flag):
    job = _VixVM_EnableSharedFolders(handle, flag, 0, None, None)
    VixJob_Wait(job)

def VixVM_GetNumSharedFolders(handle):
    job = _VixVM_GetNumSharedFolders(handle, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_COUNT]
    results = VixJob_Wait(job, props=props)
    return results[props[0]]

def VixVM_AddSharedFolder(handle, name, hostpath, options):
    name = to_bytes(name)
    hostpath = to_bytes(hostpath)
    job = _VixVM_AddSharedFolder(handle, name, hostpath, options, None, None)
    VixJob_Wait(job)

def VixVM_RemoveSharedFolder(handle, name):
    name = to_bytes(name)
    job = _VixVM_RemoveSharedFolder(handle, name, 0, None, None)
    VixJob_Wait(job)

def VixVM_GetSharedFolderState(handle, index):
    job = _VixVM_GetSharedFolderState(handle, index, None, None)
    props = [VixPropertyID.VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_HOST,
             VixPropertyID.VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_FLAGS]
    return VixJob_Wait(job, props=props)

def VixVM_SetSharedFolderState(handle, name, hostpath, options):
    name = to_bytes(name)
    hostpath = to_bytes(hostpath)
    job = _VixVM_SetSharedFolderState(handle, name, hostpath, options, None, None)
    VixJob_Wait(job)

def VixSnapshot_GetNumChildren(handle):
    count = c_int()
    err = _VixSnapshot_GetNumChildren(handle, pointer(count))
    if VIX_FAILED(err):
        raise VixException(err)
    return count.value

def VixSnapshot_GetChild(handle, index):
    snap = VixHandle()
    err = _VixSnapshot_GetChild(handle, index, pointer(snap))
    if VIX_FAILED(err):
        raise VixException(err)
    return snap.value

def VixSnapshot_GetParent(handle):
    snap = VixHandle()
    err = _VixSnapshot_GetParent(handle, pointer(snap))
    if VIX_FAILED(err):
        raise VixException(err)
    return snap.value

class VixHost(object):
    def __init__(self, prov=VixServiceProvider.VIX_SERVICEPROVIDER_VMWARE_WORKSTATION):
        try:
            self._host = VixHost_Connect(prov)
        except:
            self._host = None
            raise

    def __del__(self):
        if self._host:
            VixHost_Disconnect(self._host)

    @property
    def handle(self):
        return self._host

    def open(self, path):
        """open(vmx_path) -> VixVM object"""
        vm = VixHost_OpenVM(self._host, path)
        return VixVm(vm)

    def list(self):
        """list() -> list of open VMs"""
        return VixHost_FindItems(self._host)

    @property
    def api_version(self):
        return Vix_GetProperties(self._host, VixPropertyID.VIX_PROPERTY_HOST_API_VERSION)

    @property
    def host_type(self):
        h = Vix_GetProperties(self._host, VixPropertyID.VIX_PROPERTY_HOST_HOSTTYPE)
        return VixServiceProvider(h)

    @property
    def software_version(self):
        v = Vix_GetProperties(self._host, VixPropertyID.VIX_PROPERTY_HOST_SOFTWARE_VERSION)
        return v.decode()

class VixVm(object):
    def __init__(self, handle):
        self._vm = handle

    def __del__(self):
        Vix_ReleaseHandle(self._vm)

    @property
    def handle(self):
        return self._vm
    def on(self, gui=False):
        """on(gui=False) -> power on VM"""
        option = VixVMPowerOpOptions(VixVMPowerOpOptions.VIX_VMPOWEROP_NORMAL)
        if gui:
            option |= VixVMPowerOpOptions.VIX_VMPOWEROP_LAUNCH_GUI
        VixVM_PowerOn(self._vm, option)

    def off(self, from_guest=False):
        """off(from_guest=False) -> power off VM"""
        option = VixVMPowerOpOptions(VixVMPowerOpOptions.VIX_VMPOWEROP_NORMAL)
        if from_guest:
            option |= VixVMPowerOpOptions.VIX_VMPOWEROP_FROM_GUEST
        VixVM_PowerOff(self._vm, option)

    def reset(self, from_guest=False):
        """reset(from_guest=False) -> reset VM"""
        option = VixVMPowerOpOptions(VixVMPowerOpOptions.VIX_VMPOWEROP_NORMAL)
        if from_guest:
            option |= VixVMPowerOpOptions.VIX_VMPOWEROP_FROM_GUEST
        VixVM_Reset(self._vm, option)

    def suspend(self):
        """suspend() -> suspend VM"""
        VixVM_Suspend(self._vm)

    def pause(self):
        """pause() -> pause VM"""
        VixVM_Pause(self._vm)

    def unpause(self):
        """unpause() -> unpause VM"""
        VixVM_Unpause(self._vm)

    def wait(self, timeout=0):
        """wait(timeout=0) -> wait for VMware tools to initialize"""
        VixVM_WaitForToolsInGuest(self._vm, timeout)

    def login(self, username, password, interactive=False):
        """login(user, pass, interactive=False) -> login to VM"""
        if interactive:
            options = VIX_LOGIN_IN_GUEST_REQUIRE_INTERACTIVE_ENVIRONMENT
        else:
            options = 0
        VixVM_LoginInGuest(self._vm, username, password, options)

    def logout(self):
        """logout() -> logout from VM"""
        VixVM_LogoutFromGuest(self._vm)

    def run(self, program, args='', wait=True, window=False):
        """run(prog, args, wait=True, window=False) -> Run prog in VM"""
        options = 0
        if not wait:
            options |= VIX_RUNPROGRAM_RETURN_IMMEDIATELY
        if window:
            options |= VIX_RUNPROGRAM_ACTIVATE_WINDOW
        return VixVM_RunProgramInGuest(self._vm, program, args, options)

    def ps(self):
        """ps() -> process list from VM"""
        return VixVM_ListProcessesInGuest(self._vm)

    def kill(self, pid):
        """kill(pid) -> kill pid in VM"""
        VixVM_KillProcessInGuest(self._vm, pid)

    def script(self, interpreter=None, text='', wait=True):
        """script(interpreter, text, wait) -> run script in VM"""
        options = 0
        if not wait:
            options |= VIX_RUNPROGRAM_RETURN_IMMEDIATELY
        return VixVM_RunScriptInGuest(self._vm, interpreter, text, options)

    def copy_to(self, src, dst):
        """copy_to(src, dst) -> copy file to VM"""
        VixVM_CopyFileFromHostToGuest(self._vm, src, dst)

    def copy_from(self, src, dst):
        """copy_from(src, dst) -> copy file from VM"""
        VixVM_CopyFileFromGuestToHost(self._vm, src, dst)

    def rm(self, path):
        """rm(path) -> delete file in VM"""
        VixVM_DeleteFileInGuest(self._vm, path)

    def exists_file(self, path):
        """exists(path) -> check if file exists in VM"""
        return VixVM_FileExistsInGuest(self._vm, path)

    def rename(self, old, new):
        """rename(old, new) -> rename file in VM"""
        VixVM_RenameFileInGuest(self._vm, old, new)

    def mktemp(self):
        """mktemp() -> create a temp file in VM"""
        return VixVM_CreateTempFileInGuest(self._vm)

    def stat(self, path):
        """stat(path) -> get info about file in VM"""
        return VixVM_GetFileInfoInGuest(self._vm, path)

    def ls(self, path):
        """ls(path) -> list directory in VM"""
        return VixVM_ListDirectoryInGuest(self._vm, path)

    def mkdir(self, path):
        """mkdir(path) -> create a directory in VM"""
        VixVM_CreateDirectoryInGuest(self._vm, path)

    def rmdir(self, path):
        """rmdir(path) -> remove a directory in VM"""
        VixVM_DeleteDirectoryInGuest(self._vm, path)

    def exists_dir(self, path):
        """exists_dir(path) -> check if dir exists in VM"""
        return VixVM_DirectoryExistsInGuest(self._vm, path)

    def read_guest_var(self, name):
        """read_guest_var(name) -> read var from guest"""
        return VixVM_ReadVariable(self._vm, VIX_VM_GUEST_VARIABLE, name)

    def read_config_var(self, name):
        """read_config_var(name) -> read var from config"""
        return VixVM_ReadVariable(self._vm, VIX_VM_CONFIG_RUNTIME_ONLY, name)

    def read_env_var(self, name):
        """read_env_var(name) -> read var from environment"""
        return VixVM_ReadVariable(self._vm, VIX_GUEST_ENVIRONMENT_VARIABLE, name)

    def write_guest_var(self, name, value):
        """write_guest_var(name, value) -> write var=value to guest"""
        VixVM_WriteVariable(self._vm, VIX_VM_GUEST_VARIABLE, name, value)

    def write_config_var(self, name, value):
        """write_config_var(name, value) -> write var=value to config"""
        VixVM_WriteVariable(self._vm, VIX_VM_CONFIG_RUNTIME_ONLY, name, value)

    def write_env_var(self, name, value):
        """write_env_var(name, value) -> write var=value to environment"""
        VixVM_WriteVariable(self._vm, VIX_GUEST_ENVIRONMENT_VARIABLE, name, value)

    def enable_sharedfolders(self):
        """enable_sharedfolders() -> enable shared folders in VM"""
        VixVM_EnableSharedFolders(self._vm, 1)

    def disable_sharedfolders(self):
        """disable_sharedfolders() -> disable shared folders in VM"""
        VixVM_EnableSharedFolders(self._vm, 0)

    def num_sharedfolders(self):
        """num_sharedfolders() -> number of shared folders"""
        return VixVM_GetNumSharedFolders(self._vm)

    def get_sharedfolder(self, index):
        """get_sharedfolder(index) -> get a shared folder properties"""
        return VixVM_GetSharedFolderState(self._vm, index)

    def set_sharedfolder(self, name, hostpath, writeable=True):
        """set_sharedfolder(name, hostpath, writeable=True) -> set properties on a shared folder"""
        if writeable:
            options = VIX_SHAREDFOLDER_WRITE_ACCESS
        else:
            options = 0
        VixVM_SetSharedFolderState(self._vm, name, hostpath, options)

    def add_sharedfolder(self, name, hostpath, writeable=True):
        """add_sharedfolder(name, hostpath, writeable=True) -> add shared folder"""
        if writeable:
            options = VIX_SHAREDFOLDER_WRITE_ACCESS
        else:
            options = 0
        VixVM_AddSharedFolder(self._vm, name, hostpath, options)

    def del_sharedfolder(self, name):
        """del_sharedfolder(name) -> delete a shared folder"""
        VixVM_RemoveSharedFolder(self._vm, name)

    def num_rootsnapshots(self):
        """num_rootsnapshots() -> number of root snapshots"""
        return VixVM_GetNumRootSnapshots(self._vm)

    def get_rootsnapshot(self, index):
        """get_rootsnapshot(index) -> get a root snapshot"""
        snap = VixVM_GetRootSnapshot(self._vm, index)
        return VixSnapshot(snap)

    def get_currentsnapshot(self):
        """get_currentsnapshot() -> get the current snapshot"""
        snap = VixVM_GetCurrentSnapshot(self._vm)
        return VixSnapshot(snap)

    def get_namedsnapshot(self, name):
        """get_namedsnapshot(name) -> get a snapshot by name"""
        snap = VixVM_GetNamedSnapshot(self._vm, name)
        return VixSnapshot(snap)

    def del_snapshot(self, snapshot, children=False):
        """del_snapshot(snapshot, children=False) -> del a snapshot"""
        if children:
            options = VIX_SNAPSHOT_REMOVE_CHILDREN
        else:
            options = 0
        VixVM_RemoveSnapshot(self._vm, snapshot.handle, options)

    def revert(self, snapshot, gui=False):
        """revert(snapshot, gui=False) -> revert to snapshot"""
        option = VixVMPowerOpOptions(VixVMPowerOpOptions.VIX_VMPOWEROP_NORMAL)
        if gui:
            option |= VixVMPowerOpOptions.VIX_VMPOWEROP_LAUNCH_GUI
        VixVM_RevertToSnapshot(self._vm, snapshot.handle, options)

    def snapshot(self, name, desc='', memory=True):
        """snapshot(name, desc, memory=True) -> create a snapshot"""
        if memory:
            options = VIX_SNAPSHOT_INCLUDE_MEMORY
        else:
            options = 0
        snap = VixVM_CreateSnapshot(self._vm, name, desc, options)
        return VixSnapshot(snap)

    def screen_capture(self):
        """screen_capture() -> screen capture running VM"""
        return VixVM_CaptureScreenImage(self._vm)

    def upgrade_hardware(self):
        """upgrade_hardware() -> upgrade virtual hardware"""
        VixVM_UpgradeVirtualHardware(self._vm)

    def install_tools(self, wait=True, auto=True):
        """install_tools(options) -> Install vmware tools in VM"""
        options = 0
        if not wait:
            options |= VIX_INSTALLTOOLS_RETURN_IMMEDIATELY
        if auto:
            options |= VIX_INSTALLTOOLS_AUTO_UPGRADE
        else:
            options |= VIX_INSTALLTOOLS_MOUNT_TOOLS_INSTALLER
        VixVM_InstallTools(self._vm, options)

    def delete(self, rm_vmdisk=True):
        """delete(rm_vmdisk=True) -> delete VM"""
        if rm_vmdisk:
            options = VIX_VMDELETE_DISK_FILES
        else:
            options = 0
        VixVM_Delete(self._vm, options)

    def clone(self, path, snapshot=None, linked=False):
        """clone(path, snapshot, type) -> clone virtual machine"""
        if linked:
            ctype = VIX_CLONETYPE_LINKED
        else:
            ctype = VIX_CLONETYPE_FULL
        if snapshot:
            snap = snapshot.handle
        else:
            snap = VIX_INVALID_HANDLE
        VixVM_Clone(self._vm, snap, ctype, path)

    @property
    def num_vcpus(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_NUM_VCPUS)

    @property
    def vmx_pathname(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_VMX_PATHNAME)

    @property
    def memory_size(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_MEMORY_SIZE)

    @property
    def read_only(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_READ_ONLY)

    @property
    def name(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_NAME)

    @property
    def guestos(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_GUESTOS)

    @property
    def power_state(self):
        state = Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_POWER_STATE)
        return VixPowerState(state)

    @property
    def tools_state(self):
        state = Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_TOOLS_STATE)
        return VixToolsState(state)

    @property
    def is_running(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_VM_IS_RUNNING)

    @property
    def shares_path(self):
        return Vix_GetProperties(self._vm, VixPropertyID.VIX_PROPERTY_GUEST_SHAREDFOLDERS_SHARES_PATH)

class VixSnapshot(object):
    def __init__(self, handle):
        self._snap = handle

    def __del__(self):
        Vix_ReleaseHandle(self._snap)

    @property
    def handle(self):
        return self._snap

    def num_children(self):
        return VixSnapshot_GetNumChildren(self._snap)

    def get_child(self, index):
        snap = VixSnapshot_GetChild(self._snap, index)
        return VixSnapshot(snap)

    def get_parent(self):
        snap = VixSnapshot_GetParent(self._snap)
        return VixSnapshot(snap)

    @property
    def displayname(self):
        return Vix_GetProperties(self._snap, VixPropertyID.VIX_PROPERTY_SNAPSHOT_DISPLAYNAME)

    @property
    def description(self):
        return Vix_GetProperties(self._snap, VixPropertyID.VIX_PROPERTY_SNAPSHOT_DESCRIPTION)

    @property
    def is_replayable(self):
        return Vix_GetProperties(self._snap, VixPropertyID.VIX_PROPERTY_SNAPSHOT_IS_REPLAYABLE)


