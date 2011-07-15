#include <Python.h>
#include <vix.h>

#ifndef VIX_E_SNAPSHOT_RRSUSPEND
#define VIX_E_SNAPSHOT_RRSUSPEND 13021
#endif

struct IntConstantList {
    char *name;
    long val;
};

static int
PyModule_AddUnsignedLongConstant(PyObject *m, const char *name, unsigned long value)
{
    PyObject *o = PyLong_FromUnsignedLong(value);
    if (!o)
        return -1;
    if (PyModule_AddObject(m, name, o) == 0)
        return 0;
    Py_DECREF(o);
    return -1;
}

static PyObject *err_vix(VixError err)
{
    PyObject *m = NULL;
    PyObject *dict = NULL;
    PyObject *exc = NULL;
    const char *msg = NULL;

    m = PyImport_AddModule("_vixpy");
    if (m == NULL || (dict = PyModule_GetDict(m)) == NULL)
        return NULL;

    exc = PyDict_GetItemString(dict, "VixError");
    msg = Vix_GetErrorText(VIX_ERROR_CODE(err), NULL);
    if (msg == NULL)
        msg = "Unknown";
    
    return PyErr_Format(exc, "%s : (%d)", msg, (int)VIX_ERROR_CODE(err));
}

static PyObject *
pv_Vix_GetErrorText(PyObject *self, PyObject *args)
{
    VixError err;
    const char *msg;

    if (!PyArg_ParseTuple(args, "K:Vix_GetErrorText", &err))
        return NULL;

    msg = Vix_GetErrorText(VIX_ERROR_CODE(err), NULL);
    if (msg == NULL)
        Py_RETURN_NONE;
    else
        return Py_BuildValue("s", msg);
}

static PyObject *
pv_Vix_ReleaseHandle(PyObject *self, PyObject *args)
{
    VixHandle handle = VIX_INVALID_HANDLE;
    if (!PyArg_ParseTuple(args, "i:Vix_ReleaseHandle", &handle))
        return NULL;

    Vix_ReleaseHandle(handle);
    Py_RETURN_NONE;
}

static PyObject *
pv_Vix_AddRefHandle(PyObject *self, PyObject *args)
{
    VixHandle handle = VIX_INVALID_HANDLE;
    if (!PyArg_ParseTuple(args, "i:Vix_AddRefHandle", &handle))
        return NULL;

    Vix_AddRefHandle(handle);
    Py_RETURN_NONE;
}

static PyObject *
pv_Vix_GetHandleType(PyObject *self, PyObject *args)
{
    VixHandle handle = VIX_INVALID_HANDLE;
    VixHandleType type = VIX_HANDLETYPE_NONE;

    if (!PyArg_ParseTuple(args, "i:Vix_GetHandleType", &handle))
        return NULL;

    type = Vix_GetHandleType(handle);
    return Py_BuildValue("i", type);;
}

static PyObject *
pv_Vix_GetProperties(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle handle = VIX_INVALID_HANDLE;
    VixPropertyID property = VIX_PROPERTY_NONE;
    VixPropertyType type = VIX_PROPERTYTYPE_ANY;
    uint64 propval = 0;

    if (!PyArg_ParseTuple(args, "ii:Vix_GetProperties", &handle, &property))
        return NULL;

    if ((err = Vix_GetPropertyType(handle, property, &type)) != VIX_OK)
        goto error;

    switch (type) {
    case VIX_PROPERTYTYPE_INTEGER:
    case VIX_PROPERTYTYPE_HANDLE:
    {
        int val;
        if ((err = Vix_GetProperties(handle, property, 
                        &val, VIX_PROPERTY_NONE)) != VIX_OK) 
        {
            goto error;
        }
        return Py_BuildValue("i", val);
    }
    case VIX_PROPERTYTYPE_STRING:
    {
        PyObject *ret;
        char *val;
        if ((err = Vix_GetProperties(handle, property, 
                        &val, VIX_PROPERTY_NONE)) != VIX_OK) 
        {
            goto error;
        }
        ret = Py_BuildValue("s", val);
        Vix_FreeBuffer(val);
        return ret;
    }
    case VIX_PROPERTYTYPE_BOOL:
    {
        Bool val;
        if ((err = Vix_GetProperties(handle, property, 
                        &val, VIX_PROPERTY_NONE)) != VIX_OK) 
        {
            goto error;
        }
        if (val)
            Py_RETURN_TRUE;
        else
            Py_RETURN_FALSE;
    }
    case VIX_PROPERTYTYPE_INT64:
    {
        int64 val;
        if ((err = Vix_GetProperties(handle, property, 
                        &val, VIX_PROPERTY_NONE)) != VIX_OK) 
        {
            goto error;
        }
        return Py_BuildValue("k", val);
    }
    case VIX_PROPERTYTYPE_BLOB:
    {
        PyObject *ret;
        char *val;
        int size;
        if ((err = Vix_GetProperties(handle, property, 
                        &size, &val, VIX_PROPERTY_NONE)) != VIX_OK) 
        {
            goto error;
        }
        ret = Py_BuildValue("s#", val, size);
        Vix_FreeBuffer(val);
        return ret;
    }
    default:
        goto error;
    }

error:
    return err_vix(err);
}

static PyObject *
pv_Vix_GetPropertyType(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle handle = VIX_INVALID_HANDLE;
    VixPropertyID property = VIX_PROPERTY_NONE;
    VixPropertyType type = VIX_PROPERTYTYPE_ANY;

    if (!PyArg_ParseTuple(args, "ii:Vix_GetPropertyType", &handle, &property))
        return NULL;

    if ((err = Vix_GetPropertyType(handle, property, &type)) != VIX_OK)
        return err_vix(err);

    return Py_BuildValue("i", type);;
}

static PyObject *
pv_VixHost_Connect(PyObject *self, PyObject *args)
{
    VixHandle job = VIX_INVALID_HANDLE;
    VixHandle handle = VIX_INVALID_HANDLE;
    VixError err;
    VixServiceProvider hostType = VIX_SERVICEPROVIDER_DEFAULT;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "i:VixHost_Connect", &hostType))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixHost_Connect(VIX_API_VERSION, hostType, NULL, 0, NULL, NULL,
                          0, VIX_INVALID_HANDLE, NULL, NULL);

    err = VixJob_Wait(job, VIX_PROPERTY_JOB_RESULT_HANDLE, &handle,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    if (err != VIX_OK) {
        err_vix(err);
        goto error;
    }

    ret = Py_BuildValue("i", handle);

error:
    if (job != VIX_INVALID_HANDLE)
        Vix_ReleaseHandle(job);
    return ret;
}

static PyObject *
pv_VixHost_Disconnect(PyObject *self, PyObject *args)
{
    VixHandle handle = VIX_INVALID_HANDLE;
    if (!PyArg_ParseTuple(args, "i:VixHost_Disconnect", &handle))
        return NULL;

    VixHost_Disconnect(handle);
    Py_RETURN_NONE;
}

/* Not used on workstation and player */
static PyObject *
pv_VixHost_RegisterVM(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle handle = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *path = NULL;
    if (!PyArg_ParseTuple(args, "is:VixHost_RegisterVM", &handle, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixHost_RegisterVM(handle, path, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

/* Not used on workstation and player */
static PyObject *
pv_VixHost_UnregisterVM(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle handle = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *path = NULL;
    if (!PyArg_ParseTuple(args, "is:VixHost_UnregisterVM", &handle, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixHost_UnregisterVM(handle, path, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static void 
VixFindItemsProc(VixHandle job,
                 VixEventType event,
                 VixHandle moreEventInfo,
                 void *clientData)
{
    VixError err = VIX_OK;
    char *url = NULL;

    if (event != VIX_EVENTTYPE_FIND_ITEM)
        return;

    err = Vix_GetProperties(moreEventInfo,
            VIX_PROPERTY_FOUND_ITEM_LOCATION,
            &url,
            VIX_PROPERTY_NONE);
    if (err == VIX_OK) {
        PyObject *item = NULL;
        item = Py_BuildValue("s", url);
        PyList_Append((PyObject *)clientData, item);
        Py_XDECREF(item);
    }

    Vix_FreeBuffer(url);
}


static PyObject *
pv_VixHost_FindItems(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle handle = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    PyObject *ret = NULL;

    ret = PyList_New(0);
    if (ret == NULL) { 
        PyErr_NoMemory();
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "i:VixHost_FindItems", &handle)) {
        Py_CLEAR(ret);
        return NULL;
    }

    job = VixHost_FindItems(
            handle,
            VIX_FIND_RUNNING_VMS,
            VIX_INVALID_HANDLE,
            -1,
            VixFindItemsProc,
            ret);
                
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(job);

    if (err != VIX_OK) {
        Py_CLEAR(ret);
        return err_vix(err);
    }
    return ret;
}

static PyObject *
pv_VixHost_OpenVM(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle handle = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixHandle vm = VIX_INVALID_HANDLE;
    char *path = NULL;

    if (!PyArg_ParseTuple(args, "is:VixHost_OpenVM", &handle, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixHost_OpenVM(handle,
            path,
            VIX_VMOPEN_NORMAL,
            VIX_INVALID_HANDLE,
            NULL,
            NULL);

    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_HANDLE,
            &vm,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    else
        return Py_BuildValue("i", vm);
}

static PyObject *
pv_VixVM_PowerOn(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixVMPowerOpOptions options = VIX_VMPOWEROP_NORMAL;

    if (!PyArg_ParseTuple(args, "i|i:VixVM_PowerOn", &vm, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_PowerOn(vm, options, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_PowerOff(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixVMPowerOpOptions options = VIX_VMPOWEROP_NORMAL;

    if (!PyArg_ParseTuple(args, "i|i:VixVM_PowerOff", &vm, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_PowerOff(vm, options, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_Reset(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixVMPowerOpOptions options = VIX_VMPOWEROP_NORMAL;

    if (!PyArg_ParseTuple(args, "i|i:VixVM_Reset", &vm, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_Reset(vm, options, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_Suspend(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixVM_Suspend", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_Suspend(vm, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_Pause(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixVM_Pause", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_Pause(vm, 0, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_Unpause(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixVM_Unpause", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_Unpause(vm, 0, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_Delete(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int options = 0;

    if (!PyArg_ParseTuple(args, "ii:VixVM_Delete", &vm, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_Delete(vm, options, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}


static PyObject *
pv_VixVM_BeginRecording(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *name = NULL;
    char *desc = NULL;

    if (!PyArg_ParseTuple(args, "iss:VixVM_BeginRecording", 
                &vm, &name, &desc))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_BeginRecording(vm, name, desc, 0, 
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_EndRecording(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixVM_EndRecording", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_EndRecording(vm, 0, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_BeginReplay(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixHandle snapshot = VIX_INVALID_HANDLE;
    int options = 0;

    if (!PyArg_ParseTuple(args, "iii:VixVM_BeginReplay", 
                &vm, &snapshot, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_BeginReplay(vm, snapshot, options,
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_EndReplay(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixVM_EndReplay", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_EndReplay(vm, 0, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_WaitForToolsInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int timeout = 0;

    if (!PyArg_ParseTuple(args, 
                "i|i:VixVM_WaitForToolsInGuest", &vm, &timeout))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_WaitForToolsInGuest(vm, timeout, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    // XXX - How to differentiate timeout from error?
    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_LoginInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *user = NULL;
    char *pass = NULL;
    int options = 0;

    if (!PyArg_ParseTuple(args, 
                "iss|i:VixVM_LoginInGuest", &vm, &user, &pass, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_LoginInGuest(vm, user, pass, options, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_LogoutFromGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixVM_LogoutFromGuest", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_LogoutFromGuest(vm, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_RunProgramInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *prog = NULL;
    char *cmdLine = NULL;
    int options = 0;
    uint64 pid = 0;
    int exitcode = 0;

    if (!PyArg_ParseTuple(args, "issi:VixVM_RunProgramInGuest", 
                &vm, &prog, &cmdLine, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_RunProgramInGuest(vm, prog, cmdLine, options,
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_PROCESS_ID,
            &pid,
            VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE,
            &exitcode,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    // XXX - What about return code and the other results?
    if (err != VIX_OK)
        return err_vix(err);

    return Py_BuildValue("KI", pid, exitcode);
}

static PyObject *
pv_VixVM_ListProcessesInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int i;
    int num;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "i:VixVM_ListProcessesInGuest", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_ListProcessesInGuest(vm, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    if (err != VIX_OK)
        return err_vix(err);

    ret = PyList_New(0);
    if (ret == NULL) { 
        Vix_ReleaseHandle(job);
        PyErr_NoMemory();
        return NULL;
    }

    num = VixJob_GetNumProperties(job, VIX_PROPERTY_JOB_RESULT_ITEM_NAME);
    for (i = 0; i < num; i++) {
        PyObject *item = NULL;
        char *processName;
        uint64 pid;
        char *owner;
        char *cmdline;

        err = VixJob_GetNthProperties(job, i,
                VIX_PROPERTY_JOB_RESULT_ITEM_NAME, &processName,
                VIX_PROPERTY_JOB_RESULT_PROCESS_ID, &pid,
                VIX_PROPERTY_JOB_RESULT_PROCESS_OWNER, &owner,
                VIX_PROPERTY_JOB_RESULT_PROCESS_COMMAND, &cmdline,
                VIX_PROPERTY_NONE);

        item = Py_BuildValue("sKss", processName, pid, owner, cmdline);
        PyList_Append(ret, item);
        Py_XDECREF(item);

        Vix_FreeBuffer(processName);
        Vix_FreeBuffer(owner);
        Vix_FreeBuffer(cmdline);
    }
    Vix_ReleaseHandle(job);
    return ret;
}

static PyObject *
pv_VixVM_KillProcessInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    uint64 pid = 0;

    if (!PyArg_ParseTuple(args, "iK:VixVM_KillProcessInGuest", &vm, &pid))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_KillProcessInGuest(vm, pid, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_RunScriptInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *interpreter = NULL;
    char *scriptText = NULL;
    VixRunProgramOptions options = 0;
    uint64 pid = 0;
    int exitcode = 0;

    if (!PyArg_ParseTuple(args, "issi:VixVM_RunScriptInGuest", 
                &vm, &interpreter, &scriptText, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_RunScriptInGuest(vm, interpreter, scriptText, options,
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_PROCESS_ID,
            &pid,
            VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE,
            &exitcode,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    return Py_BuildValue("KI", pid, exitcode);
}

static PyObject *
pv_VixVM_CopyFileFromHostToGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *hostPath = NULL;
    char *guestPath = NULL;

    if (!PyArg_ParseTuple(args, "iss:VixVM_CopyFileFromHostToGuest", 
                &vm, &hostPath, &guestPath))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_CopyFileFromHostToGuest(vm, hostPath, guestPath, 0,
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_CopyFileFromGuestToHost(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *hostPath = NULL;
    char *guestPath = NULL;

    if (!PyArg_ParseTuple(args, "iss:VixVM_CopyFileFromGuestToHost", 
                &vm, &guestPath, &hostPath))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_CopyFileFromGuestToHost(vm, guestPath, hostPath, 0,
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_DeleteFileInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *guestPath = NULL;

    if (!PyArg_ParseTuple(args, "is:VixVM_DeleteFileInGuest", 
                &vm, &guestPath))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_DeleteFileInGuest(vm, guestPath, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_FileExistsInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *guestPath = NULL;
    int exists = 0;

    if (!PyArg_ParseTuple(args, "is:VixVM_FileExistsInGuest", 
                &vm, &guestPath))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_FileExistsInGuest(vm, guestPath, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS,
            &exists,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    if (exists)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
pv_VixVM_RenameFileInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *oldName = NULL;
    char *newName = NULL;

    if (!PyArg_ParseTuple(args, "is:VixVM_RenameFileInGuest",
                &vm, &oldName, &newName))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_RenameFileInGuest(vm, oldName, newName,
            0, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_CreateTempFileInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *tmpFile = NULL;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "i:VixVM_CreateTempFileInGuest", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_CreateTempFileInGuest(vm, 0, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
            &tmpFile,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    ret = Py_BuildValue("s", tmpFile);
    Vix_FreeBuffer(tmpFile);
    return ret;
}

static PyObject *
pv_VixVM_GetFileInfoInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *path = NULL;
    int fileFlags;
    int64 fileSize;

    if (!PyArg_ParseTuple(args, "is:VixVM_GetFileInfoInGuest", &vm, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_GetFileInfoInGuest(vm, path, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_FILE_FLAGS,
            &fileFlags,
            VIX_PROPERTY_JOB_RESULT_FILE_SIZE,
            &fileSize,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("ik", fileFlags, fileSize);
}

static PyObject *
pv_VixVM_ListDirectoryInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *path = NULL;
    int num;
    int i;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "is:VixVM_ListDirectoryInGuest", &vm, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_ListDirectoryInGuest(vm, path, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    if (err != VIX_OK) {
        err_vix(err);
        goto error;
    }

    ret = PyList_New(0);
    if (ret == NULL) { 
        PyErr_NoMemory();
        goto error;
    }

    num = VixJob_GetNumProperties(job, VIX_PROPERTY_JOB_RESULT_ITEM_NAME);
    for (i = 0; i < num; i++) {
        char *fname = NULL;
        PyObject *item = NULL;

        err = VixJob_GetNthProperties(job,
                i,
                VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
                &fname,
                VIX_PROPERTY_NONE);

        if (err == VIX_OK) {
            item = Py_BuildValue("s", fname);
            PyList_Append(ret, item);
            Py_XDECREF(item);
        }
        Vix_FreeBuffer(fname);
    }

error:
    Vix_ReleaseHandle(job);
    return ret;
}

static PyObject *
pv_VixVM_CreateDirectoryInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *path = NULL;

    if (!PyArg_ParseTuple(args, "is:VixVM_CreateDirectoryInGuest", &vm, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_CreateDirectoryInGuest(vm, path, VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_DeleteDirectoryInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *path = NULL;

    if (!PyArg_ParseTuple(args, "is:VixVM_DeleteDirectoryInGuest", &vm, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_DeleteDirectoryInGuest(vm, path, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_DirectoryExistsInGuest(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *path = NULL;
    int exists = 0;

    if (!PyArg_ParseTuple(args, "is:VixVM_DirectoryExistsInGuest", &vm, &path))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_DirectoryExistsInGuest(vm, path, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS,
            &exists,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    if (exists)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
pv_VixVM_ReadVariable(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int type;
    char *name;
    char *value = NULL;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "iis:VixVM_ReadVariable", &vm, &type, &name))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_ReadVariable(vm, type, name, 0, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING,
            &value,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    ret = Py_BuildValue("s", value);
    Vix_FreeBuffer(value);
    return ret;
}

static PyObject *
pv_VixVM_WriteVariable(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int type;
    char *name;
    char *value;

    if (!PyArg_ParseTuple(args, "iiss:VixVM_WriteVariable", 
                &vm, &type, &name, &value))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_WriteVariable(vm, type, name, value, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_EnableSharedFolders(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    Bool enabled = 0;

    if (!PyArg_ParseTuple(args, "ii:VixVM_EnableSharedFolders", &vm, &enabled))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_EnableSharedFolders(vm, enabled, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_GetNumSharedFolders(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int num;

    if (!PyArg_ParseTuple(args, "i:VixVM_GetNumSharedFolders", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_GetNumSharedFolders(vm, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_COUNT,
            &num,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", num);
}

static PyObject *
pv_VixVM_GetSharedFolderState(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int index = 0;
    char *name = NULL;
    char *hostPath = NULL;
    int flags = 0;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "ii:VixVM_GetSharedFolderState", &vm, &index))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_GetSharedFolderState(vm, index, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
            &name,
            VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_HOST,
            &hostPath,
            VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_FLAGS,
            &flags,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);

    ret = Py_BuildValue("ssi", name, hostPath, flags);
    Vix_FreeBuffer(name);
    Vix_FreeBuffer(hostPath);
    return ret;
}

static PyObject *
pv_VixVM_SetSharedFolderState(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *name = NULL;
    char *hostPath = NULL;
    VixMsgSharedFolderOptions flags = 0;

    if (!PyArg_ParseTuple(args, "issi:VixVM_SetSharedFolderState", 
                &vm, &name, &hostPath, &flags))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_SetSharedFolderState(vm, name, hostPath, flags, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_AddSharedFolder(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *name = NULL;
    char *hostPath = NULL;
    VixMsgSharedFolderOptions flags = 0;

    if (!PyArg_ParseTuple(args, "issi:VixVM_AddSharedFolder", 
                &vm, &name, &hostPath, &flags))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_AddSharedFolder(vm, name, hostPath, flags, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_RemoveSharedFolder(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *name = NULL;

    if (!PyArg_ParseTuple(args, "is:VixVM_RemoveSharedFolder", 
                &vm, &name))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_RemoveSharedFolder(vm, name, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}



static PyObject *
pv_VixVM_GetNumRootSnapshots(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    int result;

    if (!PyArg_ParseTuple(args, "i:VixVM_GetNumRootSnapshots", &vm))
        return NULL;

    err = VixVM_GetNumRootSnapshots(vm, &result);
    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", result);
}

static PyObject *
pv_VixVM_GetRootSnapshot(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    int index;
    VixHandle snapshot;

    if (!PyArg_ParseTuple(args, "ii:VixVM_GetRootSnapshot", &vm, &index))
        return NULL;

    err = VixVM_GetRootSnapshot(vm, index, &snapshot);
    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", snapshot);
}

static PyObject *
pv_VixVM_GetCurrentSnapshot(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle snapshot;

    if (!PyArg_ParseTuple(args, "i:VixVM_GetCurrentSnapshot", &vm))
        return NULL;

    err = VixVM_GetCurrentSnapshot(vm, &snapshot);
    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", snapshot);
}

static PyObject *
pv_VixVM_GetNamedSnapshot(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    char *name = NULL;
    VixHandle snapshot;

    if (!PyArg_ParseTuple(args, "is:VixVM_GetNamedSnapshot", &vm, &name))
        return NULL;

    err = VixVM_GetNamedSnapshot(vm, name, &snapshot);
    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", snapshot);
}

static PyObject *
pv_VixVM_RemoveSnapshot(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixHandle snapshot = VIX_INVALID_HANDLE;
    int options = 0;

    if (!PyArg_ParseTuple(args, "ii|i:VixVM_RemoveSnapshot", 
                &vm, &snapshot, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_RemoveSnapshot(vm, snapshot, options, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);
    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_RevertToSnapshot(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixHandle snapshot = VIX_INVALID_HANDLE;
    int options = 0;

    if (!PyArg_ParseTuple(args, "ii|i:VixVM_RevertToSnapshot", 
                &vm, &snapshot, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_RevertToSnapshot(vm, snapshot, options, 
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_CreateSnapshot(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    VixHandle snapshot = VIX_INVALID_HANDLE;
    char *name = NULL;
    char *desc = NULL;
    int options = VIX_SNAPSHOT_INCLUDE_MEMORY;

    if (!PyArg_ParseTuple(args, "iss|i:VixVM_CreateSnapshot", 
                &vm, &name, &desc, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_CreateSnapshot(vm, name, desc, options,
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_HANDLE,
            &snapshot,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", snapshot);
}

static PyObject *
pv_VixSnapshot_GetNumChildren(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle snapshot = VIX_INVALID_HANDLE;
    int result;

    if (!PyArg_ParseTuple(args, "i:VixSnapshot_GetNumChildren", &snapshot))
        return NULL;

    err = VixSnapshot_GetNumChildren(snapshot, &result);
    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", result);
}

static PyObject *
pv_VixSnapshot_GetChild(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle snapshot = VIX_INVALID_HANDLE;
    VixHandle child = VIX_INVALID_HANDLE;
    int index;

    if (!PyArg_ParseTuple(args, "ii:VixSnapshot_GetChild", 
                &snapshot, &index))
        return NULL;

    err = VixSnapshot_GetChild(snapshot, index, &child);
    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", child);
}

static PyObject *
pv_VixSnapshot_GetParent(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle snapshot = VIX_INVALID_HANDLE;
    VixHandle parent = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixSnapshot_GetParent", &snapshot))
        return NULL;

    err = VixSnapshot_GetParent(snapshot, &parent);
    if (err != VIX_OK)
        return err_vix(err);
    return Py_BuildValue("i", parent);
}

static PyObject *
pv_VixVM_CaptureScreenImage(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    char *bits = NULL;
    int size = 0;
    PyObject *ret = NULL;

    if (!PyArg_ParseTuple(args, "i:VixVM_CaptureScreenImage", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_CaptureScreenImage(vm, VIX_CAPTURESCREENFORMAT_PNG,
            VIX_INVALID_HANDLE, NULL, NULL);
    err = VixJob_Wait(job, 
            VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_DATA,
            &size, &bits,
            VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    ret = Py_BuildValue("s#", bits, size);
    Vix_FreeBuffer(bits);
    return ret;
}

static PyObject *
pv_VixVM_UpgradeVirtualHardware(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;

    if (!PyArg_ParseTuple(args, "i:VixVM_UpgradeVirtualHardware", &vm))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_UpgradeVirtualHardware(vm, 0, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}

static PyObject *
pv_VixVM_InstallTools(PyObject *self, PyObject *args)
{
    VixError err = VIX_OK;
    VixHandle vm = VIX_INVALID_HANDLE;
    VixHandle job = VIX_INVALID_HANDLE;
    int options;

    if (!PyArg_ParseTuple(args, "ii:VixVM_InstallTools", &vm, &options))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    job = VixVM_InstallTools(vm, options, NULL, NULL, NULL);
    err = VixJob_Wait(job, VIX_PROPERTY_NONE);
    Py_END_ALLOW_THREADS

    Vix_ReleaseHandle(job);

    if (err != VIX_OK)
        return err_vix(err);
    Py_RETURN_NONE;
}


static PyMethodDef  VixPyMethods[] = {
    {"Vix_GetErrorText", 
     (PyCFunction)pv_Vix_GetErrorText, METH_VARARGS, 
     "Vix_GetErrorText(err)"
    },
    {"Vix_ReleaseHandle", 
     (PyCFunction)pv_Vix_ReleaseHandle, METH_VARARGS, 
     "Vix_ReleaseHandle(handle)"
    },
    {"Vix_AddRefHandle", 
     (PyCFunction)pv_Vix_AddRefHandle, METH_VARARGS, 
     "Vix_AddRefHandle(handle)"
    },
    {"Vix_GetHandleType", 
     (PyCFunction)pv_Vix_GetHandleType, METH_VARARGS, 
     "Vix_GetHandleType(handle)"
    },
    {"Vix_GetProperties", 
     (PyCFunction)pv_Vix_GetProperties, METH_VARARGS, 
     "Vix_GetProperties(handle, property)"
    },
    {"Vix_GetPropertyType", 
     (PyCFunction)pv_Vix_GetPropertyType, METH_VARARGS, 
     "Vix_GetPropertyType(handle, property)"
    },

    {"VixHost_Connect", 
     (PyCFunction)pv_VixHost_Connect, METH_VARARGS,
     "VixHost_Connect(hostType)"
    },
    {"VixHost_Disconnect", 
     (PyCFunction)pv_VixHost_Disconnect, METH_VARARGS,
     "VixHost_Disconnect(hostHandle)"
    },
    {"VixHost_RegisterVM", 
     (PyCFunction)pv_VixHost_RegisterVM, METH_VARARGS,
     "VixHost_RegisterVM(hostHandle, vmxFilePath)"
    },
    {"VixHost_UnregisterVM", 
     (PyCFunction)pv_VixHost_UnregisterVM, METH_VARARGS,
     "VixHost_UnregisterVM(hostHandle, vmxFilePath)"
    },
    {"VixHost_FindItems", 
     (PyCFunction)pv_VixHost_FindItems, METH_VARARGS,
     "VixHost_FindItems(hostHandle)"
    },
    {"VixHost_OpenVM", 
     (PyCFunction)pv_VixHost_OpenVM, METH_VARARGS,
     "VixHost_OpenVM(hostHandle, vmxFilePath)"
    },

    {"VixVM_PowerOn", 
     (PyCFunction)pv_VixVM_PowerOn, METH_VARARGS,
     "VixVM_PowerOn(vmHandle, options=VIX_VMPOWEROP_NORMAL)"
    },
    {"VixVM_PowerOff", 
     (PyCFunction)pv_VixVM_PowerOff, METH_VARARGS,
     "VixVM_PowerOff(vmHandle, options=VIX_VMPOWEROP_NORMAL)"
    },
    {"VixVM_Reset", 
     (PyCFunction)pv_VixVM_Reset, METH_VARARGS,
     "VixVM_Reset(vmHandle, options=VIX_VMPOWEROP_NORMAL)"
    },
    {"VixVM_Suspend", 
     (PyCFunction)pv_VixVM_Suspend, METH_VARARGS,
     "VixVM_Suspend(vmHandle)"
    },
    {"VixVM_Pause", 
     (PyCFunction)pv_VixVM_Pause, METH_VARARGS,
     "VixVM_Pause(vmHandle)"
    },
    {"VixVM_Unpause", 
     (PyCFunction)pv_VixVM_Unpause, METH_VARARGS,
     "VixVM_Unpause(vmHandle)"
    },
    {"VixVM_Delete", 
     (PyCFunction)pv_VixVM_Delete, METH_VARARGS,
     "VixVM_Delete(vmHandle, options)"
    },

    {"VixVM_BeginRecording", 
     (PyCFunction)pv_VixVM_BeginRecording, METH_VARARGS,
     "VixVM_BeginRecording(vmHandle, name, desc)"
    },
    {"VixVM_EndRecording", 
     (PyCFunction)pv_VixVM_EndRecording, METH_VARARGS,
     "VixVM_EndRecording(vmHandle)"
    },
    {"VixVM_BeginReplay", 
     (PyCFunction)pv_VixVM_BeginReplay, METH_VARARGS,
     "VixVM_BeginReplay(vmHandle, snapshotHandle, options)"
    },
    {"VixVM_EndReplay", 
     (PyCFunction)pv_VixVM_EndReplay, METH_VARARGS,
     "VixVM_EndReplay(vmHandle)"
    },

    {"VixVM_WaitForToolsInGuest", 
     (PyCFunction)pv_VixVM_WaitForToolsInGuest, METH_VARARGS,
     "VixVM_WaitForToolsInGuest(vmHandle, timeout=0)"
    },
    {"VixVM_LoginInGuest", 
     (PyCFunction)pv_VixVM_LoginInGuest, METH_VARARGS,
     "VixVM_LoginInGuest(vmHandle, user, pass, options=0)"
    },
    {"VixVM_LogoutFromGuest", 
     (PyCFunction)pv_VixVM_LogoutFromGuest, METH_VARARGS,
     "VixVM_LogoutFromGuest(vmHandle)"
    },

    {"VixVM_RunProgramInGuest", 
     (PyCFunction)pv_VixVM_RunProgramInGuest, METH_VARARGS,
     "VixVM_RunProgramInGuest(vmHandle, prog, args, options)"
    },
    {"VixVM_ListProcessesInGuest", 
     (PyCFunction)pv_VixVM_ListProcessesInGuest, METH_VARARGS,
     "VixVM_ListProcessesInGuest(vmHandle)"
    },
    {"VixVM_KillProcessInGuest", 
     (PyCFunction)pv_VixVM_KillProcessInGuest, METH_VARARGS,
     "VixVM_KillProcessInGuest(vmHandle, pid)"
    },
    {"VixVM_RunScriptInGuest", 
     (PyCFunction)pv_VixVM_RunScriptInGuest, METH_VARARGS,
     "VixVM_RunScriptInGuest(vmHandle, interpreter, script_text, options)"
    },

    /* Deprecated
    {"VixVM_OpenUrlInGuest", 
     (PyCFunction)pv_VixVM_OpenUrlInGuest, METH_VARARGS,
     "VixVM_OpenUrlInGuest(vmHandle)"
    },
    */

    {"VixVM_CopyFileFromHostToGuest", 
     (PyCFunction)pv_VixVM_CopyFileFromHostToGuest, METH_VARARGS,
     "VixVM_CopyFileFromHostToGuest(vmHandle, src, dst)"
    },
    {"VixVM_CopyFileFromGuestToHost", 
     (PyCFunction)pv_VixVM_CopyFileFromGuestToHost, METH_VARARGS,
     "VixVM_CopyFileFromGuestToHost(vmHandle, src, dst)"
    },
    {"VixVM_DeleteFileInGuest", 
     (PyCFunction)pv_VixVM_DeleteFileInGuest, METH_VARARGS,
     "VixVM_DeleteFileInGuest(vmHandle, path)"
    },
    {"VixVM_FileExistsInGuest", 
     (PyCFunction)pv_VixVM_FileExistsInGuest, METH_VARARGS,
     "VixVM_FileExistsInGuest(vmHandle, path)"
    },
    {"VixVM_RenameFileInGuest", 
     (PyCFunction)pv_VixVM_RenameFileInGuest, METH_VARARGS,
     "VixVM_RenameFileInGuest(vmHandle, old, new)"
    },
    {"VixVM_CreateTempFileInGuest", 
     (PyCFunction)pv_VixVM_CreateTempFileInGuest, METH_VARARGS,
     "VixVM_CreateTempFileInGuest(vmHandle)"
    },
    {"VixVM_GetFileInfoInGuest", 
     (PyCFunction)pv_VixVM_GetFileInfoInGuest, METH_VARARGS,
     "VixVM_GetFileInfoInGuest(vmHandle, path)"
    },
    {"VixVM_ListDirectoryInGuest", 
     (PyCFunction)pv_VixVM_ListDirectoryInGuest, METH_VARARGS,
     "VixVM_ListDirectoryInGuest(vmHandle, path)"
    },
    {"VixVM_CreateDirectoryInGuest", 
     (PyCFunction)pv_VixVM_CreateDirectoryInGuest, METH_VARARGS,
     "VixVM_CreateDirectoryInGuest(vmHandle, path)"
    },
    {"VixVM_DeleteDirectoryInGuest", 
     (PyCFunction)pv_VixVM_DeleteDirectoryInGuest, METH_VARARGS,
     "VixVM_DeleteDirectoryInGuest(vmHandle, path)"
    },
    {"VixVM_DirectoryExistsInGuest", 
     (PyCFunction)pv_VixVM_DirectoryExistsInGuest, METH_VARARGS,
     "VixVM_DirectoryExistsInGuest(vmHandle, path)"
    },
    {"VixVM_ReadVariable", 
     (PyCFunction)pv_VixVM_ReadVariable, METH_VARARGS,
     "VixVM_ReadVariable(vmHandle, type, name)"
    },
    {"VixVM_WriteVariable", 
     (PyCFunction)pv_VixVM_WriteVariable, METH_VARARGS,
     "VixVM_WriteVariable(vmHandle, type, name, value)"
    },

    {"VixVM_EnableSharedFolders", 
     (PyCFunction)pv_VixVM_EnableSharedFolders, METH_VARARGS,
     "VixVM_EnableSharedFolders(vmHandle, flag)"
    },
    {"VixVM_GetNumSharedFolders", 
     (PyCFunction)pv_VixVM_GetNumSharedFolders, METH_VARARGS,
     "VixVM_GetNumSharedFolders(vmHandle)"
    },
    {"VixVM_GetSharedFolderState", 
     (PyCFunction)pv_VixVM_GetSharedFolderState, METH_VARARGS,
     "VixVM_GetSharedFolderState(vmHandle, index)"
    },
    {"VixVM_SetSharedFolderState", 
     (PyCFunction)pv_VixVM_SetSharedFolderState, METH_VARARGS,
     "VixVM_SetSharedFolderState(vmHandle, name, hostpath, options)"
    },
    {"VixVM_AddSharedFolder", 
     (PyCFunction)pv_VixVM_AddSharedFolder, METH_VARARGS,
     "VixVM_AddSharedFolder(vmHandle, name, hostpath, options)"
    },
    {"VixVM_RemoveSharedFolder", 
     (PyCFunction)pv_VixVM_RemoveSharedFolder, METH_VARARGS,
     "VixVM_RemoveSharedFolder(vmHandle)"
    },

    {"VixVM_GetNumRootSnapshots", 
     (PyCFunction)pv_VixVM_GetNumRootSnapshots, METH_VARARGS,
     "VixVM_GetNumRootSnapshots(vmHandle)"
    },
    {"VixVM_GetRootSnapshot", 
     (PyCFunction)pv_VixVM_GetRootSnapshot, METH_VARARGS,
     "VixVM_GetRootSnapshot(vmHandle, index)"
    },
    {"VixVM_GetCurrentSnapshot", 
     (PyCFunction)pv_VixVM_GetCurrentSnapshot, METH_VARARGS,
     "VixVM_GetCurrentSnapshot(vmHandle)"
    },
    {"VixVM_GetNamedSnapshot", 
     (PyCFunction)pv_VixVM_GetNamedSnapshot, METH_VARARGS,
     "VixVM_GetNamedSnapshot(vmHandle, name)"
    },
    {"VixVM_RemoveSnapshot", 
     (PyCFunction)pv_VixVM_RemoveSnapshot, METH_VARARGS,
     "VixVM_RemoveSnapshot(vmHandle, snapshotHandle, options)"
    },
    {"VixVM_RevertToSnapshot", 
     (PyCFunction)pv_VixVM_RevertToSnapshot, METH_VARARGS,
     "VixVM_RevertToSnapshot(vmHandle, snapshotHandle, options)"
    },
    {"VixVM_CreateSnapshot", 
     (PyCFunction)pv_VixVM_CreateSnapshot, METH_VARARGS,
     "VixVM_CreateSnapshot(vmHandle, name, desc, options)"
    },
    {"VixSnapshot_GetNumChildren", 
     (PyCFunction)pv_VixSnapshot_GetNumChildren, METH_VARARGS,
     "VixSnapshot_GetNumChildren(snapHandle)"
    },
    {"VixSnapshot_GetChild", 
     (PyCFunction)pv_VixSnapshot_GetChild, METH_VARARGS,
     "VixSnapshot_GetChild(snapHandle, index)"
    },
    {"VixSnapshot_GetParent", 
     (PyCFunction)pv_VixSnapshot_GetParent, METH_VARARGS,
     "VixSnapshot_GetParent(snapHandle)"
    },

    {"VixVM_CaptureScreenImage", 
     (PyCFunction)pv_VixVM_CaptureScreenImage, METH_VARARGS,
     "VixVM_CaptureScreenImage(vmHandle)"
    },

    {"VixVM_UpgradeVirtualHardware", 
     (PyCFunction)pv_VixVM_UpgradeVirtualHardware, METH_VARARGS,
     "VixVM_UpgradeVirtualHardware(vmHandle)"
    },
    {"VixVM_InstallTools", 
     (PyCFunction)pv_VixVM_InstallTools, METH_VARARGS,
     "VixVM_InstallTools(vmHandle, options)"
    },
    /*
    {"VixVM_Clone", 
     (PyCFunction)pv_VixVM_Clone, METH_VARARGS,
     "VixVM_Clone(vmHandle)"
    },
    */


    {NULL, NULL, 0, NULL}
};

//METH_NOARGS,

void AddConstants(PyObject *m)
{
    struct IntConstantList clist[] = {
        {"VIX_INVALID_HANDLE", VIX_INVALID_HANDLE},
        {"VIX_HANDLETYPE_NONE", VIX_HANDLETYPE_NONE},
        {"VIX_HANDLETYPE_HOST", VIX_HANDLETYPE_HOST},
        {"VIX_HANDLETYPE_VM", VIX_HANDLETYPE_VM},
        {"VIX_HANDLETYPE_NETWORK", VIX_HANDLETYPE_NETWORK},
        {"VIX_HANDLETYPE_JOB", VIX_HANDLETYPE_JOB},
        {"VIX_HANDLETYPE_SNAPSHOT", VIX_HANDLETYPE_SNAPSHOT},
        {"VIX_HANDLETYPE_PROPERTY_LIST", VIX_HANDLETYPE_PROPERTY_LIST},
        {"VIX_HANDLETYPE_METADATA_CONTAINER", 
            VIX_HANDLETYPE_METADATA_CONTAINER},
        {"VIX_OK", VIX_OK},
        {"VIX_E_FAIL", VIX_E_FAIL},
        {"VIX_E_OUT_OF_MEMORY", VIX_E_OUT_OF_MEMORY},
        {"VIX_E_INVALID_ARG", VIX_E_INVALID_ARG},
        {"VIX_E_FILE_NOT_FOUND", VIX_E_FILE_NOT_FOUND},
        {"VIX_E_OBJECT_IS_BUSY", VIX_E_OBJECT_IS_BUSY},
        {"VIX_E_NOT_SUPPORTED", VIX_E_NOT_SUPPORTED},
        {"VIX_E_FILE_ERROR", VIX_E_FILE_ERROR},
        {"VIX_E_DISK_FULL", VIX_E_DISK_FULL},
        {"VIX_E_INCORRECT_FILE_TYPE", VIX_E_INCORRECT_FILE_TYPE},
        {"VIX_E_CANCELLED", VIX_E_CANCELLED},
        {"VIX_E_FILE_READ_ONLY", VIX_E_FILE_READ_ONLY},
        {"VIX_E_FILE_ALREADY_EXISTS", VIX_E_FILE_ALREADY_EXISTS},
        {"VIX_E_FILE_ACCESS_ERROR", VIX_E_FILE_ACCESS_ERROR},
        {"VIX_E_REQUIRES_LARGE_FILES", VIX_E_REQUIRES_LARGE_FILES},
        {"VIX_E_FILE_ALREADY_LOCKED", VIX_E_FILE_ALREADY_LOCKED},
        {"VIX_E_VMDB", VIX_E_VMDB},
        {"VIX_E_NOT_SUPPORTED_ON_REMOTE_OBJECT", 
            VIX_E_NOT_SUPPORTED_ON_REMOTE_OBJECT},
        {"VIX_E_FILE_TOO_BIG", VIX_E_FILE_TOO_BIG},
        {"VIX_E_FILE_NAME_INVALID", VIX_E_FILE_NAME_INVALID},
        {"VIX_E_ALREADY_EXISTS", VIX_E_ALREADY_EXISTS},
        {"VIX_E_BUFFER_TOOSMALL", VIX_E_BUFFER_TOOSMALL},
        {"VIX_E_OBJECT_NOT_FOUND", VIX_E_OBJECT_NOT_FOUND},
        {"VIX_E_HOST_NOT_CONNECTED", VIX_E_HOST_NOT_CONNECTED},
        {"VIX_E_INVALID_UTF8_STRING", VIX_E_INVALID_UTF8_STRING},
        {"VIX_E_OPERATION_ALREADY_IN_PROGRESS", 
            VIX_E_OPERATION_ALREADY_IN_PROGRESS},
        {"VIX_E_UNFINISHED_JOB", VIX_E_UNFINISHED_JOB},
        {"VIX_E_NEED_KEY", VIX_E_NEED_KEY},
        {"VIX_E_LICENSE", VIX_E_LICENSE},
        {"VIX_E_VM_HOST_DISCONNECTED", VIX_E_VM_HOST_DISCONNECTED},
        {"VIX_E_AUTHENTICATION_FAIL", VIX_E_AUTHENTICATION_FAIL},
        {"VIX_E_INVALID_HANDLE", VIX_E_INVALID_HANDLE},
        {"VIX_E_NOT_SUPPORTED_ON_HANDLE_TYPE", 
            VIX_E_NOT_SUPPORTED_ON_HANDLE_TYPE},
        {"VIX_E_TOO_MANY_HANDLES", VIX_E_TOO_MANY_HANDLES},
        {"VIX_E_NOT_FOUND", VIX_E_NOT_FOUND},
        {"VIX_E_TYPE_MISMATCH", VIX_E_TYPE_MISMATCH},
        {"VIX_E_INVALID_XML", VIX_E_INVALID_XML},
        {"VIX_E_TIMEOUT_WAITING_FOR_TOOLS", VIX_E_TIMEOUT_WAITING_FOR_TOOLS},
        {"VIX_E_UNRECOGNIZED_COMMAND", VIX_E_UNRECOGNIZED_COMMAND},
        {"VIX_E_OP_NOT_SUPPORTED_ON_GUEST", VIX_E_OP_NOT_SUPPORTED_ON_GUEST},
        {"VIX_E_PROGRAM_NOT_STARTED", VIX_E_PROGRAM_NOT_STARTED},
        {"VIX_E_CANNOT_START_READ_ONLY_VM", VIX_E_CANNOT_START_READ_ONLY_VM},
        {"VIX_E_VM_NOT_RUNNING", VIX_E_VM_NOT_RUNNING},
        {"VIX_E_VM_IS_RUNNING", VIX_E_VM_IS_RUNNING},
        {"VIX_E_CANNOT_CONNECT_TO_VM", VIX_E_CANNOT_CONNECT_TO_VM},
        {"VIX_E_POWEROP_SCRIPTS_NOT_AVAILABLE", 
            VIX_E_POWEROP_SCRIPTS_NOT_AVAILABLE},
        {"VIX_E_NO_GUEST_OS_INSTALLED", VIX_E_NO_GUEST_OS_INSTALLED},
        {"VIX_E_VM_INSUFFICIENT_HOST_MEMORY", 
            VIX_E_VM_INSUFFICIENT_HOST_MEMORY},
        {"VIX_E_SUSPEND_ERROR", VIX_E_SUSPEND_ERROR},
        {"VIX_E_VM_NOT_ENOUGH_CPUS", VIX_E_VM_NOT_ENOUGH_CPUS},
        {"VIX_E_HOST_USER_PERMISSIONS", VIX_E_HOST_USER_PERMISSIONS},
        {"VIX_E_GUEST_USER_PERMISSIONS", VIX_E_GUEST_USER_PERMISSIONS},
        {"VIX_E_TOOLS_NOT_RUNNING", VIX_E_TOOLS_NOT_RUNNING},
        {"VIX_E_GUEST_OPERATIONS_PROHIBITED", 
            VIX_E_GUEST_OPERATIONS_PROHIBITED},
        {"VIX_E_ANON_GUEST_OPERATIONS_PROHIBITED", 
            VIX_E_ANON_GUEST_OPERATIONS_PROHIBITED},
        {"VIX_E_ROOT_GUEST_OPERATIONS_PROHIBITED", 
            VIX_E_ROOT_GUEST_OPERATIONS_PROHIBITED},
        {"VIX_E_MISSING_ANON_GUEST_ACCOUNT", VIX_E_MISSING_ANON_GUEST_ACCOUNT},
        {"VIX_E_CANNOT_AUTHENTICATE_WITH_GUEST", 
            VIX_E_CANNOT_AUTHENTICATE_WITH_GUEST},
        {"VIX_E_UNRECOGNIZED_COMMAND_IN_GUEST", 
            VIX_E_UNRECOGNIZED_COMMAND_IN_GUEST},
        {"VIX_E_CONSOLE_GUEST_OPERATIONS_PROHIBITED", 
            VIX_E_CONSOLE_GUEST_OPERATIONS_PROHIBITED},
        {"VIX_E_MUST_BE_CONSOLE_USER", VIX_E_MUST_BE_CONSOLE_USER},
        {"VIX_E_VMX_MSG_DIALOG_AND_NO_UI", VIX_E_VMX_MSG_DIALOG_AND_NO_UI},
        {"VIX_E_NOT_ALLOWED_DURING_VM_RECORDING", 
            VIX_E_NOT_ALLOWED_DURING_VM_RECORDING},
        {"VIX_E_NOT_ALLOWED_DURING_VM_REPLAY", 
            VIX_E_NOT_ALLOWED_DURING_VM_REPLAY},
        {"VIX_E_OPERATION_NOT_ALLOWED_FOR_LOGIN_TYPE", 
            VIX_E_OPERATION_NOT_ALLOWED_FOR_LOGIN_TYPE},
        {"VIX_E_LOGIN_TYPE_NOT_SUPPORTED", VIX_E_LOGIN_TYPE_NOT_SUPPORTED},
        {"VIX_E_EMPTY_PASSWORD_NOT_ALLOWED_IN_GUEST", 
            VIX_E_EMPTY_PASSWORD_NOT_ALLOWED_IN_GUEST},
        {"VIX_E_INTERACTIVE_SESSION_NOT_PRESENT", 
            VIX_E_INTERACTIVE_SESSION_NOT_PRESENT},
        {"VIX_E_INTERACTIVE_SESSION_USER_MISMATCH", 
            VIX_E_INTERACTIVE_SESSION_USER_MISMATCH},
        {"VIX_E_UNABLE_TO_REPLAY_VM", VIX_E_UNABLE_TO_REPLAY_VM},
        {"VIX_E_CANNOT_POWER_ON_VM", VIX_E_CANNOT_POWER_ON_VM},
        {"VIX_E_NO_DISPLAY_SERVER", VIX_E_NO_DISPLAY_SERVER},
        {"VIX_E_VM_NOT_RECORDING", VIX_E_VM_NOT_RECORDING},
        {"VIX_E_VM_NOT_REPLAYING", VIX_E_VM_NOT_REPLAYING},
        {"VIX_E_VM_NOT_FOUND", VIX_E_VM_NOT_FOUND},
        {"VIX_E_NOT_SUPPORTED_FOR_VM_VERSION", 
            VIX_E_NOT_SUPPORTED_FOR_VM_VERSION},
        {"VIX_E_CANNOT_READ_VM_CONFIG", VIX_E_CANNOT_READ_VM_CONFIG},
        {"VIX_E_TEMPLATE_VM", VIX_E_TEMPLATE_VM},
        {"VIX_E_VM_ALREADY_LOADED", VIX_E_VM_ALREADY_LOADED},
        {"VIX_E_VM_ALREADY_UP_TO_DATE", VIX_E_VM_ALREADY_UP_TO_DATE},
        {"VIX_E_UNRECOGNIZED_PROPERTY", VIX_E_UNRECOGNIZED_PROPERTY},
        {"VIX_E_INVALID_PROPERTY_VALUE", VIX_E_INVALID_PROPERTY_VALUE},
        {"VIX_E_READ_ONLY_PROPERTY", VIX_E_READ_ONLY_PROPERTY},
        {"VIX_E_MISSING_REQUIRED_PROPERTY", VIX_E_MISSING_REQUIRED_PROPERTY},
        {"VIX_E_INVALID_SERIALIZED_DATA", VIX_E_INVALID_SERIALIZED_DATA},
        {"VIX_E_PROPERTY_TYPE_MISMATCH", VIX_E_PROPERTY_TYPE_MISMATCH},
        {"VIX_E_BAD_VM_INDEX", VIX_E_BAD_VM_INDEX},
        {"VIX_E_INVALID_MESSAGE_HEADER", VIX_E_INVALID_MESSAGE_HEADER},
        {"VIX_E_INVALID_MESSAGE_BODY", VIX_E_INVALID_MESSAGE_BODY},
        {"VIX_E_SNAPSHOT_INVAL", VIX_E_SNAPSHOT_INVAL},
        {"VIX_E_SNAPSHOT_DUMPER", VIX_E_SNAPSHOT_DUMPER},
        {"VIX_E_SNAPSHOT_DISKLIB", VIX_E_SNAPSHOT_DISKLIB},
        {"VIX_E_SNAPSHOT_NOTFOUND", VIX_E_SNAPSHOT_NOTFOUND},
        {"VIX_E_SNAPSHOT_EXISTS", VIX_E_SNAPSHOT_EXISTS},
        {"VIX_E_SNAPSHOT_VERSION", VIX_E_SNAPSHOT_VERSION},
        {"VIX_E_SNAPSHOT_NOPERM", VIX_E_SNAPSHOT_NOPERM},
        {"VIX_E_SNAPSHOT_CONFIG", VIX_E_SNAPSHOT_CONFIG},
        {"VIX_E_SNAPSHOT_NOCHANGE", VIX_E_SNAPSHOT_NOCHANGE},
        {"VIX_E_SNAPSHOT_CHECKPOINT", VIX_E_SNAPSHOT_CHECKPOINT},
        {"VIX_E_SNAPSHOT_LOCKED", VIX_E_SNAPSHOT_LOCKED},
        {"VIX_E_SNAPSHOT_INCONSISTENT", VIX_E_SNAPSHOT_INCONSISTENT},
        {"VIX_E_SNAPSHOT_NAMETOOLONG", VIX_E_SNAPSHOT_NAMETOOLONG},
        {"VIX_E_SNAPSHOT_VIXFILE", VIX_E_SNAPSHOT_VIXFILE},
        {"VIX_E_SNAPSHOT_DISKLOCKED", VIX_E_SNAPSHOT_DISKLOCKED},
        {"VIX_E_SNAPSHOT_DUPLICATEDDISK", VIX_E_SNAPSHOT_DUPLICATEDDISK},
        {"VIX_E_SNAPSHOT_INDEPENDENTDISK", VIX_E_SNAPSHOT_INDEPENDENTDISK},
        {"VIX_E_SNAPSHOT_NONUNIQUE_NAME", VIX_E_SNAPSHOT_NONUNIQUE_NAME},
        {"VIX_E_SNAPSHOT_MEMORY_ON_INDEPENDENT_DISK", 
            VIX_E_SNAPSHOT_MEMORY_ON_INDEPENDENT_DISK},
        {"VIX_E_SNAPSHOT_MAXSNAPSHOTS", VIX_E_SNAPSHOT_MAXSNAPSHOTS},
        {"VIX_E_SNAPSHOT_MIN_FREE_SPACE", VIX_E_SNAPSHOT_MIN_FREE_SPACE},
        {"VIX_E_SNAPSHOT_RRSUSPEND", VIX_E_SNAPSHOT_RRSUSPEND},
        {"VIX_E_HOST_DISK_INVALID_VALUE", VIX_E_HOST_DISK_INVALID_VALUE},
        {"VIX_E_HOST_DISK_SECTORSIZE", VIX_E_HOST_DISK_SECTORSIZE},
        {"VIX_E_HOST_FILE_ERROR_EOF", VIX_E_HOST_FILE_ERROR_EOF},
        {"VIX_E_HOST_NETBLKDEV_HANDSHAKE", VIX_E_HOST_NETBLKDEV_HANDSHAKE},
        {"VIX_E_HOST_SOCKET_CREATION_ERROR", VIX_E_HOST_SOCKET_CREATION_ERROR},
        {"VIX_E_HOST_SERVER_NOT_FOUND", VIX_E_HOST_SERVER_NOT_FOUND},
        {"VIX_E_HOST_NETWORK_CONN_REFUSED", VIX_E_HOST_NETWORK_CONN_REFUSED},
        {"VIX_E_HOST_TCP_SOCKET_ERROR", VIX_E_HOST_TCP_SOCKET_ERROR},
        {"VIX_E_HOST_TCP_CONN_LOST", VIX_E_HOST_TCP_CONN_LOST},
        {"VIX_E_HOST_NBD_HASHFILE_VOLUME", VIX_E_HOST_NBD_HASHFILE_VOLUME},
        {"VIX_E_HOST_NBD_HASHFILE_INIT", VIX_E_HOST_NBD_HASHFILE_INIT},
        {"VIX_E_DISK_INVAL", VIX_E_DISK_INVAL},
        {"VIX_E_DISK_NOINIT", VIX_E_DISK_NOINIT},
        {"VIX_E_DISK_NOIO", VIX_E_DISK_NOIO},
        {"VIX_E_DISK_PARTIALCHAIN", VIX_E_DISK_PARTIALCHAIN},
        {"VIX_E_DISK_NEEDSREPAIR", VIX_E_DISK_NEEDSREPAIR},
        {"VIX_E_DISK_OUTOFRANGE", VIX_E_DISK_OUTOFRANGE},
        {"VIX_E_DISK_CID_MISMATCH", VIX_E_DISK_CID_MISMATCH},
        {"VIX_E_DISK_CANTSHRINK", VIX_E_DISK_CANTSHRINK},
        {"VIX_E_DISK_PARTMISMATCH", VIX_E_DISK_PARTMISMATCH},
        {"VIX_E_DISK_UNSUPPORTEDDISKVERSION", VIX_E_DISK_UNSUPPORTEDDISKVERSION},
        {"VIX_E_DISK_OPENPARENT", VIX_E_DISK_OPENPARENT},
        {"VIX_E_DISK_NOTSUPPORTED", VIX_E_DISK_NOTSUPPORTED},
        {"VIX_E_DISK_NEEDKEY", VIX_E_DISK_NEEDKEY},
        {"VIX_E_DISK_NOKEYOVERRIDE", VIX_E_DISK_NOKEYOVERRIDE},
        {"VIX_E_DISK_NOTENCRYPTED", VIX_E_DISK_NOTENCRYPTED},
        {"VIX_E_DISK_NOKEY", VIX_E_DISK_NOKEY},
        {"VIX_E_DISK_INVALIDPARTITIONTABLE", VIX_E_DISK_INVALIDPARTITIONTABLE},
        {"VIX_E_DISK_NOTNORMAL", VIX_E_DISK_NOTNORMAL},
        {"VIX_E_DISK_NOTENCDESC", VIX_E_DISK_NOTENCDESC},
        {"VIX_E_DISK_NEEDVMFS", VIX_E_DISK_NEEDVMFS},
        {"VIX_E_DISK_RAWTOOBIG", VIX_E_DISK_RAWTOOBIG},
        {"VIX_E_DISK_TOOMANYOPENFILES", VIX_E_DISK_TOOMANYOPENFILES},
        {"VIX_E_DISK_TOOMANYREDO", VIX_E_DISK_TOOMANYREDO},
        {"VIX_E_DISK_RAWTOOSMALL", VIX_E_DISK_RAWTOOSMALL},
        {"VIX_E_DISK_INVALIDCHAIN", VIX_E_DISK_INVALIDCHAIN},
        {"VIX_E_DISK_KEY_NOTFOUND", VIX_E_DISK_KEY_NOTFOUND},
        {"VIX_E_DISK_SUBSYSTEM_INIT_FAIL", VIX_E_DISK_SUBSYSTEM_INIT_FAIL},
        {"VIX_E_DISK_INVALID_CONNECTION", VIX_E_DISK_INVALID_CONNECTION},
        {"VIX_E_DISK_ENCODING", VIX_E_DISK_ENCODING},
        {"VIX_E_DISK_CANTREPAIR", VIX_E_DISK_CANTREPAIR},
        {"VIX_E_DISK_INVALIDDISK", VIX_E_DISK_INVALIDDISK},
        {"VIX_E_DISK_NOLICENSE", VIX_E_DISK_NOLICENSE},
        {"VIX_E_DISK_NODEVICE", VIX_E_DISK_NODEVICE},
        {"VIX_E_DISK_UNSUPPORTEDDEVICE", VIX_E_DISK_UNSUPPORTEDDEVICE},
        {"VIX_E_CRYPTO_UNKNOWN_ALGORITHM", VIX_E_CRYPTO_UNKNOWN_ALGORITHM},
        {"VIX_E_CRYPTO_BAD_BUFFER_SIZE", VIX_E_CRYPTO_BAD_BUFFER_SIZE},
        {"VIX_E_CRYPTO_INVALID_OPERATION", VIX_E_CRYPTO_INVALID_OPERATION},
        {"VIX_E_CRYPTO_RANDOM_DEVICE", VIX_E_CRYPTO_RANDOM_DEVICE},
        {"VIX_E_CRYPTO_NEED_PASSWORD", VIX_E_CRYPTO_NEED_PASSWORD},
        {"VIX_E_CRYPTO_BAD_PASSWORD", VIX_E_CRYPTO_BAD_PASSWORD},
        {"VIX_E_CRYPTO_NOT_IN_DICTIONARY", VIX_E_CRYPTO_NOT_IN_DICTIONARY},
        {"VIX_E_CRYPTO_NO_CRYPTO", VIX_E_CRYPTO_NO_CRYPTO},
        {"VIX_E_CRYPTO_ERROR", VIX_E_CRYPTO_ERROR},
        {"VIX_E_CRYPTO_BAD_FORMAT", VIX_E_CRYPTO_BAD_FORMAT},
        {"VIX_E_CRYPTO_LOCKED", VIX_E_CRYPTO_LOCKED},
        {"VIX_E_CRYPTO_EMPTY", VIX_E_CRYPTO_EMPTY},
        {"VIX_E_CRYPTO_KEYSAFE_LOCATOR", VIX_E_CRYPTO_KEYSAFE_LOCATOR},
        {"VIX_E_CANNOT_CONNECT_TO_HOST", VIX_E_CANNOT_CONNECT_TO_HOST},
        {"VIX_E_NOT_FOR_REMOTE_HOST", VIX_E_NOT_FOR_REMOTE_HOST},
        {"VIX_E_INVALID_HOSTNAME_SPECIFICATION", 
            VIX_E_INVALID_HOSTNAME_SPECIFICATION},
        {"VIX_E_SCREEN_CAPTURE_ERROR", VIX_E_SCREEN_CAPTURE_ERROR},
        {"VIX_E_SCREEN_CAPTURE_BAD_FORMAT", VIX_E_SCREEN_CAPTURE_BAD_FORMAT},
        {"VIX_E_SCREEN_CAPTURE_COMPRESSION_FAIL", 
            VIX_E_SCREEN_CAPTURE_COMPRESSION_FAIL},
        {"VIX_E_SCREEN_CAPTURE_LARGE_DATA", VIX_E_SCREEN_CAPTURE_LARGE_DATA},
        {"VIX_E_GUEST_VOLUMES_NOT_FROZEN", VIX_E_GUEST_VOLUMES_NOT_FROZEN},
        {"VIX_E_NOT_A_FILE", VIX_E_NOT_A_FILE},
        {"VIX_E_NOT_A_DIRECTORY", VIX_E_NOT_A_DIRECTORY},
        {"VIX_E_NO_SUCH_PROCESS", VIX_E_NO_SUCH_PROCESS},
        {"VIX_E_FILE_NAME_TOO_LONG", VIX_E_FILE_NAME_TOO_LONG},
        {"VIX_E_TOOLS_INSTALL_NO_IMAGE", VIX_E_TOOLS_INSTALL_NO_IMAGE},
        {"VIX_E_TOOLS_INSTALL_IMAGE_INACCESIBLE", 
            VIX_E_TOOLS_INSTALL_IMAGE_INACCESIBLE},
        {"VIX_E_TOOLS_INSTALL_NO_DEVICE", VIX_E_TOOLS_INSTALL_NO_DEVICE},
        {"VIX_E_TOOLS_INSTALL_DEVICE_NOT_CONNECTED", 
            VIX_E_TOOLS_INSTALL_DEVICE_NOT_CONNECTED},
        {"VIX_E_TOOLS_INSTALL_CANCELLED", VIX_E_TOOLS_INSTALL_CANCELLED},
        {"VIX_E_TOOLS_INSTALL_INIT_FAILED", VIX_E_TOOLS_INSTALL_INIT_FAILED},
        {"VIX_E_TOOLS_INSTALL_AUTO_NOT_SUPPORTED", 
            VIX_E_TOOLS_INSTALL_AUTO_NOT_SUPPORTED},
        {"VIX_E_TOOLS_INSTALL_GUEST_NOT_READY", 
            VIX_E_TOOLS_INSTALL_GUEST_NOT_READY},
        {"VIX_E_TOOLS_INSTALL_SIG_CHECK_FAILED", 
            VIX_E_TOOLS_INSTALL_SIG_CHECK_FAILED},
        {"VIX_E_TOOLS_INSTALL_ERROR", VIX_E_TOOLS_INSTALL_ERROR},
        {"VIX_E_TOOLS_INSTALL_ALREADY_UP_TO_DATE", 
            VIX_E_TOOLS_INSTALL_ALREADY_UP_TO_DATE},
        {"VIX_E_TOOLS_INSTALL_IN_PROGRESS", VIX_E_TOOLS_INSTALL_IN_PROGRESS},
        {"VIX_E_WRAPPER_WORKSTATION_NOT_INSTALLED", 
            VIX_E_WRAPPER_WORKSTATION_NOT_INSTALLED},
        {"VIX_E_WRAPPER_VERSION_NOT_FOUND", VIX_E_WRAPPER_VERSION_NOT_FOUND},
        {"VIX_E_WRAPPER_SERVICEPROVIDER_NOT_FOUND", 
            VIX_E_WRAPPER_SERVICEPROVIDER_NOT_FOUND},
        {"VIX_E_WRAPPER_PLAYER_NOT_INSTALLED", 
            VIX_E_WRAPPER_PLAYER_NOT_INSTALLED},
        {"VIX_PROPERTYTYPE_ANY", VIX_PROPERTYTYPE_ANY},
        {"VIX_PROPERTYTYPE_INTEGER", VIX_PROPERTYTYPE_INTEGER},
        {"VIX_PROPERTYTYPE_STRING", VIX_PROPERTYTYPE_STRING},
        {"VIX_PROPERTYTYPE_BOOL", VIX_PROPERTYTYPE_BOOL},
        {"VIX_PROPERTYTYPE_HANDLE", VIX_PROPERTYTYPE_HANDLE},
        {"VIX_PROPERTYTYPE_INT64", VIX_PROPERTYTYPE_INT64},
        {"VIX_PROPERTYTYPE_BLOB", VIX_PROPERTYTYPE_BLOB},
        {"VIX_PROPERTY_NONE", VIX_PROPERTY_NONE},
        {"VIX_PROPERTY_META_DATA_CONTAINER", VIX_PROPERTY_META_DATA_CONTAINER},
        {"VIX_PROPERTY_HOST_HOSTTYPE", VIX_PROPERTY_HOST_HOSTTYPE},
        {"VIX_PROPERTY_HOST_API_VERSION", VIX_PROPERTY_HOST_API_VERSION},
        {"VIX_PROPERTY_VM_NUM_VCPUS", VIX_PROPERTY_VM_NUM_VCPUS},
        {"VIX_PROPERTY_VM_VMX_PATHNAME", VIX_PROPERTY_VM_VMX_PATHNAME},
        {"VIX_PROPERTY_VM_VMTEAM_PATHNAME", VIX_PROPERTY_VM_VMTEAM_PATHNAME},
        {"VIX_PROPERTY_VM_MEMORY_SIZE", VIX_PROPERTY_VM_MEMORY_SIZE},
        {"VIX_PROPERTY_VM_READ_ONLY", VIX_PROPERTY_VM_READ_ONLY},
        {"VIX_PROPERTY_VM_IN_VMTEAM", VIX_PROPERTY_VM_IN_VMTEAM},
        {"VIX_PROPERTY_VM_POWER_STATE", VIX_PROPERTY_VM_POWER_STATE},
        {"VIX_PROPERTY_VM_TOOLS_STATE", VIX_PROPERTY_VM_TOOLS_STATE},
        {"VIX_PROPERTY_VM_IS_RUNNING", VIX_PROPERTY_VM_IS_RUNNING},
        {"VIX_PROPERTY_VM_SUPPORTED_FEATURES", 
            VIX_PROPERTY_VM_SUPPORTED_FEATURES},
        {"VIX_PROPERTY_VM_IS_RECORDING", VIX_PROPERTY_VM_IS_RECORDING},
        {"VIX_PROPERTY_VM_IS_REPLAYING", VIX_PROPERTY_VM_IS_REPLAYING},
        {"VIX_PROPERTY_JOB_RESULT_ERROR_CODE", 
            VIX_PROPERTY_JOB_RESULT_ERROR_CODE},
        {"VIX_PROPERTY_JOB_RESULT_VM_IN_GROUP", 
            VIX_PROPERTY_JOB_RESULT_VM_IN_GROUP},
        {"VIX_PROPERTY_JOB_RESULT_USER_MESSAGE", 
            VIX_PROPERTY_JOB_RESULT_USER_MESSAGE},
        {"VIX_PROPERTY_JOB_RESULT_EXIT_CODE", 
            VIX_PROPERTY_JOB_RESULT_EXIT_CODE},
        {"VIX_PROPERTY_JOB_RESULT_COMMAND_OUTPUT", 
            VIX_PROPERTY_JOB_RESULT_COMMAND_OUTPUT},
        {"VIX_PROPERTY_JOB_RESULT_HANDLE", VIX_PROPERTY_JOB_RESULT_HANDLE},
        {"VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS", 
            VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS},
        {"VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME", 
            VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME},
        {"VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE", 
            VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE},
        {"VIX_PROPERTY_JOB_RESULT_ITEM_NAME", 
            VIX_PROPERTY_JOB_RESULT_ITEM_NAME},
        {"VIX_PROPERTY_JOB_RESULT_FOUND_ITEM_DESCRIPTION", 
            VIX_PROPERTY_JOB_RESULT_FOUND_ITEM_DESCRIPTION},
        {"VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_COUNT", 
            VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_COUNT},
        {"VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_HOST", 
            VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_HOST},
        {"VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_FLAGS", 
            VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_FLAGS},
        {"VIX_PROPERTY_JOB_RESULT_PROCESS_ID", 
            VIX_PROPERTY_JOB_RESULT_PROCESS_ID},
        {"VIX_PROPERTY_JOB_RESULT_PROCESS_OWNER", 
            VIX_PROPERTY_JOB_RESULT_PROCESS_OWNER},
        {"VIX_PROPERTY_JOB_RESULT_PROCESS_COMMAND", 
            VIX_PROPERTY_JOB_RESULT_PROCESS_COMMAND},
        {"VIX_PROPERTY_JOB_RESULT_FILE_FLAGS", 
            VIX_PROPERTY_JOB_RESULT_FILE_FLAGS},
        {"VIX_PROPERTY_JOB_RESULT_PROCESS_START_TIME", 
            VIX_PROPERTY_JOB_RESULT_PROCESS_START_TIME},
        {"VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING", 
            VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING},
        {"VIX_PROPERTY_JOB_RESULT_PROCESS_BEING_DEBUGGED", 
            VIX_PROPERTY_JOB_RESULT_PROCESS_BEING_DEBUGGED},
        {"VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_SIZE", 
            VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_SIZE},
        {"VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_DATA", 
            VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_DATA},
        {"VIX_PROPERTY_JOB_RESULT_FILE_SIZE", 
            VIX_PROPERTY_JOB_RESULT_FILE_SIZE},
        {"VIX_PROPERTY_JOB_RESULT_FILE_MOD_TIME", 
            VIX_PROPERTY_JOB_RESULT_FILE_MOD_TIME},
        {"VIX_PROPERTY_FOUND_ITEM_LOCATION", 
            VIX_PROPERTY_FOUND_ITEM_LOCATION},
        {"VIX_PROPERTY_SNAPSHOT_DISPLAYNAME", 
            VIX_PROPERTY_SNAPSHOT_DISPLAYNAME},
        {"VIX_PROPERTY_SNAPSHOT_DESCRIPTION", 
            VIX_PROPERTY_SNAPSHOT_DESCRIPTION},
        {"VIX_PROPERTY_SNAPSHOT_POWERSTATE", 
            VIX_PROPERTY_SNAPSHOT_POWERSTATE},
        {"VIX_PROPERTY_SNAPSHOT_IS_REPLAYABLE", 
            VIX_PROPERTY_SNAPSHOT_IS_REPLAYABLE},
        {"VIX_PROPERTY_GUEST_SHAREDFOLDERS_SHARES_PATH", 
            VIX_PROPERTY_GUEST_SHAREDFOLDERS_SHARES_PATH},
        {"VIX_PROPERTY_VM_ENCRYPTION_PASSWORD", 
            VIX_PROPERTY_VM_ENCRYPTION_PASSWORD},
        {"VIX_EVENTTYPE_JOB_COMPLETED", VIX_EVENTTYPE_JOB_COMPLETED},
        {"VIX_EVENTTYPE_JOB_PROGRESS", VIX_EVENTTYPE_JOB_PROGRESS},
        {"VIX_EVENTTYPE_FIND_ITEM", VIX_EVENTTYPE_FIND_ITEM},
        {"VIX_EVENTTYPE_CALLBACK_SIGNALLED", VIX_EVENTTYPE_CALLBACK_SIGNALLED},
        {"VIX_FILE_ATTRIBUTES_DIRECTORY", VIX_FILE_ATTRIBUTES_DIRECTORY},
        {"VIX_FILE_ATTRIBUTES_SYMLINK", VIX_FILE_ATTRIBUTES_SYMLINK},
        {"VIX_HOSTOPTION_USE_EVENT_PUMP", VIX_HOSTOPTION_USE_EVENT_PUMP},
        {"VIX_SERVICEPROVIDER_DEFAULT", VIX_SERVICEPROVIDER_DEFAULT},
        {"VIX_SERVICEPROVIDER_VMWARE_SERVER", 
            VIX_SERVICEPROVIDER_VMWARE_SERVER},
        {"VIX_SERVICEPROVIDER_VMWARE_WORKSTATION", 
            VIX_SERVICEPROVIDER_VMWARE_WORKSTATION},
        {"VIX_SERVICEPROVIDER_VMWARE_PLAYER", 
            VIX_SERVICEPROVIDER_VMWARE_PLAYER},
        {"VIX_SERVICEPROVIDER_VMWARE_VI_SERVER", 
            VIX_SERVICEPROVIDER_VMWARE_VI_SERVER},
        {"VIX_API_VERSION", VIX_API_VERSION},
        {"VIX_FIND_RUNNING_VMS", VIX_FIND_RUNNING_VMS},
        {"VIX_FIND_REGISTERED_VMS", VIX_FIND_REGISTERED_VMS},
        {"VIX_VMOPEN_NORMAL", VIX_VMOPEN_NORMAL},
        {"VIX_PUMPEVENTOPTION_NONE", VIX_PUMPEVENTOPTION_NONE},
        {"VIX_VMPOWEROP_NORMAL", VIX_VMPOWEROP_NORMAL},
        {"VIX_VMPOWEROP_FROM_GUEST", VIX_VMPOWEROP_FROM_GUEST},
        {"VIX_VMPOWEROP_SUPPRESS_SNAPSHOT_POWERON", 
            VIX_VMPOWEROP_SUPPRESS_SNAPSHOT_POWERON},
        {"VIX_VMPOWEROP_LAUNCH_GUI", VIX_VMPOWEROP_LAUNCH_GUI},
        {"VIX_VMPOWEROP_START_VM_PAUSED", VIX_VMPOWEROP_START_VM_PAUSED},
        {"VIX_VMDELETE_DISK_FILES", VIX_VMDELETE_DISK_FILES},
        {"VIX_POWERSTATE_POWERING_OFF", VIX_POWERSTATE_POWERING_OFF},
        {"VIX_POWERSTATE_POWERED_OFF", VIX_POWERSTATE_POWERED_OFF},
        {"VIX_POWERSTATE_POWERING_ON", VIX_POWERSTATE_POWERING_ON},
        {"VIX_POWERSTATE_POWERED_ON", VIX_POWERSTATE_POWERED_ON},
        {"VIX_POWERSTATE_SUSPENDING", VIX_POWERSTATE_SUSPENDING},
        {"VIX_POWERSTATE_SUSPENDED", VIX_POWERSTATE_SUSPENDED},
        {"VIX_POWERSTATE_TOOLS_RUNNING", VIX_POWERSTATE_TOOLS_RUNNING},
        {"VIX_POWERSTATE_RESETTING", VIX_POWERSTATE_RESETTING},
        {"VIX_POWERSTATE_BLOCKED_ON_MSG", VIX_POWERSTATE_BLOCKED_ON_MSG},
        {"VIX_POWERSTATE_PAUSED", VIX_POWERSTATE_PAUSED},
        {"VIX_POWERSTATE_RESUMING", VIX_POWERSTATE_RESUMING},
        {"VIX_TOOLSSTATE_UNKNOWN", VIX_TOOLSSTATE_UNKNOWN},
        {"VIX_TOOLSSTATE_RUNNING", VIX_TOOLSSTATE_RUNNING},
        {"VIX_TOOLSSTATE_NOT_INSTALLED", VIX_TOOLSSTATE_NOT_INSTALLED},
        {"VIX_VM_SUPPORT_SHARED_FOLDERS", VIX_VM_SUPPORT_SHARED_FOLDERS},
        {"VIX_VM_SUPPORT_MULTIPLE_SNAPSHOTS", VIX_VM_SUPPORT_MULTIPLE_SNAPSHOTS},
        {"VIX_VM_SUPPORT_TOOLS_INSTALL", VIX_VM_SUPPORT_TOOLS_INSTALL},
        {"VIX_VM_SUPPORT_HARDWARE_UPGRADE", VIX_VM_SUPPORT_HARDWARE_UPGRADE},
        {"VIX_LOGIN_IN_GUEST_REQUIRE_INTERACTIVE_ENVIRONMENT", 
            VIX_LOGIN_IN_GUEST_REQUIRE_INTERACTIVE_ENVIRONMENT},
        {"VIX_RUNPROGRAM_RETURN_IMMEDIATELY", VIX_RUNPROGRAM_RETURN_IMMEDIATELY},
        {"VIX_RUNPROGRAM_ACTIVATE_WINDOW", VIX_RUNPROGRAM_ACTIVATE_WINDOW},
        {"VIX_VM_GUEST_VARIABLE", VIX_VM_GUEST_VARIABLE},
        {"VIX_VM_CONFIG_RUNTIME_ONLY", VIX_VM_CONFIG_RUNTIME_ONLY},
        {"VIX_GUEST_ENVIRONMENT_VARIABLE", VIX_GUEST_ENVIRONMENT_VARIABLE},
        {"VIX_SNAPSHOT_REMOVE_CHILDREN", VIX_SNAPSHOT_REMOVE_CHILDREN},
        {"VIX_SNAPSHOT_INCLUDE_MEMORY", VIX_SNAPSHOT_INCLUDE_MEMORY},
        {"VIX_SHAREDFOLDER_WRITE_ACCESS", VIX_SHAREDFOLDER_WRITE_ACCESS},
        {"VIX_CAPTURESCREENFORMAT_PNG", VIX_CAPTURESCREENFORMAT_PNG},
        {"VIX_CAPTURESCREENFORMAT_PNG_NOCOMPRESS", 
            VIX_CAPTURESCREENFORMAT_PNG_NOCOMPRESS},
        {"VIX_CLONETYPE_FULL", VIX_CLONETYPE_FULL},
        {"VIX_CLONETYPE_LINKED", VIX_CLONETYPE_LINKED},
        {"VIX_INSTALLTOOLS_MOUNT_TOOLS_INSTALLER", 
            VIX_INSTALLTOOLS_MOUNT_TOOLS_INSTALLER},
        {"VIX_INSTALLTOOLS_AUTO_UPGRADE", VIX_INSTALLTOOLS_AUTO_UPGRADE},
        {"VIX_INSTALLTOOLS_RETURN_IMMEDIATELY", 
            VIX_INSTALLTOOLS_RETURN_IMMEDIATELY},
        {NULL, 0},
    };

    int i;
    for (i = 0; clist[i].name != NULL; i++)
        PyModule_AddUnsignedLongConstant(m, clist[i].name, clist[i].val);
}

PyMODINIT_FUNC init_vixpy(void)
{
    PyObject *m = NULL;
    PyObject *dict = NULL;
    PyObject *VixExc = NULL;

    m = Py_InitModule("_vixpy", VixPyMethods);
    if (m == NULL)
        goto error;

    if ((dict = PyModule_GetDict(m)) == NULL)
        goto error;

    AddConstants(m);


    VixExc = PyErr_NewException("_vixpy.VixError", NULL, NULL);
    PyDict_SetItemString(dict, "VixError", VixExc);
    Py_XDECREF(VixExc);
    

error:
    if (PyErr_Occurred())
        PyErr_SetString(PyExc_ImportError, "_vixpy: init failed");
}

