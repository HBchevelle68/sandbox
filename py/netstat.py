import socket
import struct


tcp_states = [
    'TCP_ESTABLISHED',
    'TCP_SYN_SENT',
    'TCP_SYN_RECV',
    'TCP_FIN_WAIT1',
    'TCP_FIN_WAIT2',
    'TCP_TIME_WAIT',
    'TCP_CLOSE',
    'TCP_CLOSE_WAIT',
    'TCP_LAST_ACK',
    'TCP_LISTEN',
    'TCP_CLOSING',    
    'TCP_NEW_SYN_RECV',
    'TCP_MAX_STATES'
]

with open('/proc/net/tcp', 'r') as proc_fd: 
    
    lines = proc_fd.readlines()
    print(lines)

    for line in lines:
        split_lines = line.split()
        print(split_lines)

    del lines[0]
    print()
    for line in lines:
        split_lines = line.split()
        #print(split_lines)
        
        local_ip,local_port = split_lines[1].split(':')
        rem_ip, rem_port = split_lines[2].split(':')
        state = int(split_lines[3],16)
        tx_queue = int(split_lines[4],16)
        rx_queue = int(split_lines[5],16)


        #print(f'{local_ip}:{local_port}')
        local_ip = socket.inet_ntoa(struct.pack("<L",int(local_ip,16)))

        print(f'Local: {local_ip}:{int(local_port,16)}'
        f'  State:{tcp_states[int(split_lines[3],16)]}')


