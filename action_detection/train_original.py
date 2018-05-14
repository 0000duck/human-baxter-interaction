
import tensorflow as tf
import tensorlayer as tl
from tensorlayer.layers import set_keep
import numpy as np
import time
from load_dataset import load_data_all
from next_batch import next_batch

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
        max_grad_norm = 5
        hidden_size = 200
        max_epoch = 40
        max_max_epoch = 80
        keep_prob = 1.0
        lr_decay = 0.99
        class_size = 4
        num_steps = 80
        batch_size = 2
    else:
        raise ValueError("Invalid model: %s", FLAGS.model)

    # train_data_x = [3754,75]
    # train_data_y = [3754,1]
    train_data_x,train_data_y,test_data_x,test_data_y  = load_data_all()

    print('len(train_data) {}'.format(len(train_data_x)))
    print('len(test_data) {}'.format(len(test_data_x)))


    sess = tf.InteractiveSession()

    # Training and Validing
    # input_data = [5,20,75]
    # targets = [5,20]
    input_data = tf.placeholder(tf.float32, [batch_size, num_steps, skeleton_size])
    targets = tf.placeholder(tf.int32, [batch_size, num_steps])

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

        return cost,acc


    # Inference for Training
    network, lstm1, lstm2 = inference(input_data, is_training=True, num_steps=num_steps, reuse=None)
    # Inference for Validating
    network_val, lstm1_val, lstm2_val = inference(input_data, is_training=False, num_steps=num_steps, reuse=True)

    # Cost for Training
    cost,acc = loss_fn(network.outputs, targets) 
    # Cost for Validating
    cost_val,acc_val = loss_fn(network_val.outputs, targets)#, batch_size, num_steps)


    with tf.variable_scope('learning_rate'):
        lr = tf.Variable(0.0, trainable=False)


    optimizer = tf.train.AdadeltaOptimizer(learning_rate=lr, rho=0.95, epsilon=1e-8)
    all_vars = tf.trainable_variables()
    train_op = optimizer.minimize(loss=cost, var_list=all_vars)

    tl.layers.initialize_global_variables(sess)

    # network.print_params()
    # network.print_layers()
    # tl.layers.print_all_variables()

    print("\nStart learning a language model by using PTB dataset")
    for i in range(max_max_epoch):
        new_lr_decay = lr_decay ** max(i - max_epoch, 0.0)
        sess.run(tf.assign(lr, learning_rate * new_lr_decay))

        # Training
        print("Epoch: %d/%d Learning rate: %.3f" % (i + 1, max_max_epoch, sess.run(lr)))

        # 计算每个epoch可以训练多少次
        epoch_size = (len(train_data_x) // batch_size)  // num_steps


        start_time = time.time()
        costs = 0.0
        accurate = 0.0
        iters = 0

        #=====================================================
        # Training
        state1 = tl.layers.initialize_rnn_state(lstm1.initial_state)
        state2 = tl.layers.initialize_rnn_state(lstm2.initial_state)
        # 开始每一个epoch内的循环迭代
        for step, (x, y) in enumerate(next_batch(train_data_x,train_data_y,
                                                              batch_size, num_steps)):

            feed_dict = {input_data: x, targets: y,
                         lstm1.initial_state: state1,
                         lstm2.initial_state: state2,
                         }

            feed_dict.update(network.all_drop)

            # 训练 ！！！！！！！！
            _cost, _acc,state1, state2, _ = sess.run([cost,
                                                    acc,
                                                    lstm1.final_state,
                                                    lstm2.final_state,
                                                    train_op,],
                                                    feed_dict=feed_dict  )
            costs += _cost
            accurate += _acc
            iters += 1

            # print("step: %d cost: %f acc: %f" % ( step, _cost, _acc ) ) 

        print("train Epoch: %d/%d , coss: %f , acc: %f, time %f" % (i + 1, max_max_epoch,
                                                       costs / iters,
                                                       accurate / iters,
                                                       time.time()-start_time))
        #=====================================================
        # Validing

        start_time = time.time()
        costs = 0.0
        accurate = 0.0
        iters = 0

        # reset all states at the begining of every epoch
        state1 = tl.layers.initialize_rnn_state(lstm1_val.initial_state)
        state2 = tl.layers.initialize_rnn_state(lstm2_val.initial_state)

        for step, (x, y) in enumerate(next_batch(test_data_x,test_data_y,
                                                              batch_size, num_steps)):
            feed_dict = {input_data: x, targets: y,
                        lstm1_val.initial_state: state1,
                        lstm2_val.initial_state: state2,
                        }


            _cost, _acc,state1, state2 = sess.run([cost_val,
                                            acc_val,
                                            lstm1_val.final_state,
                                            lstm2_val.final_state],
                                            feed_dict=feed_dict
                                            )
            costs += _cost
            accurate += _acc
            iters += 1

            # print("step: %d cost: %f acc: %f" % ( step, _cost, _acc ) )

        print("test Epoch: %d/%d , coss: %f , acc: %f, time %f" % (i + 1, max_max_epoch,
                                                       costs / iters,
                                                       accurate / iters,
                                                       time.time()-start_time))





if __name__ == "__main__":
    tf.app.run()

