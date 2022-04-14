''' Reads the data from a GPS sensor on a Serial port and processes it.
'''

import sys
import logging

from time import sleep
from csv import reader
from datetime import datetime

from serial import Serial

class Sensor:
    ''' A class that keeps track of a sensor over a Serial port.

    '''

    def __init__(self, port, baudrate, timeout):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout

        self.reading = None

        logging.debug('created Sensor() object.')

    def start(self):
        ''' Starts the connection and starts polling the sensor.
        '''
        with Serial(
            port = self.port,
            baudrate = self.baudrate,
            timeout = self.timeout) as device:

            logging.debug('starting to read sensor')
            while True:
                # Delete the current reading.
                if self.reading is not None:
                    del(self.reading)

                # Read data from the sensor and create a Reading object.
                data = device.readline()
                self.reading = Reading(data = data)


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
        logging.info(f'{self.sats}, {self.lat}, {self.lon}, {self.alt}, {self.speed}, {self.course}, {self.datetime}')

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
                    logging.debug('invalid datetime format')
                    self.datetime = None
            except IndexError:
                # There is no data in the reading yet.
                logging.debug('no data from sensor')


if __name__ == '__main__':
    try:
        logging.basicConfig(format='%(levelname)s: %(message)s', level = logging.DEBUG)

        # Create the sensor object
        sensor = Sensor(
            port = '/dev/cu.usbmodem3101',
            baudrate = 115200,
            timeout = 1)

        # Start reading data from the sensor.
        sensor.start()
    except KeyboardInterrupt:
        # Exit cleanly if a KeyboardInterrupt is raised.
        del(sensor)
        logging.debug('exiting...')
        sys.exit()

