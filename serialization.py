from struct import *


class PIDConfig:
    hash = b'asdfasdf'

    def __init__(self):
        self.kp = None  # float32

    @staticmethod
    def buffer_valid(buf):
        return True

    @classmethod
    def deserialize(cls, buf):
        if not PIDConfig.buffer_valid(buf):
            return None
        obj = cls()
        obj._deserialize(buf)
        return obj

    def _deserialize(self, buf):
        self.kp, = unpack_from('>i', buf, 0)

    def serialize(self):
        buf = []
        buf.append(pack('>i', self.kp))
        return b''.join(buf)
