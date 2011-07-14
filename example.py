"""
vixpy example

Virtual machines must have 
- vmware tools installed
- No auto login
- Administrator password set appropriately

"""
import logging
import os
import shutil
import sys
import time
from vixpy import *

VM_BASE = r'c:\vm'

vm_list = {
    'XP' : VM_BASE + r'\WinXP\Windows XP Professional.vmx',
    '2K3' : VM_BASE + r'\Win2k3\Windows Server 2003 Enterprise Edition.vmx'
    }


TESTDIR = r'c:\testfiles'


def mkdirs(path):
    dname = ''
    for d in path.split(os.path.sep):
        dname = os.path.join(dname, d)
        try:
            os.mkdir(dname)
        except:
            pass


class SoftwareTest(object):
    _USER = 'administrator'
    _PASS = '12345678'

    def __init__(self, osname):
        self.name = osname
        self.path = ''
        self.log = None
        self.host = None
        self.vm = None
        self.scount = 0

        self.init_hostenv()


    def init_hostenv(self):
        self.path = os.path.join('testdata', self.name)
        shutil.rmtree(self.path, True)
        time.sleep(1)
        mkdirs(self.path)
        time.sleep(1)

        self.log = logging.getLogger(self.name)
        self.log.addHandler(logging.StreamHandler(sys.stdout))
        self.log.addHandler(logging.FileHandler(os.path.join(self.path, 'testlog.txt')))
        self.log.setLevel(logging.INFO)


    def err(self, msg, indent=0):
        self.log.info('\t'*indent + '[-] ' + msg)

    def info(self, msg, indent=0):
        self.log.info('\t'*indent + '[*] ' + msg)

    def success(self, msg, indent=0):
        self.log.info('\t'*indent + '[+] ' + msg)

    def warn(self, msg, indent=0):
        self.log.info('\t'*indent + '[!] ' + msg)


    def start_vm(self):
        self.host = VixHost()

        self.info("Opening VM")
        self.vm = self.host.open(vm_list[self.name])
        self.vm.revert(self.vm.get_namedsnapshot(self.name), True)

        self.info("Checking power state")
        if self.vm.power_state not in ("Powering On", "Powered On"):
            self.info("Powering on", 1)
            self.vm.on(gui=True)
        else:
            self.info("Already running", 1)

        self.info("Checking for vmware tools")
        if self.vm.power_state not in ("Tools Running",):
            self.info("Waiting for OS to fully load", 1)
            self.vm.wait()
        else:
            self.info("Tools already running (OS loaded)", 1)

        self.info("Logging in as %s" % self._USER)
        self.vm.login(self._USER, self._PASS, True)


    def reset_vm(self):
        self.info("Restarting VM")

        self.info("Powering off...", 1)
        # Blah - can't use reset...
        self.vm.off(True)
        if self.vm.power_state != 'Powered Off':
            raise VixError('Unknown State')

        self.info("Powering on...", 1)
        self.vm.on(gui=True)

        self.info("Checking for vmware tools - OS running state")
        if self.vm.power_state not in ("Tools Running",):
            self.info("Waiting for OS to fully load", 1)
            self.vm.wait()
        else:
            self.info("Tools already running (OS loaded)", 1)

        self.info("Logging in as %s" % self._USER)
        self.vm.login(self._USER, self._PASS, True)


    def init_testenv(self, testfiles):
        def make_path(f):
            return os.path.join(TESTDIR, os.path.basename(f))

        self.info("Creating Test environment")
        try:
            self.info("Creating test dir - %s" % TESTDIR, 1)
            self.vm.mkdir(TESTDIR)
        except VixError:
            self.warn("Directory already exists", 1)
            pass

        for f in testfiles:
            self.info("Copying %s to VM" % f, 1)
            self.vm.copy_to(f, make_path(f))

        self.success("Test environment created")


    def run_install(self):
        self.info("Running test software install")
        self.vm.run(os.path.join(TESTDIR, 'install.exe'), True, True)


    def run_uninstall(self):
        self.info("Running test software uninstall")
        self.vm.run(os.path.join(TESTDIR, 'uninstall.exe'), True, True)


    def verify(self):
        self.info("Verifying")
        self.info("Running verify script...", 1)
        self.vm.run(os.path.join(TESTDIR, 'verify.bat'), '')
        self.vm.copy_from(
                os.path.join(TESTDIR, 'verify_out.txt'),
                os.path.join(self.path, 'verify_out.txt'))


    def screen_shot(self):
        f = os.path.join(self.path, 'screen-shot-%02d.png' % self.scount)
        self.scount += 1
        self.info("Taking screen shot : %s" % f)
        open(f, 'wb').write(self.vm.screen_capture())


    def run_test(self):
        self.info("Starting Test : %s" % self.name)

        try:
            self.start_vm()
            self.screen_shot()
            self.init_testenv(['install.exe', 'uninstall.exe', 'verify.bat'])
            self.screen_shot()
            self.run_install()
            self.screen_shot()
            self.verify()
            self.screen_shot()
            self.reset_vm()
            self.screen_shot()
            self.run_uninstall()
            self.screen_shot()
        except Exception as e:
            self.err('Failed with exception')
            self.err(str(e))
        self.info("Test Finished")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "usage: %s <OS Name>" % sys.argv[0]
        print "  XP 2K3"
        sys.exit(-1)

    t = SoftwareTest(sys.argv[1])
    t.run_test()



