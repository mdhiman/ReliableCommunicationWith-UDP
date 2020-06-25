# ReliableCommunicationWith-UDP
Assume that the TCP sockets are not there. We know that UDP sockets are not reliable, meaning that there is no guarantee that a message sent using a UDP socket will reach the receiver. We want to implement our own socket type, called MRP (My Reliable Protocol) socket, that will guarantee that any message sent using a MRP socket is always delivered to the receiver. However, unlike TCP sockets, MRP sockets may not guarantee in-order delivery or exactly-once delivery of messages. Thus messages may be delivered more than once (duplicate messages) or out of order (later message delivered earlier).
