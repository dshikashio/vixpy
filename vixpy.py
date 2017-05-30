from _vixpy import *

"""
__all__ = ['VixHost',
           'VixVm',
           'VixSnapshot',
           'VixError']
"""
class VixHost(object):
    def __init__(self, prov=VIX_SERVICEPROVIDER_VMWARE_WORKSTATION,
            host='', user='', passwd=''):
        try:
            self._host = VixHost_Connect(prov, host, user, passwd)
        except:
            self._host = None
            raise

    def __del__(self):
        if self._host:
            VixHost_Disconnect(self._host)

    @property
    def handle(self):
        return self._vm

    def open(self, path):
        """open(vmx_path) -> VixVM object"""
        vm = VixHost_OpenVM(self._host, path)
        return VixVm(vm)

    def list(self):
        """list() -> list of open VMs"""
        return VixHost_FindItems(self._host)

    @property
    def api_version(self):
        return Vix_GetProperties(self._host, VIX_PROPERTY_HOST_API_VERSION)


# XXX - snapshots and shared folders should be lists?
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
        if gui:
            option = VIX_VMPOWEROP_LAUNCH_GUI
        else:
            option = VIX_VMPOWEROP_NORMAL
        VixVM_PowerOn(self._vm, option)

    def off(self, from_guest=False):
        """off(from_guest=False) -> power off VM"""
        if from_guest:
            option = VIX_VMPOWEROP_FROM_GUEST
        else:
            option = VIX_VMPOWEROP_NORMAL
        VixVM_PowerOff(self._vm, option)

    def reset(self, from_guest=False):
        """reset(from_guest=False) -> reset VM"""
        if from_guest:
            option = VIX_VMPOWEROP_FROM_GUEST
        else:
            option = VIX_VMPOWEROP_NORMAL
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

    def delete(self, rm_vmdisk=True):
        """delete(rm_vmdisk=True) -> delete VM"""
        if rm_vmdisk:
            options = VIX_VMDELETE_DISK_FILES
        else:
            options = 0
        VixVM_Delete(self._vm, options)

    def start_recording(self, name, desc):
        """start_recording(name, desc) -> start recording"""
        VixVM_BeingRecording(self._vm, name, desc)

    def stop_recording(self):
        """stop_recording() -> stop recording"""
        VixVM_EndRecording(self._vm)

    def start_replay(self, snapshot, gui=False):
        if gui:
            option = VIX_VMPOWEROP_LAUNCH_GUI
        else:
            option = VIX_VMPOWEROP_NORMAL
        VixVM_BeginReplay(selv._vm, snapshot.handle, option)

    def stop_replay(self):
        """stop_replay() -> stop replay"""
        VixVM_EndReplay(self._vm)

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

    def run(self, program, args, wait=True, window=False):
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

    def eval(self, interpreter, text, wait=True):
        """eval(interpreter, text, wait) -> run script in VM"""
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

    def exists(self, path):
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
        if gui:
            options = VIX_VMPOWEROP_LAUNCH_GUI
        else:
            options = VIX_VMPOWEROP_NORMAL
        VixVM_RevertToSnapshot(self._vm, snapshot.handle, options)

    def snapshot(self, name, desc, memory=True):
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

    def clone(self, path, snapshot=None, type=VIX_CLONETYPE_FULL):
        """clone(path, snapshot, type) -> clone virtual machine"""
        VixVM_Clone(self._vm, snapshot.handle() if snapshot is not None else 0, type, path)
		
    @property
    def num_vcpus(self):
        return Vix_GetProperties(self._vm, VIX_PROPERTY_VM_NUM_VCPUS)

    @property
    def vmx_pathname(self):
        return Vix_GetProperties(self._vm, VIX_PROPERTY_VM_VMX_PATHNAME)

    @property
    def memory_size(self):
        return Vix_GetProperties(self._vm, VIX_PROPERTY_VM_MEMORY_SIZE)

    @property
    def read_only(self):
        return Vix_GetProperties(self._vm, VIX_PROPERTY_VM_READ_ONLY)

    @property
    def name(self):
        return Vix_GetProperties(self._vm, VIX_PROPERTY_VM_NAME)

    @property
    def guestos(self):
        return Vix_GetProperties(self._vm, VIX_PROPERTY_VM_GUESTOS)

    @property
    def power_state(self):
        state_desc = {VIX_POWERSTATE_POWERING_OFF   : "Powering Off",
                      VIX_POWERSTATE_POWERED_OFF    : "Powered Off",
                      VIX_POWERSTATE_POWERING_ON    : "Powering On",
                      VIX_POWERSTATE_POWERED_ON     : "Powered On",
                      VIX_POWERSTATE_SUSPENDING     : "Suspending",
                      VIX_POWERSTATE_SUSPENDED      : "Suspended",
                      VIX_POWERSTATE_TOOLS_RUNNING  : "Tools Running",
                      VIX_POWERSTATE_RESETTING      : "Resetting",
                      VIX_POWERSTATE_BLOCKED_ON_MSG : "Blocked Msg",
                      VIX_POWERSTATE_PAUSED         : "Paused",
                      VIX_POWERSTATE_RESUMING       : "Resuming"}
        state = Vix_GetProperties(self._vm, VIX_PROPERTY_VM_POWER_STATE)
        try:
            return state_desc[state]
        except KeyError:
            return "Unknown - " + str(state)

    @property
    def tools_state(self):
        state_desc = {VIX_TOOLSSTATE_UNKNOWN        : "Unknown",
                      VIX_TOOLSSTATE_RUNNING        : "Running",
                      VIX_TOOLSSTATE_NOT_INSTALLED  : "Not Installed"}
        state = Vix_GetProperties(self._vm, VIX_PROPERTY_VM_TOOLS_STATE)
        try:
            return state_desc[state]
        except KeyError:
            return "Unknown - " + str(state)

    @property
    def is_running(self):
        return Vix_GetProperties(self._vm, VIX_PROPERTY_VM_IS_RUNNING)

    @property
    def shares_path(self):
        return Vix_GetProperties(self._vm,
                VIX_PROPERTY_GUEST_SHAREDFOLDERS_SHARES_PATH)


# XXX - Children should be a list
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
        return Vix_GetProperties(self._snap, VIX_PROPERTY_SNAPSHOT_DISPLAYNAME)

    @property
    def description(self):
        return Vix_GetProperties(self._snap, VIX_PROPERTY_SNAPSHOT_DESCRIPTION)

    @property
    def is_replayable(self):
        return Vix_GetProperties(self._snap, VIX_PROPERTY_SNAPSHOT_IS_REPLAYABLE)


