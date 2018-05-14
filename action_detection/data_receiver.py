import os
from socket import *
import time
import numpy as np
host = ""
port = 13000

addr = (host, port)
UDPSock = socket(AF_INET, SOCK_DGRAM)
UDPSock.bind(addr)


# 获取初始buff大小
bufsize = UDPSock.getsockopt(SOL_SOCKET, SO_RCVBUF)
print("Buffer size [Before] :%d" %bufsize)
# 更改buff大小
UDPSock.setsockopt(SOL_SOCKET,SO_RCVBUF,4049)
# 获取更新后的buff大小
bufsize = UDPSock.getsockopt(SOL_SOCKET, SO_RCVBUF)
print("Buffer size [After] :%d" %bufsize)


data_x = np.zeros([1, 1, 75], dtype=np.float32)

print("Waiting to receive messages...")
while True:

  # 得到数据
  (data, addr) = UDPSock.recvfrom(4049)
  data_string = data.decode()

  # 把数据转换为list
  data_string = data_string[1:-1]
  values = data_string.rstrip().split(',')
  values = [float(x) for x in values]

  # 把list转换为numpyarray
  values = np.array(values, dtype=np.float32)
  data_x[0][0] = values

  print(data_x)

  if data == "exit":
    break


UDPSock.close()
os._exit(0)