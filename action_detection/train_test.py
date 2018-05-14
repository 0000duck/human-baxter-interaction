
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

    # train_data_x = [3754,75]
    # train_data_y = [3754,1]
    train_data_x,train_data_y,test_data_x,test_data_y  = load_data_all()

    print('len(train_data) {}'.format(len(train_data_x)))
    print('len(test_data) {}'.format(len(test_data_x)))


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

    # Training and Validing
    input_data = tf.placeholder(tf.float32, [batch_size, num_steps, skeleton_size])
    targets = tf.placeholder(tf.int32, [batch_size, num_steps])
    # Testing (Evaluation)
    input_data_test = tf.placeholder(tf.float32, [1, 1, skeleton_size])
    targets_test = tf.placeholder(tf.int32, [1, 1])


    # Inference for Training
    network, lstm1, lstm2 = inference(input_data, is_training=True, num_steps=num_steps, reuse=None)
    # Inference for Validating
    network_val, lstm1_val, lstm2_val = inference(input_data, is_training=False, num_steps=num_steps, reuse=True)
    # Inference for Testing (Evaluation)
    network_test, lstm1_test, lstm2_test = inference(input_data_test,
                            is_training=False, num_steps=1, reuse=True)

    # Cost for Training
    cost, acc, _ = loss_fn(network.outputs, targets) 
    # Cost for Validating
    cost_val, acc_val, _ = loss_fn(network_val.outputs, targets) 
    # Cost for Testing (Evaluation)
    cost_test, acc_test, y_op  = loss_fn(network_test.outputs, targets_test) 

    # ======实际的建模由这里结束======

    with tf.variable_scope('learning_rate'):
        lr = tf.Variable(0.0, trainable=False)


    optimizer = tf.train.AdadeltaOptimizer(learning_rate=lr, rho=0.95, epsilon=1e-8)
    all_vars = tf.trainable_variables()
    train_op = optimizer.minimize(loss=cost, var_list=all_vars)

    tl.layers.initialize_global_variables(sess)

    # network.print_params()
    # network.print_layers()
    # tl.layers.print_all_variables()

    print("\nStart learning a action detection and classification model by using THU skeleton dataset...")
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
            _cost, _acc, state1, state2, _ = sess.run([cost,
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


            _cost, _acc, state1, state2= sess.run([cost_val,
                                                    acc_val,
                                                    lstm1_val.final_state,
                                                    lstm2_val.final_state
                                                    ],
                                                    feed_dict=feed_dict
                                                )
            costs += _cost
            accurate += _acc
            iters += 1

            # print("step: %d cost: %f acc: %f" % ( step, _cost, _acc ) )

        print("val Epoch:  %d/%d , coss: %f , acc: %f, time %f" % (i + 1, max_max_epoch,
                                                       costs / iters,
                                                       accurate / iters,
                                                       time.time()-start_time))


    print("Evaluation")
    # Testing
    # go through the test set step by step, it will take a while.

    start_time = time.time()
    true_list = []     
    prediction_list = []
    accurate = 0.0
    iters = 0
    # reset all states at the begining
    state1 = tl.layers.initialize_rnn_state(lstm1_test.initial_state)
    state2 = tl.layers.initialize_rnn_state(lstm2_test.initial_state)

    for step, (x, y) in enumerate(next_batch(test_data_x,test_data_y,
                                                              batch_size=1, num_steps=1)):
        # one_fram_start_time = time.time()
        feed_dict = {input_data_test: x, 
                    targets_test: y,
                    lstm1_test.initial_state: state1,
                    lstm2_test.initial_state: state2,
                    }
        _acc, _y, state1, state2 = sess.run([
                                        acc_test, 
                                        y_op,
                                        lstm1_test.final_state,
                                        lstm2_test.final_state],
                                        feed_dict=feed_dict
                                        )
        accurate += _acc
        iters += 1

        y = y.tolist()      # y 是   [batch_size           , num_steps]
        _y = _y.tolist()    # _y 是  [batch_size*num_steps ,          ]

        y = y[0]

        true_list.extend(y)
        prediction_list.extend(_y)

        # print("time %f" % (time.time()-one_fram_start_time))


    print("test acc: %f, time %f" % (accurate/iters,
                                     time.time()-start_time))

    print("y_truth     : %s" % (true_list))

    print("y_prediction: %s" % (prediction_list))

    tl.files.save_npz(network_test.all_params, name='model_test.npz')
    network_test.print_params()


if __name__ == "__main__":
    tf.app.run()


















