''' Reads the data from a GPS sensor on a Serial port and processes it.
'''

from time import sleep
from csv import reader
from datetime import datetime

from serial import Serial


class Reading:
    ''' A class that keeps track of a reading.
    '''

    def __init__(self, data):
        self.data = data

        # Setup variables.
        self.sats = 0
        self.lat = None
        self.lon = None
        self.alt = None
        self.speed = None
        self.course = None
        self.datetime = None

        # Parse the data from the sensor.
        self._parse_data()

    def _parse_data(self):
        ''' Function to parse csv data returned from the sensor.
        '''
        # Open the data in a list into a _csv.reader object.
        data = reader([self.data.decode('utf-8').replace('\n', '')])

        # Get the reading from the _csv.reader object.
        reading = next(data)

        # Populate the object with data from the reading.
        if reading:
            try:
                self.sats = reading[0]
                self.lat = reading[1]
                self.lon = reading[2]
                self.alt = reading[3]
                self.speed = reading[4]
                self.course = reading[5]

                try:
                    self.datetime = datetime.fromisoformat(reading[6] + '+00:00')
                except ValueError:
                    # The datetime conversion will fail when an empty string
                    # is returned from the sensor. It should be set back to
                    # None when this happens.
                    self.datetime = None
            except IndexError:
                # There is no data in the reading yet.
                pass


if __name__ == '__main__':
    port = '/dev/cu.usbmodem3101'
    baudrate = 115200
    timeout = 1

    device = Serial(port = port, baudrate = baudrate, timeout = timeout)

    while True:
        # Get data from the sensor
        data = device.readline()

        # Create a Reading object with the data.
        reading = Reading(data = data)

        print('number of satallites: {}'.format(reading.sats))
        print('lat: {}'.format(reading.lat))
        print('lon: {}'.format(reading.lon))
        print('speed: {}'.format(reading.speed))
        print(reading.datetime)

        # Do a bit of garbage collection.
        del(reading)

