import random
import numpy as np

# k_flods = 2
# k_flods = 3
k_flods = 6
# k_flods = 9

n_class_1 = 12
n_class_2 = 6
n_class_3 = 6
n_class_4 = 6

filename_list_x=[       "停顿1.txt",
                        "停顿2.txt",
                        "停顿3.txt",
                        "停顿4.txt",
                        "停顿5.txt",
                        "停顿6.txt",
                        "停顿7.txt",
                        "停顿8.txt",
                        "停顿9.txt",
                        "停顿10.txt",
                        "停顿11.txt",
                        "停顿12.txt",
                        # "停顿13.txt",
                        # "停顿14.txt",
                        # "停顿15.txt",
                        # "停顿16.txt",
                        # "停顿17.txt",
                        # "停顿18.txt",

                        "加油1.txt",
                        "加油2.txt",
                        "加油3.txt",
                        "加油4.txt",
                        "加油5.txt",
                        "加油6.txt",
                        # "加油7.txt",
                        # "加油8.txt",
                        # "加油9.txt",

                        "打招呼1.txt",
                        "打招呼2.txt",
                        "打招呼3.txt",
                        "打招呼4.txt",
                        "打招呼5.txt",
                        "打招呼6.txt",
                        # "打招呼7.txt",
                        # "打招呼8.txt",
                        # "打招呼9.txt",

                        "敬礼1.txt",
                        "敬礼2.txt",
                        "敬礼3.txt",
                        "敬礼4.txt",
                        "敬礼5.txt",
                        "敬礼6.txt",
                        # "敬礼7.txt",
                        # "敬礼8.txt",
                        # "敬礼9.txt",
                        ]

filename_list_1 = [ 0 for _ in range(n_class_1)]
filename_list_2 = [ 1 for _ in range(n_class_2)]
filename_list_3 = [ 2 for _ in range(n_class_3)]
filename_list_4 = [ 3 for _ in range(n_class_4)]

filename_list_y=[]

filename_list_y.extend(filename_list_1)
filename_list_y.extend(filename_list_2)
filename_list_y.extend(filename_list_3)
filename_list_y.extend(filename_list_4)


def read_one_file(file_name):
    state_flag = 0
    pause = []          # 10个动作序列
    pause_a = [ ]       # 一个动作序列

    with open(file_name, 'r') as f:
        lines = f.readlines()

    for idx, values in enumerate(lines):

        if state_flag==0:       #去除开始的准数据
            if len(values)<10:
                state_flag=1

        if state_flag==1:       #去除分隔数据
            if len(values)>10:
                state_flag=2

        if state_flag==2:
            if len(values)>10:

                values = values.rstrip().split(',')[0:75]
                values = [float(x) for x in values]
                pause_a.append(values)

            if len(values)<10:
                pause.append(pause_a)
                pause_a = []
                state_flag = 1

    pause.append(pause_a)
    return pause

def read_files(filename_list):
    actions=[]
    for file_name in filename_list:
        actions.append(read_one_file(file_name))
    return actions

def load_data_shuffle(filename_list_x,filename_list_y):

    actions = read_files(filename_list_x)

    file_len = len(actions)

    train_x = []
    train_y = []
    
    for i in range(10):#每个文件都有10个动作,通过10个阶段加载

        rand_list = np.random.permutation(file_len)# 生成这一阶段加载文件的顺序

        for j in range(file_len):#每个阶段都有n个文件可选
            
            choose = rand_list[j]# 第j步选第choose个文件
            action_len = len( actions[choose][i] )
            train_x.extend(actions[choose][i])

            if filename_list_y[choose] != 0:
                train_y.extend([ 4 for k in range(action_len // 3) ])  # 动作准备阶段
                train_y.extend([ filename_list_y[choose] for k in range( action_len - action_len // 3) ]) # 动作执行阶段
            else:
                train_y.extend([ filename_list_y[choose] for k in range( action_len ) ]) # 动作执行阶段

    return train_x,train_y

def load_data_all():

    # 生成随机选择的数组
    choose_1 = np.random.permutation(n_class_1)
    choose_2 = np.random.permutation(n_class_2)
    choose_3 = np.random.permutation(n_class_3)
    choose_4 = np.random.permutation(n_class_4)

    filename_1 = filename_list_x[0                              :n_class_1]
    filename_2 = filename_list_x[n_class_1                      :n_class_1+n_class_2]
    filename_3 = filename_list_x[n_class_1+n_class_2            :n_class_1+n_class_2+n_class_3]
    filename_4 = filename_list_x[n_class_1+n_class_2+n_class_3  :n_class_1+n_class_2+n_class_3+n_class_4]

    y_1 = filename_list_y[0                              :n_class_1]
    y_2 = filename_list_y[n_class_1                      :n_class_1+n_class_2]
    y_3 = filename_list_y[n_class_1+n_class_2            :n_class_1+n_class_2+n_class_3]
    y_4 = filename_list_y[n_class_1+n_class_2+n_class_3  :n_class_1+n_class_2+n_class_3+n_class_4]

    filename_list_train_x = []
    filename_list_train_y = []
    filename_list_test_x = []
    filename_list_test_y = []

    for c in range(n_class_1):
        if c < n_class_1//k_flods:  #得到测试划分
            filename_list_test_x.append( filename_1[ choose_1[c] ] )
            filename_list_test_y.append( y_1[ choose_1[c] ])
        else:                       #得到训练划分
            filename_list_train_x.append(filename_1[choose_1[c]])
            filename_list_train_y.append( y_1[ choose_1[c] ] )

    for c in range(n_class_2):
        if c < n_class_2//k_flods:  
            filename_list_test_x.append( filename_2[ choose_2[c] ] )
            filename_list_test_y.append( y_2[ choose_2[c] ])
        else:                      
            filename_list_train_x.append( filename_2[ choose_2[c] ] )
            filename_list_train_y.append( y_2[ choose_2[c] ])

    for c in range(n_class_3):
        if c < n_class_3//k_flods: 
            filename_list_test_x.append( filename_3[ choose_3[c] ] )
            filename_list_test_y.append( y_3[ choose_3[c] ])
        else:
            filename_list_train_x.append(filename_3[choose_3[c]])
            filename_list_train_y.append( y_3[ choose_3[c] ] )

    for c in range(n_class_4):
        if c < n_class_4//k_flods: 
            filename_list_test_x.append( filename_4[ choose_4[c] ] )
            filename_list_test_y.append( y_4[ choose_4[c] ])
        else:
            filename_list_train_x.append(filename_4[choose_4[c]])
            filename_list_train_y.append( y_4[ choose_4[c] ] )

    print("测试数据为：")
    print(filename_list_test_x)

    train_x, train_y = load_data_shuffle(filename_list_train_x,filename_list_train_y)
    test_x,test_y = load_data_shuffle(filename_list_test_x,filename_list_test_y)
    return train_x,train_y,test_x,test_y






# ==============================================================
# 旧代码

def load_data(file_name_list):

    pause1,pause2,pause3,fighting,hello,salute=read_files(file_name_list)

    action_list = [fighting,hello,salute]

    train_x = []
    train_y = []

    a_rand = 0
    b_rand = 0
    c_rand = 0

    for i in range(10):
        while True:
            a_rand = random.randint(0, 2)
            b_rand = random.randint(0, 2)
            c_rand = random.randint(0, 2)
            if a_rand != b_rand and b_rand != c_rand and c_rand != a_rand:
                break

        # 第一个间隙+动作
        train_x.extend(pause1[i])
        train_y.extend([ 0 for i in range(len(pause1[i] ))  ])

        train_x.extend(action_list[a_rand][i])
        train_y.extend([a_rand+1 for i in range(len(action_list[a_rand][i])) ])

        # 第二个间隙+动作
        train_x.extend(pause2[i])
        train_y.extend([0 for i in range(len(pause2[i]))])

        train_x.extend(action_list[b_rand][i])
        train_y.extend([b_rand+1 for i in range(len(action_list[b_rand][i])) ])

        # 第三个间隙+动作
        train_x.extend(pause3[i])
        train_y.extend([0 for i in range(len(pause3[i]))])

        train_x.extend(action_list[c_rand][i])
        train_y.extend([c_rand+1 for i in range(len(action_list[c_rand][i])) ])

    return train_x,train_y

def load_data_no_shuffle(file_name_list):

    pause1,pause2,pause3,fighting,hello,salute=read_files(file_name_list)

    action_list = [fighting,hello,salute]

    train_x = []
    train_y = []

    for i in range(10):

        # 第一个间隙+动作
        train_x.extend(pause1[i])
        train_y.extend([ 0 for i in range( len(pause1[i]) )  ])

        train_x.extend(fighting[i])
        train_y.extend([1 for i in range(len(fighting[i])) ])

        # 第二个间隙+动作
        train_x.extend(pause2[i])
        train_y.extend([0 for i in range(len(pause2[i]))])

        train_x.extend(hello[i])
        train_y.extend([2 for i in range(len(hello[i])) ])

        # 第三个间隙+动作
        train_x.extend(pause3[i])
        train_y.extend([0 for i in range(len(pause3[i]))])

        train_x.extend(salute[i])
        train_y.extend([3 for i in range(len(salute[i])) ])

    return train_x,train_y
