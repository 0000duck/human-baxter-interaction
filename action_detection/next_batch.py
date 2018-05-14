import numpy as np

def next_batch(raw_data_x,raw_data_y, batch_size, num_steps):

    raw_data_x = np.array(raw_data_x, dtype=np.float32)
    raw_data_y = np.array(raw_data_y, dtype=np.int64)

    data_len = len(raw_data_x)
    batch_len = data_len // batch_size

    data_x = np.zeros([batch_size, batch_len, 75], dtype=np.float32)
    data_y = np.zeros([batch_size, batch_len], dtype=np.int64)

    for i in range(batch_size):
        data_x[i] = raw_data_x[batch_len * i : batch_len * (i + 1)]
        data_y[i] = raw_data_y[batch_len * i: batch_len * (i + 1)]

    epoch_size = batch_len  // num_steps

    if epoch_size == 0:
        raise ValueError("epoch_size == 0, decrease batch_size or num_steps")

    for i in range(epoch_size):
        x = data_x[:, i*num_steps:(i+1)*num_steps,:]
        y = data_y[:, i*num_steps:(i+1)*num_steps]
        yield (x, y)
