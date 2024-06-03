import mmap
import struct
from typing import Tuple


class APBTest:
    CONTROL_0: int = 0x00
    CONTROL_1: int = 0x10
    STATUS: int = 0x20

    def __init__(self, path: str = "/dev/uio/apb_test") -> None:
        """
        Default path requires a udev rule:

            SUBSYSTEM=="uio", SYMLINK+="uio/%s{name}", GROUP="uio"
        """
        self.fd = open(path, "w+b", buffering=0)
        self.mm = mmap.mmap(self.fd.fileno(), 256)

    def __del__(self) -> None:
        self.mm.close()
        self.fd.close()

    def read_status(self) -> int:
        """
        Read STATUS
        """
        self.mm.seek(self.STATUS)
        return struct.unpack("I", self.mm.read(4))[0]

    def read_control_0(self) -> bytes:
        """
        Read CONTROL_0
        """
        self.mm.seek(self.CONTROL_0)
        return self.mm.read(4)

    def read_control_1(self) -> bytes:
        """
        Read CONTROL_1
        """
        self.mm.seek(self.CONTROL_1)
        return self.mm.read(4)

    def write_control_1(self, data: bytes) -> None:
        """
        Write CONTROL_1
        """
        self.mm.seek(self.CONTROL_1)
        self.mm.write(data)
