import struct


class SdbFile():
    def __init__(self, path):
        file = open(path, "rb")

        header = self.get_int(file)
        timeOffset = self.get_int(file) * 100000
        print(timeOffset)

        for x in range(100):
            datumCode = self.get_int(file)
            timeStamp = self.get_int(file)
            value = self.get_int(file)

            time = (timeOffset + timeStamp) * 1000
            source = datumCode >> 24 & 0xFF
            datum = datumCode & 0xFFFFFF


            print(source, datum, time, value)




    def get_int(self, file):
        """
        Returns an integer from the specified open file handle
        """
        return struct.unpack("i", file.read(4))[0]




if __name__ == "__main__":
    file = SdbFile("/home/aridarno/Desktop/19121002.sdb")
