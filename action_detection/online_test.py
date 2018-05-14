import tensorflow as tf
import tensorlayer as tl
from tensorlayer.layers import set_keep
import numpy as np
import time
from load_dataset import load_data_all
from next_batch import next_batch
from send_cmd import *

import os
from socket import *
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

flags = tf.flags
flags.DEFINE_string(
    "model", "small",
    "A type of model. Possible options are: small, medium, large.")
FLAGS = flags.FLAGS


def main(_):
    if FLAGS.model == "small":
        skeleton_size = 75
        init_scale = 0.1
        learning_rate = 1.0
        hidden_size = 200
        max_epoch = 30
        max_max_epoch = 80
        keep_prob = 1.0
        lr_decay = 0.99

        class_size = 5
        num_steps = 80
        batch_size = 5

    else:
        raise ValueError("Invalid model: %s", FLAGS.model)


    sess = tf.InteractiveSession()

    # x = input_data = [5,20,75]
    # output = [100,4]
    def inference(x, is_training, num_steps, reuse=None):
        
        print("\nnum_steps : %d, is_training : %s, reuse : %s" %
              (num_steps, is_training, reuse))
        initializer = tf.random_uniform_initializer(-init_scale, init_scale)
        with tf.variable_scope("model", reuse=reuse):
            tl.layers.set_name_reuse(reuse)


            input_layer = tl.layers.InputLayer(inputs=x, name="input_layer")


            B, T, D = input_layer.outputs.get_shape().as_list()

            network = tl.layers.ReshapeLayer(layer=input_layer,
                                             shape=[B * T, D],
                                             name="reshape1")

            network = tl.layers.DenseLayer(layer=network,
                                           n_units=128,
                                           act=tf.nn.relu,
                                           name="fc1")

            network = tl.layers.ReshapeLayer(layer=network,
                                             shape=[B, T, 128],
                                             name="reshape2")

            # 第一层LSTM的输出形状是 [batch_size, num_steps, hidden_size]，
            # 这是为了让下一层LSTM可以堆叠在其上面。
            network = tl.layers.RNNLayer(network,
                                         cell_fn=tf.contrib.rnn.BasicLSTMCell,  # tf.nn.rnn_cell.BasicLSTMCell,
                                         cell_init_args={'forget_bias': 0.0},  # 'state_is_tuple': True},
                                         n_hidden=hidden_size,
                                         initializer=initializer,
                                         n_steps=num_steps,
                                         return_last=False,
                                         name='basic_lstm_layer1')

            lstm1 = network

            network = tl.layers.DropoutLayer(network,
                                             keep=keep_prob,
                                             is_fix=True,
                                             is_train=is_training,
                                             name='drop1')


            # 第二层LSTM的输出形状是 [batch_size*num_steps, hidden_size]，
            # 这是为了让输出层（全连接层 Dense）可以堆叠在其上面。
            network = tl.layers.RNNLayer(network,
                                         cell_fn=tf.contrib.rnn.BasicLSTMCell,
                                         cell_init_args={'forget_bias': 0.0},
                                         n_hidden=hidden_size,
                                         initializer=initializer,
                                         n_steps=num_steps,
                                         return_last=False,
                                         return_seq_2d=True,
                                         name='basic_lstm_layer2')
            lstm2 = network

            network = tl.layers.DropoutLayer(network,
                                             keep=keep_prob,
                                             is_fix=True,
                                             is_train=is_training,
                                             name='drop2')

            network = tl.layers.DenseLayer(layer=network,
                                           n_units=64,
                                           act=tf.nn.relu,
                                           name="fc2")

            network = tl.layers.DenseLayer(network,
                                           n_units=class_size,
                                           act=tf.identity,
                                           name='output')

            # output = [100,4]

        return network, lstm1, lstm2

    # outputs : 2D tensor [batch_size*num_steps, n_units]
    # targets : 2D tensor [batch_size, num_steps]
    def loss_fn(outputs, targets):  # , batch_size, num_steps):

        targets = tf.reshape(targets, [-1])
        
        cost = tl.cost.cross_entropy(outputs,targets , name='cost')

        correct_prediction = tf.equal(tf.argmax(outputs, 1, output_type=tf.int32), targets)
        acc = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

        y_op = tf.argmax(tf.nn.softmax(outputs), 1)

        return cost, acc, y_op

    # ======实际的建模由这里开始======

    # Testing (Evaluation)
    input_data_test = tf.placeholder(tf.float32, [1, 1, skeleton_size])
    targets_test = tf.placeholder(tf.int32, [1, 1])


    # Inference for Testing (Evaluation)
    network_test, lstm1_test, lstm2_test = inference(input_data_test,
                            is_training=False, num_steps=1, reuse=None)


    cost_test, acc_test, y_op  = loss_fn(network_test.outputs, targets_test) 

    # ======实际的建模由这里结束======

    with tf.variable_scope('learning_rate'):
        lr = tf.Variable(0.0, trainable=False)

    # 加载之前训练好的参数
    load_params = tl.files.load_npz(name='model_87.npz')
    tl.files.assign_params(sess, load_params, network_test)
    network_test.print_params()


    print("Evaluation")

    start_time = time.time()
    prediction_list = []
    
    # reset all states at the begining
    state1 = tl.layers.initialize_rnn_state(lstm1_test.initial_state)
    state2 = tl.layers.initialize_rnn_state(lstm2_test.initial_state)

    action_over = 1
    last_action = 0
    i = -1

    while(1):

        (data, addr) = UDPSock.recvfrom(4049)
        data_string = data.decode()

        # 把数据转换为list
        data_string = data_string[1:-1]
        values = data_string.rstrip().split(',')
        values = [float(x) for x in values]

        # 把list转换为numpy.array
        values = np.array(values, dtype=np.float32)
        data_x[0][0] = values

        # 预测
        x = data_x
        feed_dict = {input_data_test: x, 
                    lstm1_test.initial_state: state1,
                    lstm2_test.initial_state: state2,
                    }

        _y, state1, state2 = sess.run([ y_op,
                                        lstm1_test.final_state,
                                        lstm2_test.final_state],
                                        feed_dict=feed_dict
                                        )
        

        _y = _y.tolist()    # _y 是  [batch_size*num_steps , ]
        prediction_list.extend(_y)

        # 动作状态转换处理流程
        i = i + 1
        if last_action == 0:
            if prediction_list[i] in [1, 2, 3]:
                last_action = prediction_list[i]
                print(prediction_list[i])
                action_over = 0
            else:
                continue

        if (prediction_list[i] != last_action or action_over == 1) and (prediction_list[i] in [1, 2, 3]):
        # if ( action_over == 1) and (prediction_list[i] in [1, 2, 3]):
            last_action = prediction_list[i]
            # print(prediction_list[i])
            if prediction_list[i] == 1:
                send_cmd("fight")
                print("加油！！加油！！加油！！加油！！")
                print("")
                print("")
            elif prediction_list[i] == 2:
                send_cmd("hello")
                print("打招呼 打招呼 打招呼 打招呼 打招呼 打招呼 打招呼 打招呼 打招呼 打招呼 ")
                print("")
                print("")
            elif prediction_list[i] == 3:
                send_cmd("salute")
                print("敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 敬礼 ")
                print("")
                print("")

            action_over = 0
            continue

        if action_over == 0 and prediction_list[i] == 4:
            action_over = 1
            continue



if __name__ == "__main__":
    tf.app.run()
    UDPSock.close()
    os._exit(0)


















