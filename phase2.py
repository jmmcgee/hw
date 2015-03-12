import sys
import Queue
import random
import math



DEBUG = True

# Constants

# Time to transmit a maximum 1500 (+44) byte MTU frame over a 11 Mbps network
DATA_FRAME_MAX_TRANSMISSION = 0.00112

# Time to transmit a 20 (+44) byte ack frame over a 11 Mbps network
ACK_FRAME_TRANSMISSION = (64.0 / 1544.0) * 0.00112

SIFS = 0.00005
DIFS = 0.0001

BACKOFF_T = 0.001
TIMEOUT_T = 0.05



# Maths and distributions

def NEG_EXP(PARAM):
    # random.random is uniform over [0.0, 1.0)
    # math.log is base-e with no base kwarg
    return (-1.0 / PARAM) * math.log(1.0 - random.random())



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



class TransmissionStart(Event):
    def __init__(self, event_time, host_id):
        super(TransmissionStart, self).__init__(event_time, host_id)



class TransmissionCompletion(Event):
    def __init__(self, event_time, host_id, frame):
        super(TransmissionCompletion, self).__init__(event_time, host_id)
        self.frame = frame



class TransmissionTimeout(Event):
    def __init__(self, event_time, host_id, frame):
        super(TransmissionTimeout, self).__init__(event_time, host_id)
        self.frame = frame



class Frame(object):
    def __init__(self, transmission_time, source_host_id, dest_host_id):
        self.transmission_time = transmission_time
        self.source_host_id = source_host_id
        self.dest_host_id = dest_host_id
        self.corrupted = False



class DataFrame(Frame):
    def __init__(self, PARAM_MU, source_host_id, dest_host_id):
        transmission_time = DATA_FRAME_MAX_TRANSMISSION * NEG_EXP(PARAM_MU)
        super(DataFrame, self).__init__(transmission_time, source_host_id, dest_host_id)



class AckFrame(Frame):
    def __init__(self, source_host_id, dest_host_id, data_frame):
        transmission_time = ACK_FRAME_TRANSMISSION
        super(AckFrame, self).__init__(transmission_time, source_host_id, dest_host_id)
        self.data_frame = data_frame



class Host(object):
    def __init__(self, host_id, network, PARAM_MU):
        self.host_id = host_id
        self.network = network
        self.PARAM_MU = PARAM_MU

        self.frame_queue = Queue.Queue(maxsize=0)
        self.ack_queue = Queue.Queue(maxsize=0)
        self.resend_queue = Queue.Queue(maxsize=0)
        self.sent_frames = []

        self.backoff = 0.0
        self.is_backing_off = False
        self.unsuccessful_attempts = 0

    def create_arrival_event(self, current_time):
        return FrameArrival(current_time + NEG_EXP(self.PARAM_MU), self.host_id)

    def resend_frame(self, frame):
        self.resend_queue.put(frame)

    def enqueue_frame(self, frame):
        if (type(frame) == AckFrame):
            self.ack_queue.put(frame)  
        else:
            self.frame_queue.put(frame)

    def dequeue_frame(self):
        if (not self.ack_queue.empty()):
            return self.ack_queue.get()
        elif (not self.resend_queue.empty()):
            return self.resend_queue.get()
        elif (not self.frame_queue.empty()):
            frame = self.frame_queue.get()
            self.sent_frames.append(frame)
            return frame
        else:
            return None

    def has_frame_to_send(self):
        return (not self.resend_queue.empty()) or (not self.frame_queue.empty()) or (not self.ack_queue.empty())

    def acknowledge(self, frame):
        self.sent_frames.remove(frame)

    def start_backoff(self):
        self.is_backing_off = True
        self.unsuccessful_attempts += 1
        self.backoff = math.floor(random.random() * self.unsuccessful_attempts * BACKOFF_T)

    def stop_backoff(self):
        self.is_backing_off = False

    def reset_backoff(self):
        self.is_backing_off = False
        self.unsuccessful_attempts = 0
        self.backoff = 0

    def decrement_backoff(self, backoff_dec):
        if self.backoff > 0 and self.is_backing_off:
            self.backoff -= backoff_dec
        elif self.backoff == 0:
            # TODO send another transmission
            pass
        elif self.backoff < 0:
            if DEBUG:
                print 'ERROR: backoff for host {host_id} is {backoff}'.format(
                        host_id=self.host_id,backoff=self.backoff)

    def get_backoff(self):
        if self.is_backing_off:
            return self.backoff
        else:
            return None


class Network(object):
    def __init__(self, N, PARAM_LAMBDA, PARAM_MU):
        self.N = N
        self.PARAM_LAMBDA = PARAM_LAMBDA
        self.PARAM_MU = PARAM_MU

        self.time = 0.0
        self.transmitting = []

        self.hosts = dict((i, Host(i, self, self.PARAM_MU)) for i in xrange(N))
        self.events = Queue.PriorityQueue(maxsize=0)

        for i, host in self.hosts.iteritems():
            self.events.put(host.create_arrival_event(0.0))

    def simulate(self, limit):
        event_n = 0

        while((not self.events.empty()) and (event_n < limit)):
            event_n += 1
            event = self.events.get()
            event_type = type(event)

            diff_time = event.event_time - self.time
    
            # Reduce the backoff of all hosts if idle
            rewind_time_backoff = False

            if not self.transmitting:
                backing_off_hosts = [host for host in self.hosts.values() if host.is_backing_off]
                backoffs = [host.get_backoff() for host in backing_off_hosts]
                min_backoff = min(backoffs + [diff_time])
                for host in backing_off_hosts:
                    host.decrement_backoff(min_backoff)
                    if (host.get_backoff() == 0.0) and (host.is_backing_off):
                        host.stop_backoff()
                        rewind_time_backoff = True

                        self.events.put(TransmissionStart(self.time + min_backoff, host.host_id))

            if rewind_time_backoff:
                self.time += min_backoff
                self.events.put(event)
                continue

            self.time = event.event_time
            if DEBUG:
                print 'TIME is {time}s'.format(time=self.time)

            if (event_type == FrameArrival):
                # Create next frame arrival event
                self.events.put(self.hosts[event.host_id].create_arrival_event(self.time))

                # Choose a random neighbor and create a data frame for transmission
                destination_host_ids = [i for i in xrange(self.N)]
                destination_host_ids.remove(event.host_id)
                destination_host_id = random.choice(destination_host_ids)

                new_data_frame = DataFrame(self.PARAM_MU, event.host_id, destination_host_id)

                self.hosts[event.host_id].enqueue_frame(new_data_frame)

                self.events.put(TransmissionAttempt(self.time, event.host_id))

                if DEBUG:
                    print 'A new data frame has been generated at host {0}. Its content will take {1}s to transmit.'.format(event.host_id, new_data_frame.transmission_time)

            if (event_type == TransmissionAttempt):
                host = self.hosts[event.host_id]

                if host.has_frame_to_send():
                    if not self.transmitting:
                        self.events.put(TransmissionStart(self.time + DIFS, event.host_id))
                    elif not host.is_backing_off:
                        host.start_backoff()

                    if DEBUG:
                        print 'Host {0} has the intent of transmitting a frame.'.format(host.host_id)

            if (event_type == TransmissionStart):
                # We dont't care what's happening, start transmitting
                frame = self.hosts[event.host_id].dequeue_frame()
                if frame in self.transmitting:
                    raise Exception('should not occur')
                    pass

                transmission_time = frame.transmission_time

                self.events.put(TransmissionCompletion(self.time + transmission_time, event.host_id, frame))

                self.transmitting.append(frame)

                if (type(frame) == DataFrame):
                    self.events.put(TransmissionTimeout(self.time + TIMEOUT_T, event.host_id, frame))

                if (len(self.transmitting) > 1):
                    for frame in self.transmitting:
                        frame.corrupted = True
                    if DEBUG:
                        print 'Corruption has occured.'

                if DEBUG:
                    print 'Host {0} has begun to transmit {1} from host {2} to host {3}'.format(event.host_id, frame, frame.source_host_id, frame.dest_host_id)

            if (event_type == TransmissionCompletion):
                host = self.hosts[event.host_id]
                frame = event.frame

                if (frame == None):
                    if DEBUG:
                        print 'last_frame for completing transmission on host {host} is over'.format(host=host.host_id)

                self.transmitting.remove(frame)
                frame_type = type(frame)

                if ((frame_type == DataFrame) and (not frame.corrupted)):
                    # Enqueue an ack frame and attempt to transmit it
                    # sending host is set to a waiting state
                    #   no frames can be sent until ack releases lock
                    ack_response = AckFrame(frame.dest_host_id, frame.source_host_id, frame)
                    self.hosts[frame.dest_host_id].enqueue_frame(ack_response)

                    self.events.put(TransmissionStart(self.time + SIFS, frame.dest_host_id))

                if ((frame_type == AckFrame) and (not frame.corrupted)):
                    # Acknowledge successful reciept of data frame
                    # receiving host (which originally sent data) is not waiting

                    host = self.hosts[frame.dest_host_id]
                    host.acknowledge(event.frame.data_frame)
                    host.reset_backoff()

                    self.events.put(TransmissionAttempt(self.time, host.host_id))

                if DEBUG:
                    print 'Transmission of {0} has completed. It was{1} corrupted.'.format(frame, '' if frame.corrupted else ' not')

            if (event_type == TransmissionTimeout):
                host = self.hosts[event.host_id]
                frame = event.frame

                if frame in host.sent_frames:
                    host.resend_frame(frame)
                    host.start_backoff()

                    if DEBUG:
                        print '{0} from host {1} timed out.'.format(frame, host.host_id)


            print

network = Network(10, 0.9, 0.5)
network.simulate(100000)
