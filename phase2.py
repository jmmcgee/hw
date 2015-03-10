import sys
import Queue
import random
import math



# Constants

# Time to transmit a maximum 1500 (+44) byte MTU frame over a 11 Mbps network
DATA_FRAME_MAX_TRANSMISSION = 0.00112

# Time to transmit a 20 (+44) byte ack frame over a 11 Mbps network
ACK_FRAME_TRANSMISSION = (64.0 / 1544.0) * 0.00112

SIFS = 0.00005
DIFS = 0.0001

BACKOFF_T = 3



# Maths and distributions

def NEG_EXP(PARAM_LAMBDA):
    # random.random is uniform over [0.0, 1.0)
    # math.log is base-e with no base kwarg
    return (-1.0 / PARAM_LAMBDA) * math.log(1.0 - random.random())



# Simlulation constructs
class Event(object):
    def __init__(self, event_time, host_id):
        self.event_time = event_time
        self.host_id = host_id

    def __cmp__(self, other):
        return cmp(self.event_time, other.event_time)



class FrameArrival(Event):
    def __init__(self, event_time, host_id):
        super(FrameArrival, self).__init__(event_time, host_id)



class TransmissionAttempt(Event):
    def __init__(self, event_time, host_id):
        super(TransmissionAttempt, self).__init__(event_time, host_id)



class TransmissionCompletion(Event):
    def __init__(self, event_time, host_id):
        super(Transmission, self).__init__(event_time, host_id)



class Frame(object):
    def __init__(self, transmission_time, source_host_id, dest_host_id):
        self.transmission_time = transmission_time
        self.source_host_id = source_host_id
        self.dest_host_id = dest_host_id



class DataFrame(Frame):
    def __init__(self, PARAM_LAMBDA, source_host_id, dest_host_id):
        transmission_time = DATA_FRAME_MAX_TRANSMISSION * NEG_EXP(PARAM_LAMBDA)
        super(DataFrame, self).__init__(transmission_time, source_host_id, dest_host_id)



class AckFrame(Frame):
    def __init__(self, source_host_id, dest_host_id):
        transmission_time = ACK_FRAME_TRANSMISSION
        super(AckFrame, self).__init__(transmission_time, source_host_id, dest_host_id)



class Host(object):
    def __init__(self, host_id, network, PARAM_MU):
        self.host_id = host_id
        self.network = network
        PARAM_MU

        self.frame_queue = Queue.Queue(maxsize=0)

        self.backoff = 0.0
        self.unsuccessful_attempts = 0

    def create_arrival_event(self, current_time):
        return FrameArrival(current_time + NEG_EXP(PARAM_MU), self.host_id)

    def enqueue_frame(self, frame):
        self.frame_queue.put(frame)

    def dequeue_frame(self):
        return self.frame_queue.get()

    def start_backoff(self):
        self.unsuccessful_attempts += 1
        self.backoff = math.floor(random.random() * self.unsuccessful_attempts * BACKOFF_T)

    def reset_backoff(self):
        self.unsuccessful_attempts = 0
        self.backoff = 0

    def decrement_backoff(self):
        if self.backoff > 0:
            self.backoff -= 1

    def get_backoff(self):
        return self.backoff


class Network(object):
    def __init__(self, N, PARAM_LAMBDA, PARAM_MU):
        self.time = 0.0
        self.transmitting = False

        self.hosts = dict((i, Host(i, self, PARAM_MU)) for i in xrange(N))
        self.events = Queue.PriorityQueue(maxsize=0)

        for i, host in self.hosts.iteritems():
            self.events.put(host.create_arrival_event(self.time))

    def simulate(self, limit):
        event_n = 0

        while((not self.events.empty()) and (event_n < limit)):
            event_n += 1
            event = self.events.get()
            event_type = type(event)

            self.time = event.event_time

            if (event_type == FrameArrival):
                # Create next frame arrival event
                self.events.put(self.hosts[event.host_id].create_arrival_event(self.time))

                # Choose a random neighbor and create a data frame for transmission
                destination_host_ids = [i for i in xrange(N)]
                destination_host_ids.remove(event.host_id)
                destination_host_id = random.choice(destination_host_ids)
                self.hosts[event.host_id].enqueue_frame(DataFrame(PARAM_LAMBDA, event.host_id, destination_host_id))

                if not self.transmitting:
                    self.events.put(TransmissionAttempt(self.time + DIFS, event.host_id))
                else:
                    self.hosts[event.host_id].start_backoff()
                    

            if (event_type == TransmissionAttempt):
                if not self.transmitting:
                    frame = self.hosts[event.host_id].dequeue_frame()
                    self.hosts[event.host_id].reset_backoff()

                    transmission_time = frame.transmission_time
                    self.events.put(TransmissionCompletion(self.time + transmission_time))
                    self.transmitting = frame
                else:
                    self.hosts[event.host_id].start_backoff()

            if (event_type == TransmissionCompletion):
                frame = self.transmitting

                self.transmitting = False

                frame_type = type(frame)

                if (frame_type == DataFrame):
                    # Enqueue an ack frame and attempt to transmit it
                    # sending host is set to a waiting state
                    #   no frames can be sent until ack releases lock
                    pass

                if (frame_type == AckFrame):
                    # Acknowledge successful reciept of data frame
                    # receiving host (which originally sent data) is not waiting
                    pass



network = Network(10, 0.3, 0.5)
network.simulate(100000)
