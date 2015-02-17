# Modules

import random
import Queue
import math



# Simulation constructs

class Event(object):
    def __init__(self, event_time):
        self.next_event = None
        self.prev_event = None
        self.event_time = event_time

    def set_next_event(self, next_event):
        self.next_event = next_event
        return self

    def set_prev_event(self, prev_event):
        self.prev_event = prev_event
        return self

    def get_next_event(self):
        return self.next_event

    def get_prev_event(self):
        return self.prev_event

    def get_event_time(self):
        return self.event_time



class ArrivalEvent(Event):
    def __init__(self, event_time):
        super(ArrivalEvent, self).__init__(event_time)



class DepartureEvent(Event):
    def __init__(self, event_time):
        super(DepartureEvent, self).__init__(event_time)



class Packet(object):
    def __init__(self, processing_interval):
        self.time = processing_interval

    def get_processing_interval(self):
        return self.time



class PacketBuffer(object):
    def __init__(self, buffer_max):
        self.queue = Queue.Queue(buffer_max)

    def push_packet(self, packet):
        if not self.full():
            self.queue.put(packet)
            return True
        else:
            return False

    def pop_packet(self):
        return self.queue.get()

    def empty(self):
        return self.queue.empty()

    def full(self):
        return self.queue.full()



class EventList(object):
    def __init__(self, buffer_max, arrival_interval_func, departure_interval_func):
        self.head =         None
        self.buffer =       PacketBuffer(buffer_max)
        self.processing =   False
        self.current_time = 0.0

        self.ARRIVAL_INTERVAL =     arrival_interval_func
        self.PROCESSING_INTERVAL =  departure_interval_func

        self.add_arrival(self.ARRIVAL_INTERVAL())

    def add_arrival(self, interval):
        return self.insert_event(ArrivalEvent(self.current_time + interval))

    def add_departure(self, interval):
        return self.insert_event(DepartureEvent(self.current_time + interval))

    def insert_event(self, event):
        # Set head if no head
        if self.head is None:
            self.head = event
            return event

        # Start at head of linked list, we are guaranteed one event in the list
        event_time = event.get_event_time()
        prev_link = None
        next_link = self.head

        # Traverse to find the next event w.r.t. our newly inserted event
        while ((next_link is not None) and (next_link.get_event_time() < event_time)):
            prev_link = next_link
            next_link = next_link.get_next_event()

        event.set_next_event(next_link)
        event.set_prev_event(prev_link)

        if next_link is not None: 
            next_link.set_prev_event(event)
        if prev_link is not None:
            prev_link.set_next_event(event)

        if prev_link is None:
            self.head = event

        return event

    def process_event(self):
        event = self.head

        if event is None:
            return

        self.head = event.get_next_event()

        # Free up space for GC
        if self.head is not None:
            self.head.set_prev_event(None)

        if isinstance(event, ArrivalEvent):
            self.process_arrival(event)
        elif isinstance(event, DepartureEvent):
            self.process_departure(event)

    def process_arrival(self, event):
        # Advance time to current event
        self.current_time = event.get_event_time()

        # Create new arrival in future
        self.add_arrival(self.ARRIVAL_INTERVAL())

        # Create packet from current arrival
        packet = Packet(self.PROCESSING_INTERVAL())

        if self.buffer.empty():
            if self.processing:
                self.buffer.push_packet(packet)

                print 'Packet arrived to empty buffer and queued at {0}'.format(event.get_event_time())
            else:
                self.add_departure(packet.get_processing_interval())
                self.processing = True

                print 'Packet arrived to empty buffer for immediate processing at {0}'.format(event.get_event_time())
        else:
            if self.buffer.push_packet(packet):
                print 'Packet arrived to non-empty buffer of length {1} at {0}'.format(event.get_event_time(), self.buffer.queue.qsize())
            else:
                print 'Packet arrived to full buffer and was dropped at {0}'.format(event.get_event_time())

    def process_departure(self, event):
        # Advance time to current event
        self.current_time = event.get_event_time()

        # Schedule next departure in the future
        if not self.buffer.empty():
            packet = self.buffer.pop_packet()
            self.add_departure(packet.get_processing_interval())
        else:
            self.processing = False

        print 'Packet processed and departed at {0}'.format(event.get_event_time())



    def __repr__(self):
        listrep = ''

        e = self.head

        while (e is not None):
            listrep += '{0} {1}, '.format(type(e), e.get_event_time())
            e = e.get_next_event()

        return listrep



class Simulation(EventList):
    def __init__(self, events_to_simulate, buffer_size, arrival_interval_func, departure_interval_func):
        super(Simulation, self).__init__(buffer_size, arrival_interval_func, departure_interval_func)
        self.events_to_simulate =       events_to_simulate

    def run(self):
        for i in xrange(self.events_to_simulate):
            print 'Event {0}:'.format(i)
            self.process_event()
            print ''



# Simulation

EVENTS_TO_SIMULATE =    10000
MAX_BUFFER =            500
PARAM_LAMBDA =          0.4
PARAM_MU =              0.7

def NEG_EXP(param):
    # random.random is uniform over [0.0, 1.0)
    # math.log is base-e with no base kwarg
    return (-1.0 / param) * math.log(1.0 - random.random())

def ARRIVAL_INTERVAL():
    return NEG_EXP(PARAM_LAMBDA)

def PROCESSING_INTERVAL():
    return NEG_EXP(PARAM_MU)

simulation = Simulation(EVENTS_TO_SIMULATE, MAX_BUFFER, ARRIVAL_INTERVAL, PROCESSING_INTERVAL)

simulation.run()